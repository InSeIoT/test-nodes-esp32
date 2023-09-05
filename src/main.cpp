#include "main.h"

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  wifi_setup();
  wimo.begin();
  mqtt_setup();

  Serial.print("IMU Setup... ");
  CurieIMU.begin(PIN_CS);
  Serial.print("Done!\n");

  xTaskCreatePinnedToCore(
      Task0Sampling,
      "TaskCore0",
      1000,
      NULL,
      0,
      &TaskCore0,
      0);

  CurieIMU.setIntDataReadyEnabled(true);
  digitalWrite(LED_BUILTIN, HIGH);
}

/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/** CORE 0 */
/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Task0Sampling(void *pvParameters)
{
  Serial.print("Task0Sampling running in Core: ");
  Serial.println(xPortGetCoreID());
  
  for (;;)
  {

    if (CurieIMU.getIntDataReadyStatus()) {
      int modSample = sc_samples % SAMPLES_CACHE;
      CurieIMU.readMotionSensor(imu.acc.x, imu.acc.y, imu.acc.z, imu.gyro.x, imu.gyro.y, imu.gyro.z);
      imu.t = CurieIMU.readTemperature();
      imuc = wimo.correction(imu);
      dataSamples[sc_current_block][modSample][0] = timeClient.getEpochMillis();
      dataSamples[sc_current_block][modSample][1] = imuc.acc.x;
      dataSamples[sc_current_block][modSample][2] = imuc.acc.y;
      dataSamples[sc_current_block][modSample][3] = imuc.acc.z;
      dataSamples[sc_current_block][modSample][4] = imuc.gyro.x;
      dataSamples[sc_current_block][modSample][5] = imuc.gyro.y;
      dataSamples[sc_current_block][modSample][6] = imuc.gyro.z;
      dataSamples[sc_current_block][modSample][7] = imuc.t;
      sc_samples++;
    }

    if (sc_samples >= SAMPLES_TX) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }
}

/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/** CORE 1 */
/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop()
{
  if (sc_samples >= SAMPLES_TX)
  {
    uint32_t samples_captured = sc_samples;
    uint32_t block_to_read = sc_current_block;
    sc_current_block = (sc_current_block + 1) % SAMPLES_CACHE_BLOCKS;
    sc_samples = 0;
    outputStr = "";
    DynamicJsonDocument json(30000);
    json["timeStamp"] = timeClient.getEpochMillis();
    json["event"] = "log";
    JsonObject device = json.createNestedObject("device");
    device["name"] = DEV_NAME;
    device["uuid"] = DEV_UUID;
    device["type"] = "sensor";
    device["dataTypes"] = "accelerometer, gyroscope";
    JsonArray data = json.createNestedArray("data");
    for (int samp = 0; samp < samples_captured; samp++)
    {
      JsonObject jsample = data.createNestedObject();
      jsample["timeStamp"] = dataSamples[block_to_read][samp][0];
      jsample["temperature"] = dataSamples[block_to_read][samp][7];
      JsonArray acc = jsample.createNestedArray("accelerometer");
      acc.add(dataSamples[block_to_read][samp][1]);
      acc.add(dataSamples[block_to_read][samp][2]);
      acc.add(dataSamples[block_to_read][samp][3]);
      JsonArray gyro = jsample.createNestedArray("gyroscope");
      gyro.add(dataSamples[block_to_read][samp][4]);
      gyro.add(dataSamples[block_to_read][samp][5]);
      gyro.add(dataSamples[block_to_read][samp][6]);
    }
    serializeJson(json, outputStr);
    publishMessage(MQTT_TOPIC, outputStr, true);
  }
  if (!client.connected()) reconnect(); // check if client is connected
  timeClient.update(); //Update time from NTP server
}