
#define TIME_TO_SLEEP (long)(1000000 * 60 * 59.5) /* Time ESP32 will go to sleep (in seconds) Conversion factor for micro seconds to seconds 1000000 => 60 Minutes */

#include <Wire.h>
#include <WiFi.h>

#include "HTTP.h"
#include "Camera.h"

// TODO
// const char *ssid = "Wengert Station";
// const char *password = "(L3mb3rg3rLand!)";
const char *ssid = "Oban";
const char *password = "schnulli01schneufel04";

bool setPowerBoostKeepOn(int en)
{

  Wire.beginTransmission(0x75);
  Wire.write(0x00);
  Wire.write((en) ? 0x37 : 0x35); // Set bit1: 1 enable 0 disable boost keep on

  return Wire.endTransmission() == 0;
}

void connectWIFI()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  Serial.println(String("IP5306 KeepOn ") + (setPowerBoostKeepOn(1) ? "OK" : "FAIL!"));

  connectWIFI();

  if (get("get-time.php") == "1")
  {
    cameraUpdate();
  }

  esp_deep_sleep(TIME_TO_SLEEP);
}

void loop()
{
}
