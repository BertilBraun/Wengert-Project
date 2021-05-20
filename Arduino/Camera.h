#pragma once

#define CameraPowerPin 2
#define OV5642_CAM

#include <SPI.h>
#include <Wire.h>
// #include <ArduinoJson.h>

#include "ArduCAM/ArduCAM.h"
#include "Http.h"
#include "UDHttp.h"

const int CS = 15;

uint32_t len;
ArduCAM myCAM(OV5642, CS);

void initCam() {
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);

    // Reset the CPLD
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
    myCAM.OV5642_set_JPEG_size(OV5642_1280x960);
}

uint32_t camCapture() {
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    myCAM.start_capture();

    Serial.print("CAM Capturing");
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        ;
    Serial.println(" Done");

    uint32_t len = myCAM.read_fifo_length();

    myCAM.CS_LOW();
    myCAM.set_fifo_burst();

    return len;
}

void cameraUpdate() {
    pinMode(CameraPowerPin, OUTPUT);
    digitalWrite(CameraPowerPin, HIGH);

    Wire.begin();
    SPI.begin();
    SPI.setFrequency(4000000);  // 4MHz

    initCam();

    len = camCapture();
    if (len >= MAX_FIFO_SIZE) {
        Serial.println("Oversized Image.");
        return;
    } else if (len == 0) {
        Serial.println("Image Size is 0.");
        return;
    }

    Serial.print("Starting Data Stream: " + String(len) + ": ");

    auto start = millis();

    upload("weather-station.meinwengert.de", "/upload-image.php", len,
           [](Client* client) {
               Serial.print("Data Streaming...");

               while (len--) {
                   char data = SPI.transfer(0x00);
                   sendChunk(client, &data, 1);
               }
           });

    Serial.println("\nUpload done! Took: " + String(millis() - start) + "ms");

    SPI.end();

    digitalWrite(CameraPowerPin, LOW);
}
