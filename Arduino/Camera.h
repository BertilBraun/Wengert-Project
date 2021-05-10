#pragma once

#include "Http.h"
#include "UDHttp.h"
#include "esp_camera.h"

#include <ArduinoJson.h>

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

camera_fb_t *fb = NULL;

void initCam()
{
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x \n", err);
    return;
  }

  String json = get("ESP32CAM_config.json");
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, json.c_str());

  if (error)
  {
    Serial.printf("deserializeJson() failed: %s \n", error.c_str());
    return;
  }

  sensor_t *s = esp_camera_sensor_get();

  s->set_pixformat(s, PIXFORMAT_JPEG);
  s->set_framesize(s, (framesize_t)doc["framesize"]);
  s->set_brightness(s, doc["brightness"]);
  s->set_contrast(s, doc["contrast"]);
  s->set_quality(s, doc["quality"]);
  s->set_sharpness(s, doc["sharpness"]);
  s->set_saturation(s, doc["saturation"]);
  s->set_special_effect(s, doc["special_effect"]);
  s->set_whitebal(s, doc["awb"]);
  s->set_awb_gain(s, doc["awb_gain"]);
  s->set_wb_mode(s, doc["wb_mode"]);
  s->set_exposure_ctrl(s, doc["aec"]);
  s->set_aec2(s, doc["aec2"]);
  s->set_ae_level(s, doc["ae_level"]);
  s->set_aec_value(s, doc["aec_value"]);
  s->set_gain_ctrl(s, doc["agc"]);
  s->set_agc_gain(s, doc["agc_gain"]);
  s->set_gainceiling(s, (gainceiling_t)doc["gainceiling"]);
  s->set_bpc(s, doc["bpc"]);
  s->set_wpc(s, doc["wpc"]);
  s->set_raw_gma(s, doc["raw_gma"]);
  s->set_lenc(s, doc["lenc"]);
  s->set_hmirror(s, doc["hmirror"]);
  s->set_vflip(s, doc["vflip"]);
  s->set_dcw(s, doc["dcw"]);
  s->set_colorbar(s, doc["colorbar"]);
}

void cameraUpdate()
{
  initCam();

  fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }

  Serial.print("Starting Data Stream: " + String(fb->len) + ": ");

  auto start = millis();

  upload("weather-station.meinwengert.de", "/upload-image.php", fb->len, [](Client *client) {
    Serial.print("Data Streaming...");
    sendChunk(client, (char *)fb->buf, fb->len);
  });

  esp_camera_fb_return(fb);
  Serial.println("\nUpload done! Took: " + String(millis() - start) + "ms");
}
