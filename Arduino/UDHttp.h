/*
    Tuan Nguyen (http://www.iotsharing.com) or (nha.tuan84@gmail.com)
*/

#pragma once

#include <WiFi.h>

#define HEADER_SIZE 512
#define CHUNK_SIZE 128
#define HOST_LEN 200

//callback that will be invoked whenever data is available
typedef void (*DataCb)(Client *client, int len);

#define HEADER "POST %s HTTP/1.1\r\n"       \
               "Host: %s\r\n"            \
               "Connection: keep-alive\r\n" \
               "Accept: */*\r\n"            \
               "Content-Length: %d\r\n"     \
               "Expect: \r\n"               \
               "Content-Type: multipart/form-data; boundary=------------------------wengertImageUpload\r\n\r\n"

#define OPEN "--------------------------wengertImageUpload\r\n"                       \
             "Content-Disposition: form-data; name='58B6E6B64C7A088FA18CAB6A84668F9E'; filename='image.jpeg'\r\n" \
             "Content-Type: application/octet-stream\r\n\r\n"

#define CLOSE "\r\n--------------------------wengertImageUpload--\r\n"

void sendChunk(Client *client, char *buf, int len)
{
    int idx = 0;
    while (len > 0)
    {
        size_t result = client->write((uint8_t *) &buf[idx], min(len, CHUNK_SIZE));
        len -= result;
        idx += result;
    }
}

void upload(const String& host, const String& path, int sizeOfFile, DataCb dataCb)
{
    WiFiClient client;
    if (!client.connect(host.c_str(), 80))
    {
        Serial.println("Connection failed");
        return;
    }

    int contentLen = strlen(OPEN) + strlen(CLOSE) + sizeOfFile;
    
    char buf[HEADER_SIZE];
    snprintf(buf, HEADER_SIZE, HEADER, path.c_str(), host.c_str(), contentLen);

    //send header
    sendChunk(&client, buf, strlen(buf));
    sendChunk(&client, OPEN, strlen(OPEN));

    //send data
    dataCb(&client, sizeOfFile);

    sendChunk(&client, CLOSE, strlen(CLOSE));
}
