#pragma once

#define CameraPowerPin 2

#include <SPI.h>

#include "HTTP.h"
#include "UDHttp.h"
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

void resposeCB(uint8_t *buffer, int len)
{
  Serial.printf("%s\n", buffer);
}

unsigned int data_len_to_read = 0;

void dataCB(Client *client)
{
  unsigned char temp = 0, temp_last = 0;
  bool is_header = false;

  while (data_len_to_read--)
  {
    temp_last = temp;
    temp = SPI.transfer(0x00);

    if ((temp == 0xD9) && (temp_last == 0xFF))
    {
      client->write(temp);
      myCAM.CS_HIGH();
      is_header = false;
    }
    if (is_header == true)
    {
      client->write(temp);
    }
    else if ((temp == 0xD8) && (temp_last == 0xFF))
    {
      is_header = true;
      client->write(temp_last);
      client->write(temp);
    }
  }
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

  data_len_to_read = len;
  upload("http://weather-station.meinwengert.de/upload-image.php", "image.jpg", len, dataCB, resposeCB);

  Serial.println("\nUpload done!");
}
