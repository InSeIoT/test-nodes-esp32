#include "cert.h"

#include "device.h"
#ifndef __DEVICE_H__
   #define DEV_NAME "MyDevice"
   #define DEV_UUID "MyUUID"
#endif

#include "mqtt.h"
#ifndef __MQTT_H__
   #define MQTT_SERVER "mqtt.example.com"
   #define MQTT_PORT 8883
   #define MQTT_USER "mqtt_user"
   #define MQTT_PASS "mqtt_pass"
   #define MQTT_TOPIC "mqtt_topic"
#endif


#include "wifi.h"
#ifndef __WIFI_H__
   #define WIFI_SSID "MySSID"
   #define WIFI_PASS "MyPassword"
#endif

/* -------------------------------------------------------------------------- */
/*                              GPIO Definitions                              */
/* -------------------------------------------------------------------------- */
#define PIN_LED 5
#define PIN_SCK 18
#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_CS 15
#define PIN_INT1 4
#define PIN_INT2 0


/* -------------------------------------------------------------------------- */
/*                               Sampling Cache                               */
/* -------------------------------------------------------------------------- */
#define SAMPLES_CACHE 50
#define SAMPLES_TX 45
#define SAMPLES_CACHE_BLOCKS 2

#define PACKAGE_SIZE 8192

/* -------------------------------------------------------------------------- */
/*                              NTP(System Time)                              */
/* -------------------------------------------------------------------------- */
#define NTP_UPDATE 3600000 // Update every 1 hour