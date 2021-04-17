#pragma once

#include <HTTPClient.h>

String get(const String &path)
{
  static const String serverName = "http://weather-station.meinwengert.de/";

  String serverPath = serverName + path;

  HTTPClient http;
  http.begin(serverPath.c_str());
  http.setTimeout(5000);

  // Send HTTP request
  int httpResponseCode = http.GET();
  String payload = http.getString();

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.println(payload);

  // Free resources
  http.end();

  return httpResponseCode > 0 ? payload : "Error on Request";
}

bool post(const String &path, const String &data)
{
  static const String serverName = "http://weather-station.meinwengert.de/";

  String serverPath = serverName + path;

  HTTPClient http;
  http.begin(serverPath.c_str());
  http.setTimeout(5000);

  // Send HTTP request
  int httpResponseCode = http.POST(data);
  String payload = http.getString();

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.println(payload);

  // Free resources
  http.end();

  return httpResponseCode > 0;
}
