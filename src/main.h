#define ARDUINOJSON_USE_LONG_LONG 1

#include <Arduino.h>
#include <WiFi.h>

#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <CurieIMU.h>

#include "./utils/WiMo/WiMo2.h"

#include "./services/mqtt.service.h"


static uint32_t sc_current_block = 0;
static uint32_t sc_samples = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiMo wimo;
IMU_RAW imu;
IMU imuc;

long long dataSamples[SAMPLES_CACHE_BLOCKS][SAMPLES_CACHE][8];
String outputStr;

TaskHandle_t TaskCore0;
void Task0Sampling(void *pvParameters);

void wifi_setup()
{
  Serial.println("Conectando a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    delay(100);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());

  delay(500);

  timeClient.begin();

  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

  timeClient.setUpdateInterval(NTP_UPDATE);
  
  delay(500);
}

