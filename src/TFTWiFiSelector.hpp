#include <Arduino.h>
#include <MiniGrafx.h>
#include <Ticker.h>
#include <functional>
#include <ESP8266WiFi.h>

#ifdef DEBUG
#define logf(...) Serial.printf(__VA_ARGS__)
#else
#define logf(...)
#endif

class TFTWiFiSelector {
 public:
  TFTWiFiSelector(MiniGrafx *gfx, const char* titleFont, const char* selectorFont);
  ~TFTWiFiSelector();
  void setSubmitCallback(std::function<void(String ssid)> callback) {
    this->submitCallback = callback;
  }
  void draw(String title);
  void touchCallback(int16_t x, int16_t y);

 private:
  MiniGrafx *gfx;
  const char *titleFont;
  const char *selectorFont;
  std::function<void(String value)> submitCallback;

  Ticker wifiScanner;
  uint8_t maxWiFiNetworks = 8;
  uint16_t screenWidth = 240;
  uint16_t screenHeight = 320;
  uint32_t lastNetworkScan = 0;
  uint8_t scannedNetworks = 0;
  int8_t selectedNetwork = -1;
  bool scanningNetworks = false;
  void drawTitle(String title);
  void drawNetworks();
  void scanNetworks();
  void drawNext(bool inverted = false);
  void scanNetworksCallback(int onComplete);
  uint8_t rssiToPercentage(int32_t rssi);
};
