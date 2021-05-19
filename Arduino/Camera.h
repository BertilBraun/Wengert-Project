#pragma once

#define CameraPowerPin 2

#include <SPI.h>
#include <Wire.h>
// #include <ArduinoJson.h>

#include "ArduCAM/ArduCAM.h"
#include "Http.h"
#include "UDHttp.h"

const int CS = 15;

uint32_t len;
ArduCAM myCAM(OV5642, CS);

void _camInit() {
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
    myCAM.OV5642_set_JPEG_size(OV5640_2048x1536);
    delay(1000);
    myCAM.clear_fifo_flag();
    myCAM.write_reg(ARDUCHIP_FRAMES, 0x00);
}

void initCam() {
    // Init Camera
    _camInit();

    // String json = get("ESP32CAM_config.json");
    // DynamicJsonDocument doc(4096);
    // DeserializationError error = deserializeJson(doc, json.c_str());

    // if (error) {
    //     Serial.printf("deserializeJson() failed: %s \n", error.c_str());
    //     return;
    // }

    // sensor_t *s = esp_camera_sensor_get();

    // s->set_pixformat(s, PIXFORMAT_JPEG);
    // s->set_framesize(s, (framesize_t)doc["framesize"]);
    // s->set_brightness(s, doc["brightness"]);
    // s->set_contrast(s, doc["contrast"]);
    // s->set_quality(s, doc["quality"]);
    // s->set_sharpness(s, doc["sharpness"]);
    // s->set_saturation(s, doc["saturation"]);
    // s->set_special_effect(s, doc["special_effect"]);
    // s->set_whitebal(s, doc["awb"]);
    // s->set_awb_gain(s, doc["awb_gain"]);
    // s->set_wb_mode(s, doc["wb_mode"]);
    // s->set_exposure_ctrl(s, doc["aec"]);
    // s->set_aec2(s, doc["aec2"]);
    // s->set_ae_level(s, doc["ae_level"]);
    // s->set_aec_value(s, doc["aec_value"]);
    // s->set_gain_ctrl(s, doc["agc"]);
    // s->set_agc_gain(s, doc["agc_gain"]);
    // s->set_gainceiling(s, (gainceiling_t)doc["gainceiling"]);
    // s->set_bpc(s, doc["bpc"]);
    // s->set_wpc(s, doc["wpc"]);
    // s->set_raw_gma(s, doc["raw_gma"]);
    // s->set_lenc(s, doc["lenc"]);
    // s->set_hmirror(s, doc["hmirror"]);
    // s->set_vflip(s, doc["vflip"]);
    // s->set_dcw(s, doc["dcw"]);
    // s->set_colorbar(s, doc["colorbar"]);
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
           [](Client *client) {
               Serial.print("Data Streaming...");

               while (len--) {
                   char data = SPI.transfer(0x00);
                   sendChunk(client, &data, 1);
               }
           });

    myCAM.CS_HIGH();
    Serial.println("\nUpload done! Took: " + String(millis() - start) + "ms");

    SPI.end();

    digitalWrite(CameraPowerPin, LOW);
}
