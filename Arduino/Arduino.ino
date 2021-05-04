
#define TIME_TO_SLEEP (long)(1000000 * 60 * 59.5) /* Time ESP32 will go to sleep (in seconds) Conversion factor for micro seconds to seconds 1000000 => 60 Minutes */

#include <WiFi.h>

#include "HTTP.h"
#include "Camera.h"

const char *ssid = "Wengert Station";
const char *password = "(L3mb3rg3rLand!)";

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

  connectWIFI();

  if (get("get-time.php") == "1")
  {
    cameraUpdate();
  }

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
  esp_deep_sleep_start();
}

void loop()
{
}
