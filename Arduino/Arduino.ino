
// Pump pins
#define OPEN_PUMP_PIN        12
#define CLOSE_PUMP_PIN       14
#define UNHOOK_PUMP_PIN      13

#define SerialMon Serial
#define SerialAT Serial1

#define TIME_TO_SLEEP 1000000 * 1200        /* Time ESP32 will go to sleep (in seconds) Conversion factor for micro seconds to seconds 1000000 * 1200 seconds = 20 Min */

#include <Wire.h>
#include <ArduinoHttpClient.h>

#include "Camera.h"
#include "Sensor.h"
#include "Modem.h"

HttpClient http(client, "meinwengert.de", 80);

bool setPowerBoostKeepOn(int en) {

  Wire.beginTransmission(0x75);
  Wire.write(0x00);
  Wire.write((en) ? 0x37 : 0x35); // Set bit1: 1 enable 0 disable boost keep on

  return Wire.endTransmission() == 0;
}

void Pump(int pin) {
  SerialMon.println(String("Changing Pump: ") + ((pin == CLOSE_PUMP_PIN) ? "close" : "open"));
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
  // TODOLATER delay(20000);
  digitalWrite(UNHOOK_PUMP_PIN, HIGH);
  delay(100);
  digitalWrite(UNHOOK_PUMP_PIN, LOW);
}

void setup() {

  SerialMon.begin(115200);
  Wire.begin();

  pinMode(A0, INPUT);

  SerialMon.println(String("IP5306 KeepOn ") + (setPowerBoostKeepOn(1) ? "OK" : "FAIL!"));

  InitModem();
  InitBME();

  Pump(CLOSE_PUMP_PIN);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
}

void loop() {

  std::vector<byte> imageData;
  CameraUpdate(imageData);

  if (ConnectModem()) {
    
    int TTW = UploadSensorData(http);
    UploadCameraData(http, imageData);

    DisconnectModem();

    if (TTW > 0)
      Pump(OPEN_PUMP_PIN);
    else
      Pump(CLOSE_PUMP_PIN);
  }
  esp_deep_sleep_start();
}
