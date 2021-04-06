#pragma once

#define SEALEVELPRESSURE_HPA (1013.25)

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "Modem.h"

Adafruit_BME280 bme;

void InitSensor() {

  pinMode(A0, INPUT);

  Serial.print("BME280 Sensor");
  if (!bme.begin(0x76)) {
    Serial.print(" FAIL, check wiring!");
    while (!bme.begin(0x76));
  }
  Serial.println(" OK");
}

String getBMEData() {

  InitSensor();

  String ret = String("api_key=tPmAT5Ab3j7F9") +
               "&humidity="            + String(bme.readHumidity()) +
               "&ground_humidity="     + String(analogRead(A0) / 10.24F) +
               "&temperature="         + String(bme.readTemperature()) +
               "&pressure="            + String(bme.readPressure() / 100.0F) +
               "&timeToWater="         + String(0);

  return ret;
}

void UploadSensorData(const String& httpSensorData) {

  Serial.println("Performing HTTP POST Sensor Data request...");
  Serial.println(httpSensorData);
  post("/post-data.php", httpSensorData);
}
