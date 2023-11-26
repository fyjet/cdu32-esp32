This the ESP32 firmware for FSBridge project: https://github.com/fyjet/fsbridge

# Cabling
ESP32 GPIO    TFT display ILI9341  
 15 (pin23)           TFT_CS  
 4  (pin26)           TFT_RESET  
 2  (pin24)           TFT_DC  
 23 (pin37)           TFT_SDI/MOSI  
 18 (pin30)           TFT_SCK  
 3.3v                 TFT_LED  
 19 (pin31)           TFT_SDO/MISO  
 18 (pin30)           T_CLK  
 25 (pin 9)           T_CS  
 23 (pin37)           T_DIN  
 19 (pin31)           T_DO  
 26 (pin10)           T_IRQ (used for fast TS readings)  
 
ESP32 GPIO    encoders  
 13 (pin15)           ENC1 A  
 12 (pin13)           ENC1 B  
 GND                  ENC1 COMMON  
 14 (pin12)           ENC2 A  
 27 (pin11)           ENC2 B  
 GND                  ENC2 COMMON  
 
ESP32 GPIO    buttons  
 32 (pin 7)           button1  
 GND                  button1 COMMON  
 33 (pin 8)           button2  
 GND                  button2 COMMON  
  5 (pin29)           button3 auto pilot  
 GND                  button3 COMMON  
 
ESP32 GPIO    lights  
 17 (pin28)           LED autopilot  
 GND                  LED COMMUN  
 
ESP32 GPIO    buzzer  
 16 (pin27)           buzzer+  
 GND                  buzzer-  
 
Configuration is done in include/config.h  
  Network  
  MQTT  
 