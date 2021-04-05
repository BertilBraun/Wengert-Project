
String apiKeyValue = "tPmAT5Ab3j7F9";

#define I2C_SDA_BME          32
#define I2C_SCL_BME          33

#define SerialMon Serial

#define SEALEVELPRESSURE_HPA (1013.25)

#include <Wire.h>
#include <ArduinoHttpClient.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void InitSensor() {

  pinMode(A0, INPUT);

  SerialMon.print("BME280 sensor");
  if (!bme.begin(0x76)) {
    SerialMon.print(" FAIL, check wiring!");
    while (!bme.begin(0x76));
  }
  Serial.println(" OK");
}

String getBMEData() {

  InitSensor();

  String ret = "api_key="             + apiKeyValue +
               "&humidity="           + String(bme.readHumidity()) +
               "&ground_humidity="    + String(analogRead(A0) / 10.24F) +
               "&temperature="        + String(bme.readTemperature()) +
               "&pressure="           + String(bme.readPressure() / 100.0F) +
               "&timeToWater="        + String(0);

  return ret;
}

void UploadSensorData(HttpClient& http, const String& httpSensorData) {

  SerialMon.println("Performing HTTP POST Sensor Data request...");
  SerialMon.println(httpSensorData);
  if (!http.post("/post-data.php", "application/x-www-form-urlencoded", httpSensorData))
    SerialMon.println(http.responseBody());
  else {
    http.stop();
    SerialMon.println("Error on request");
    SerialMon.println(http.responseStatusCode());
    SerialMon.println(http.responseBody());
  }
  http.stop();
}
