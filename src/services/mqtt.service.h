#include <Arduino.h>
#include "../config/config.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
}

/************* Connect to MQTT Broker ***********/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");

      client.subscribe("led_state");   // subscribe the topics here

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishMessage(const char* topic, String payload , boolean retained){
  if (!client.publish(topic, payload.c_str())){
    Serial.println("Error Sending!\n");
  } 
}

void mqtt_setup() {
  espClient.setCACert(root_ca);
  client.setServer(MQTT_SERVER, atoi(MQTT_PORT));
  client.setCallback(callback);
  if (!client.connected()) reconnect(); 
  client.setBufferSize(2048);
  Serial.println("MQTT Buffer Size: " + String(client.getBufferSize()));
  delay(500);
}