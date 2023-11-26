/*
 *  ESP32 npanel
 *
 * ESP32 GPIO    TFT display ILI9341
 * 15 (pin23)           TFT_CS
 * 4  (pin26)           TFT_RESET
 * 2  (pin24)           TFT_DC
 * 23 (pin37)           TFT_SDI/MOSI
 * 18 (pin30)           TFT_SCK
 * 3.3v                 TFT_LED
 * 19 (pin31)           TFT_SDO/MISO
 * 18 (pin30)           T_CLK
 * 25 (pin 9)           T_CS
 * 23 (pin37)           T_DIN
 * 19 (pin31)           T_DO
 * 26 (pin10)           T_IRQ (used for fast TS readings)
 *
 * ESP32 GPIO    encoders
 * 13 (pin15)           ENC1 A
 * 12 (pin13)           ENC1 B
 * GND                  ENC1 COMMON
 * 14 (pin12)           ENC2 A
 * 27 (pin11)           ENC2 B
 * GND                  ENC2 COMMON
 *
 * ESP32 GPIO    buttons
 * 32 (pin 7)           button1
 * GND                  button1 COMMON
 * 33 (pin 8)           button2
 * GND                  button2 COMMON
 *  5 (pin29)           button3 auto pilot
 * GND                  button3 COMMON
 *
 * ESP32 GPIO    lights
 * 17 (pin28)           LED autopilot
 * GND                  LED COMMUN
 *
 * ESP32 GPIO    buzzer
 * 16 (pin27)            buzzer+
 * GND                  buzzer-
 *
 * Configuration is done in include/config.h
 *   Network
 *   MQTT
 *
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <MD_REncoder.h>
#include <EasyButton.h>
#include <sstream>
#include "Display.h"
#include "const.h"
#include "config.h"

using namespace std;

const char*   ssid          = CONFIG_WIFI_SSID;
const char*   password      = CONFIG_WIFI_PASSWORD;
const char*   mqttServer    = CONFIG_MQTT_IP;
const int     mqttPort      = CONFIG_MQTT_PORT;

WiFiClient    ethClient;
PubSubClient  client(ethClient);
TFT_eSPI      tft           = TFT_eSPI();

volatile int  touchPressed  = 0;                      //updated by ISR function when touch controller is active

Display       display(&tft);

MD_REncoder   R1            = MD_REncoder(12, 13);
MD_REncoder   R2            = MD_REncoder(14,27);
EasyButton    button1(33);
EasyButton    button2(32);
EasyButton    button3(5);

unsigned long wdt;                                    // watchdog timer to detect MQTT link loss to bridge
bool          fsbridgeLink  = false;                  // state of link to device
bool          fsbridgeSync  = false;                  // synchro with fsbridge (if reset)

#define       FREQ            1500                    // buzzer frequency in Hz
#define       SOUND_PWM_CHANNEL 0
#define       SOUND_RESOLUTION  8                     // 8 bit resolution
#define       SOUND_ON        (1<<(SOUND_RESOLUTION-1)) // 50% duty cycle
#define       SOUND_OFF       0                       // 0% duty cycle

void tone(int pin, int frequency, int duration)
{
  ledcSetup(SOUND_PWM_CHANNEL, frequency, SOUND_RESOLUTION);  // Set up PWM channel
  ledcAttachPin(pin, SOUND_PWM_CHANNEL);              // Attach channel to pin
  ledcWrite(SOUND_PWM_CHANNEL, SOUND_ON);
  delay(duration);
  ledcWrite(SOUND_PWM_CHANNEL, SOUND_OFF);
}

/*
 * Event manager: key pressed
 */
void manageKey(int keyPressed) {
  stringstream skp;
  skp << keyPressed;
  string m = skp.str();
  client.publish(TOPIC_E_K,m.c_str());
}

void onPressed1() {
    manageKey(BT1);
}
void onPressed2() {
    manageKey(BT2);
}

void onPressed3() {
    manageKey(BT3);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  string m="";
  if (strcmp(topic,TOPIC_C_D)==0) {
    wdt=millis(); // reset timer
    char operation=(char)payload[0];

    // New screen
    // N,title
    if (operation=='N') {
      for (unsigned int i = 2; i < length; i++) {
        m = m + (char)payload[i];
      }
      display.newPage(m.c_str());
      fsbridgeSync=true;
      fsbridgeLink=true;
    }

    // Display create or modify clickable text
    // T,XX,CC,B,Z,text
    if (operation=='T') {
      int place=(((char)payload[2]-48)*10)+(char)payload[3]-48;
      int color=(((char)payload[5]-48)*10)+(char)payload[6]-48;
      int boxed=(char)payload[8]-48;
      int size=(char)payload[10]-48;
      for (unsigned int i = 12; i < length; i++) {
        m = m + (char)payload[i];
      }
      display.addTouchKey(place,color,boxed,size,m.c_str());
    }

    // Create clickable button
    // B,XX,buttonText
    if (operation=='B') {
      int place=(((char)payload[2]-48)*10)+(char)payload[3]-48;
      for (unsigned int i = 5; i < length; i++) {
        m = m + (char)payload[i];
      }
      display.addTouchKey(place,97,1,3,m.c_str());
    }

    // Slider
    // S,XX,CC,B,value in percent
    if (operation=='S') {
      int place=(((char)payload[2]-48)*10)+(char)payload[3]-48;
      int color=(((char)payload[5]-48)*10)+(char)payload[6]-48;
      int boxed=(char)payload[8]-48;
      for (unsigned int i = 10; i < length; i++) {
        m = m + (char)payload[i];
      }
      //display.writeSlider(place,color,boxed,m.c_str());
      display.addTouchKey(place,color,boxed,0,m.c_str());
    }

    // LED
    // L,value (0 or 1)
    if (operation=='L') {
      for (unsigned int i = 2; i < length; i++) {
        m = m + (char)payload[i];
      }
      display.writeLED(m.c_str());
    }

    // BUZZER
    // X (no options)
    if (operation=='X') {
      tone(16,FREQ,100);
    }
  }

  // Return status and reset timer
  // status = 0 all is ok and synchronized
  // status = 1 fsbridge need to resent current page
  if (strcmp(topic,TOPIC_C_REQSTATUS)==0) {
    wdt=millis(); // reset timer
    if (fsbridgeSync) {
      client.publish(TOPIC_E_S,"0");
    } else {
      client.publish(TOPIC_E_S,"1");
    }
  }

  // Return freeHeap
  if (strcmp(topic,TOPIC_C_REQDIAG)==0) {
    int freeHeap=ESP.getFreeHeap();
    stringstream sFreeHeap;
    sFreeHeap << freeHeap;
    client.publish(TOPIC_E_S,sFreeHeap.str().c_str());
  }

}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32client")) {
      delay(1000);
    }
  }
  // subscribe to command subtopics
  client.subscribe(TOPIC_C);
  // send OK to client
  client.publish(TOPIC_E_S,"0");
}

/*
* Touch screen interruption
*/
void IRAM_ATTR isr() {
  touchPressed=1;
}

void setup() {

  //Serial.begin(9600);
  pinMode(26 ,INPUT_PULLUP );
  pinMode(17 ,OUTPUT );
  digitalWrite(17 ,1 );
  tone(16 ,FREQ ,100 );
  
  //tft.setFreeFont(&FreeSerif9pt7b);  // here to change font
  display.init();
  display.newPage("Connecting Wifi");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Retry...
  }
  //Wifi connected

  display.newPage("Connecting MQTT");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  client.setKeepAlive(1000);

  R1.begin();
  R2.begin();
  button1.begin();
  button2.begin();
  button3.begin();
  button1.onPressed(onPressed1);
  button2.onPressed(onPressed2);
  button3.onPressed(onPressed3);
  attachInterrupt(26, isr, FALLING); // touch screen interrupt
  
  display.newPage("Waiting FSBRIDGE");
  digitalWrite(17,0);

  wdt=millis(); // reset timer

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  if (fsbridgeLink && millis()-wdt>WDT_TIMEOUT) {
    display.newPage("Link lost, wait FSBRIDGE");
    digitalWrite(17,0);
    fsbridgeLink=false;
    fsbridgeSync=false;
  }

  // polling
  // encoders polling
  uint8_t x1 = R1.read();
  if (x1)
  {
    if (x1 == DIR_CW) {
      manageKey(INC1UP);
    } else {
      manageKey(INC1DN);
    }
  }

  uint8_t x2 = R2.read();
  if (x2)
  {
    if (x2 == DIR_CW) {
      manageKey(INC2UP);
    } else {
      manageKey(INC2DN);
    }
  }

  //  buttons polling
  button1.read();
  button2.read();
  button3.read();

  // touch screen interruption received
  if (touchPressed==1)
  {
    detachInterrupt(26);  // detach interrupt
    int d=display.poll();
    if (d!=-1)
    {
      // touch key beep
      tone(16,FREQ,4);
      manageKey(d);
    }
    touchPressed=0;
    attachInterrupt(26, isr, FALLING); // re-enable interrupt
  }

  // Network loop
  client.loop();
}
