#pragma once

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

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

#define HEADERS "POST /upload-image.php HTTP/1.1\r\n"      \
                "Host: weather-station.meinwengert.de\r\n" \
                "Connection: keep-alive\r\n"               \
                "Accept: */*\r\n"                          \
                "Content-Length: "

#define HEADERE "\r\n"         \
                "Expect: \r\n" \
                "Content-Type: multipart/form-data; boundary=---image-upload-wengert\r\n\r\n"

#define OPEN "---image-upload-wengert\r\n"                      \
             "Content-Disposition: form-data; name='data'; filename='image.jpg'\r\n" \
             "Content-Type: application/octet-stream\r\n\r\n"

#define CLOSE "\r\n---image-upload-wengert--\r\n"

#define CHUNK_SIZE 100

void sendChunk(Client *client, const String &data)
{
    int idx = 0;
    int len = data.length();

    const uint8_t* str = (const uint8_t*) data.c_str();

    while (len > 0)
    {
            size_t result = client->write(&str[idx], min(len, CHUNK_SIZE));
            len -= result;
            idx += result;
    }
}
