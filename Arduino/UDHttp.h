/*
    Tuan Nguyen (http://www.iotsharing.com) or (nha.tuan84@gmail.com)
*/

#pragma once

#include <functional>
#include <WiFi.h>

#include <base64.h>
#include <WiFiClientSecure.h>

#define HEADER_SIZE 500
#define CHUNK_SIZE 100
#define HOST_LEN 200
//callback that will be invoked whenever data is available
typedef void (*DataCb)(Client *client);
typedef void (*ResponseCb)(uint8_t *buffer, int len);
typedef void (*ProgressCb)(int percent);

/*
    Tuan Nguyen (http://www.iotsharing.com) or (nha.tuan84@gmail.com)
*/

#define HEADER "POST %s HTTP/1.1\r\n"       \
               "Host: %s:%d\r\n"            \
               "Connection: keep-alive\r\n" \
               "Accept: */*\r\n"            \
               "Content-Length: %d\r\n"     \
               "Expect: \r\n"               \
               "Content-Type: multipart/form-data; boundary=------------------------%s\r\n\r\n"

#define OPEN "--------------------------%s\r\n"                               \
             "Content-Disposition: form-data; name='data'; filename='%s'\r\n" \
             "Content-Type: application/octet-stream\r\n\r\n"

#define CLOSE "\r\n--------------------------%s--\r\n"

//simple url parser
int simpleUrlParser(char *url, char *host, int &port)
{
    char *tmp = strstr(url, "http://");
    port = 80;
    char cport[6];
    char *tmp2;
    char *tmp3;
    if (tmp != NULL && host != NULL)
    {
        tmp2 = tmp + strlen("http://");
        tmp = strchr(tmp2, '/');
        if (tmp != NULL)
        {
            tmp3 = strchr(tmp2, ':');
            if (tmp3 == NULL)
            {
                int len = tmp - tmp2;
                if (len < HOST_LEN)
                {
                    memcpy(host, tmp2, len);
                    return 0;
                }
                Serial.println("increase HOST_LEN\n");
            }
            else
            {
                int len = tmp3 - tmp2;
                if (len < HOST_LEN)
                {
                    memcpy(host, tmp2, len);
                    memset(cport, 0, 6);
                    memcpy(cport, tmp3 + 1, tmp - (tmp3 + 1));
                    port = atoi(cport);
                    return 0;
                }
                Serial.println("increase HOST_LEN\n");
            }
        }
    }
    return -1;
}

void sendChunk(Client *client, uint8_t *buf, int len)
{
    int idx = 0;
    while (len > 0)
    {
        size_t result = client->write(&buf[idx], min(len, CHUNK_SIZE));
        len -= result;
        idx += result;
    }
}

int upload(char *uploadUrlHandler, char *fileName, int sizeOfFile, DataCb dataCb, ResponseCb responseCb)
{
    char buf[HEADER_SIZE];
    char host[HOST_LEN];
    int port = 80;
    int contentLen;
    WiFiClient client;
    int result;
    int sent = 0;

    if (dataCb == NULL)
    {
        Serial.println("DataCb or ProgressCb is NULL!\n");
        return -1;
    }
    //gen key from file name using b64
    //String key = base64::encode(String(fileName)); //it make app crash -> fix later
    char *key = "aHR0cDovL3d3dy5pb3RzaGFyaW5nLmNvbQ==";
    //very simple url parser

    if ((strlen(OPEN) + strlen(key) + strlen(fileName)) > HEADER_SIZE)
    {
        Serial.println("Increase HEADER_SIZE\n");
        return -1;
    }
    if ((strlen(CLOSE) + strlen(key)) > HEADER_SIZE)
    {
        Serial.println("Increase HEADER_SIZE\n");
        return -1;
    }
    if ((strlen(HEADER) + strlen(host) + strlen(key) + 20) > HEADER_SIZE)
    {
        Serial.println("Increase HEADER_SIZE\n");
        return -1;
    }
    memset(host, 0, HOST_LEN);
    if (simpleUrlParser(uploadUrlHandler, host, port) == -1)
    {
        Serial.println("url is wrong\n");
        return -1;
    }
    //calculate open
    memset(buf, 0, HEADER_SIZE);
    snprintf(buf, HEADER_SIZE, OPEN, key, fileName);
    contentLen = strlen(buf);
    //calculate close
    memset(buf, 0, HEADER_SIZE);
    snprintf(buf, HEADER_SIZE, CLOSE, key);
    // content-length
    contentLen = contentLen + strlen(buf) + sizeOfFile;
    //fill header
    memset(buf, 0, HEADER_SIZE);
    snprintf(buf, HEADER_SIZE, HEADER, uploadUrlHandler, host, port, contentLen, key);

    if (!client.connect(host, port))
    {
        Serial.println("Connection failed\n");
        return -1;
    }
    //send header
    sendChunk(&client, (uint8_t *)buf, strlen(buf));
    memset(buf, 0, HEADER_SIZE);
    //send open
    snprintf(buf, HEADER_SIZE, OPEN, key, fileName);
    sendChunk(&client, (uint8_t *)buf, strlen(buf));


    //send data
    dataCb(&client);


    memset(buf, 0, HEADER_SIZE);
    snprintf(buf, HEADER_SIZE, CLOSE, key);
    sendChunk(&client, (uint8_t *)buf, strlen(buf));
    memset(buf, 0, CHUNK_SIZE);
    //process response
    while (client.available() > 0)
    {
        int result = client.read((uint8_t *)buf, CHUNK_SIZE);
        if (responseCb != NULL && result != -1)
        {
            responseCb((uint8_t *)buf, result);
        }
    }
    return 0;
}
