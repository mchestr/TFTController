#include "main.hpp"


void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  ArduinoOTA.onStart([]() { Serial.println("Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ts.begin();
  gfx.init();
  gfx.fillBuffer(MINI_BLACK);
  gfx.commit();
  touchController.setTouchCallback(std::bind(&TFTKeyboard::touchCallback,
                                             &keyboard, std::placeholders::_1,
                                             std::placeholders::_2));
  keyboard.setSubmitCallback(submitCallback);

  if (!touchController.loadCalibration()) {
    touchController.calibrate(calibrationCallback);
  }
}

void loop() {
  ArduinoOTA.handle();
  gfx.fillBuffer(MINI_BLACK);
  keyboard.draw("WiFi Password?", true);
  touchController.loop();
  gfx.commit();
}

void calibrationCallback(int16_t x, int16_t y) {
  gfx.fillBuffer(MINI_BLACK);
  gfx.setColor(MINI_YELLOW);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.drawString(120, 160,
                 "Please calibrate\ntouch screen by\ntouching the point");
  gfx.setColor(MINI_WHITE);
  gfx.fillCircle(x, y, 10);
  gfx.commit();
}

void submitCallback(String value) {
  Serial.println("Got value " + value);
}