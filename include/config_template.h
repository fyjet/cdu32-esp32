/*
*  Template for configuration
*/

#define CONFIG_WIFI_SSID     "SSID"
#define CONFIG_WIFI_PASSWORD "password"
#define CONFIG_MQTT_IP       "IPOrHOstName"
#define CONFIG_MQTT_PORT     1883

/*
 *  Watchdog timer timeout for fsbrigde link, in seconds
 */
#define WDT_TIMEOUT          3000

/*
*  Change deviceid here, replace npanel by propervalue if needed
*/
#define TOPIC_C              "j/npanel/c/#"
#define TOPIC_C_D            "j/npanel/c/d"
#define TOPIC_C_REQSTATUS    "j/npanel/c/reqstatus"
#define TOPIC_C_REQDIAG      "j/npanel/c/reqdiag"
#define TOPIC_E_S            "j/npanel/e/s"
#define TOPIC_E_K            "j/npanel/e/k"
