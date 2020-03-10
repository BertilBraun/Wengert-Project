
#define CameraPowerPin 2

#include <SPI.h>
#include <vector>
#include <base64.h>
#include <cctype>
#include <iomanip>
#include <sstream>

#include "ArduCAM/ArduCAM.h"

const int CS = 15;

ArduCAM myCAM(OV5642, CS);

String url_encode(const std::vector<byte> &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (byte c : value) {

    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }

    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int(c);
    escaped << std::nouppercase;
  }

  return escaped.str().c_str();
}

void InitCam() {

  pinMode(CameraPowerPin, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  //Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  while (1) {
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    uint8_t temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55) {
      Serial.println("SPI interface FAIL!");
      delay(1000);
    } else
      break;
  }
  Serial.println("SPI interface OK");

  while (1) {
    uint8_t vid, pid;
    myCAM.wrSensorReg16_8(0xff, 0x01);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
    if ((vid != 0x56) || (pid != 0x42)) {
      Serial.println("Can't find OV5642 module!");
      delay(1000);
    } else
      break;
  }
  Serial.println("OV5642 detected.");

  myCAM.set_format(JPEG);
  myCAM.InitCAM();

  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM.OV5642_set_JPEG_size(OV5640_2048x1536);
  delay(1000);
  myCAM.clear_fifo_flag();
  myCAM.write_reg(ARDUCHIP_FRAMES, 0x00);
}

void GetImageData(std::vector<byte>& imageData) {

  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();
  myCAM.start_capture();

  Serial.print("CAM Capturing");

  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));

  Serial.println(" Done");

  uint32_t len = myCAM.read_fifo_length();

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  unsigned char temp = 0, temp_last = 0;
  bool is_header = false;

  imageData.reserve(len);

  while ( len-- )
  {
    temp_last = temp;
    temp = SPI.transfer(0x00);

    if ( (temp == 0xD9) && (temp_last == 0xFF) ) {
      imageData.push_back(temp);
      myCAM.CS_HIGH();
      is_header = false;
    }
    if (is_header == true)
      imageData.push_back(temp);
    else if ((temp == 0xD8) && (temp_last == 0xFF)) {
      is_header = true;
      imageData.push_back(temp_last);
      imageData.push_back(temp);
    }
  }
}

void CameraUpdate(std::vector<byte>& imageData) {

  digitalWrite(CameraPowerPin, HIGH);
  
  SPI.begin();

  InitCam();
  GetImageData(imageData);

  SPI.end();
  
  digitalWrite(CameraPowerPin, LOW);

  if (imageData.size() >= MAX_FIFO_SIZE)
    Serial.println("Oversized Image.");
  else if (imageData.size() == 0 )
    Serial.println("Image Size is 0.");
}

void UploadCameraData(HttpClient& http, const std::vector<byte>& imageData) {

  SerialMon.println("Performing HTTP GET Time request");
  int response = 0;
  if (!http.get("get-Time.php"))
    response = http.responseBody().toInt();
  else
    SerialMon.println("Error on HTTP GET request");
  http.stop();

  SerialMon.print("It is now ");
  SerialMon.print(response);
  SerialMon.println(" O'Clock");

  if (response < 7 || response > 21)
    return;

  size_t copySize = 512;

  std::vector<byte> data;
  data.reserve(copySize);

  SerialMon.print("Sending Image Data ");
  SerialMon.println(imageData.size());
  SerialMon.println("Performing HTTP POST Image Data request");

  if (! http.post("/delete-image.php", "application/x-www-form-urlencoded", ""))
    SerialMon.println(http.responseBody());
  else
    SerialMon.println("Error on delete Image Data request");
  http.stop();

  for (int i = 0; i < imageData.size(); i += copySize) {

    data.clear();
    data.insert(data.end(), imageData.begin() + i, imageData.begin() + i + min(imageData.size() - i, copySize));

    if (! http.post("/post-image.php", "application/x-www-form-urlencoded", "data=" + url_encode(data)))
      SerialMon.println(http.responseBody());
    else
      SerialMon.println("Error on HTTP POST Image Data request");
    http.stop();
  }
}
