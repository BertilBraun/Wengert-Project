#define SerialMon Serial
#define SerialAT Serial1

#define TIME_TO_SLEEP 1000000 * 60 * 30        /* Time ESP32 will go to sleep (in seconds) Conversion factor for micro seconds to seconds 1000000 => 30 Minutes */

#define LOGGING  // TODO

#include <Wire.h>

#include "Camera.h"
#include "Sensor.h"
#include "Modem.h"

bool setPowerBoostKeepOn(int en) {

  Wire.beginTransmission(0x75);
  Wire.write(0x00);
  Wire.write((en) ? 0x37 : 0x35); // Set bit1: 1 enable 0 disable boost keep on

  return Wire.endTransmission() == 0;
}

void setup() {

  SerialMon.begin(115200);
  Wire.begin();

  SerialMon.println(String("IP5306 KeepOn ") + (setPowerBoostKeepOn(1) ? "OK" : "FAIL!"));

  { // to call data destructors before deep sleep -> maybe memory overflow?
    String sensorData = getBMEData();
    
    std::vector<byte> imageData = CameraUpdate();
  
    if (ConnectModem()) {
  
      UploadCameraData(imageData);
      UploadSensorData(sensorData);
  
      DisconnectModem();
    }
  }
  
  esp_deep_sleep(TIME_TO_SLEEP);
}

void loop() { }
