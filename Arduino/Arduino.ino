
// Pump pins
#define OPEN_PUMP_PIN        12
#define CLOSE_PUMP_PIN       14
#define UNHOOK_PUMP_PIN      13

#define SerialMon Serial
#define SerialAT Serial1

#define TIME_TO_SLEEP 1000000 * 1800        /* Time ESP32 will go to sleep (in seconds) Conversion factor for micro seconds to seconds 1000000 * 1800 seconds = 30 Minutes */

#define LOGGING  // TODO

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
  // TODO LATER delay(20000);
  digitalWrite(UNHOOK_PUMP_PIN, HIGH);
  delay(100);
  digitalWrite(UNHOOK_PUMP_PIN, LOW);
}

bool HalfHourPassed(HttpClient& http) {

  SerialMon.print("Getting if half an hour has passed: ");
  SerialMon.print(http.get("/get-Half-Hour.php"));
  // TODO http.get("/get-Half-Hour.php");
  String response = http.responseBody();
  http.stop();

  if (response != "0" && response != "1") {
    SerialMon.print("Error on Get Request: ");
    SerialMon.println(http.responseStatusCode());
    return false;
  }
  else if (response == "1") {
    SerialMon.println("Has");
    return true;
  }
  else {
    SerialMon.println("Not");
    return false;
  }
}

void setup() {

  SerialMon.begin(115200);
  Wire.begin();
  http.connectionKeepAlive();

  SerialMon.println(String("IP5306 KeepOn ") + (setPowerBoostKeepOn(1) ? "OK" : "FAIL!"));

  InitModem();
  InitSensor();

  std::vector<byte> imageData;
  CameraUpdate(imageData);

  if (ConnectModem() && HalfHourPassed(http)) {

    int TTW = UploadSensorData(http);
    UploadCameraData(http, imageData);

    DisconnectModem();

    Pump(TTW > 0 ? OPEN_PUMP_PIN : CLOSE_PUMP_PIN);
  }

  esp_deep_sleep(TIME_TO_SLEEP);
}

void loop() { }
