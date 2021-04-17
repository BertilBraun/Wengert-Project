#pragma once

#define CameraPowerPin 2

#include <SPI.h>
#include <vector>
#include <cctype>
#include <iomanip>
#include <sstream>

#include "HTTP.h"
#include "ArduCAM/ArduCAM.h"

const int CS = 15;

ArduCAM myCAM(OV5642, CS);

String url_encode(const std::vector<byte> &value)
{
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (byte c : value)
  {

    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
    {
      escaped << c;
      continue;
    }

    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int(c);
    escaped << std::nouppercase;
  }

  return escaped.str().c_str();
}

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

std::vector<byte> getImageData()
{

  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  myCAM.start_capture();

  Serial.print("CAM Capturing");

  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    ;

  Serial.println(" Done");

  uint32_t len = myCAM.read_fifo_length();
  std::vector<byte> imageData;

  if (len >= MAX_FIFO_SIZE || len == 0)
  {
    Serial.println("Invalid Image size: " + len);
    return imageData;
  }

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  unsigned char temp = 0, temp_last = 0;
  bool is_header = false;

  imageData.reserve(len);
  while (len--)
  {
    temp_last = temp;
    temp = SPI.transfer(0x00);

    if ((temp == 0xD9) && (temp_last == 0xFF))
    {
      imageData.push_back(temp);
      myCAM.CS_HIGH();
      is_header = false;
    }
    if (is_header == true)
      imageData.push_back(temp);
    else if ((temp == 0xD8) && (temp_last == 0xFF))
    {
      is_header = true;
      imageData.push_back(temp_last);
      imageData.push_back(temp);
    }
  }

  return imageData;
}

void uploadCameraData(const std::vector<byte> &imageData)
{

  size_t copySize = 512;

  std::vector<byte> data;
  data.reserve(copySize);

  Serial.print("Sending Image Data ");
  Serial.println(imageData.size());
  Serial.println("Performing HTTP POST Image Data request");

  post("create-image.php", "");

  for (int i = 0; i < imageData.size(); i += copySize)
  {
    data.clear();
    data.insert(data.end(), imageData.begin() + i, imageData.begin() + i + min(imageData.size() - i, copySize));

    if (!post("post-image.php?data=" + url_encode(data), ""))
      break;
  }
}

void cameraUpdate()
{
  initCam();
  std::vector<byte> imageData = getImageData();

  if (imageData.size())
    uploadCameraData(imageData);
}
