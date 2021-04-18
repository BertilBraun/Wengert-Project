#pragma once

#define CameraPowerPin 2

#include <SPI.h>

#include <WiFiClient.h>

#include "HTTP.h"
#include "ArduCAM/ArduCAM.h"

const int CS = 15;

ArduCAM myCAM(OV5642, CS);

void initCam()
{
  pinMode(CameraPowerPin, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  digitalWrite(CameraPowerPin, HIGH);

  SPI.begin();

  //Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  while (1)
  {
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    uint8_t temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
      Serial.println("SPI interface FAIL!");
      delay(1000);
    }
    else
      break;
  }
  Serial.println("SPI interface OK");

  while (1)
  {
    uint8_t vid, pid;
    myCAM.wrSensorReg16_8(0xff, 0x01);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
    if ((vid != 0x56) || (pid != 0x42))
    {
      Serial.println("Can't find OV5642 module!");
      delay(1000);
    }
    else
      break;
  }
  Serial.println("OV5642 detected.");

  myCAM.set_format(JPEG);
  myCAM.InitCAM();

  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM.OV5642_set_JPEG_size(OV5642_1024x768);
  delay(1000);
  myCAM.clear_fifo_flag();
  myCAM.write_reg(ARDUCHIP_FRAMES, 0x00);
}

uint32_t startCapture()
{

  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  myCAM.start_capture();

  Serial.print("CAM Capturing");

  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    ;

  Serial.println(" Done");

  uint32_t len = myCAM.read_fifo_length();

  if (len >= MAX_FIFO_SIZE || len == 0)
  {
    Serial.println("Invalid Image size: " + len);
    return -1;
  }

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  return len;
}

void cameraUpdate()
{
  initCam();

  uint32_t len = startCapture();
  if (len == -1)
  {
    Serial.println("Failed to Capture Camera!");
    return;
  }

  Serial.print("Starting Data Stream: " + String(len));

  WiFiClient client;
  if (!client.connect("http://weather-station.meinwengert.de", 80))
  {
    Serial.println("connection failed");
    return;
  }

  String response = "POST /upload-image.php HTTP/1.1\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-Length: " + String(len) + "\r\n";

  Serial.println("conected to the server");
  client.println(response);

  static const size_t bufferSize = 4096;
  static uint8_t buffer[bufferSize] = {0xFF};

  while (len)
  {
    size_t will_copy = (len < bufferSize) ? len : bufferSize;
    SPI.transferBytes(&buffer[0], &buffer[0], will_copy);
    if (!client.connected())
      break;
    client.write(&buffer[0], will_copy);
    len -= will_copy;
  }

  myCAM.CS_HIGH();

  Serial.println("\nUpload done!");
}
