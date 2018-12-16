#include <FS.h>
#include <XPT2046_Touchscreen.h>
#include <functional>

#ifdef DEBUG
#define logf(...) Serial.printf(__VA_ARGS__)
#else
#define logf(...)
#endif

class TFTController {
 public:
  TFTController(XPT2046_Touchscreen *touchScreen);
  TFTController(XPT2046_Touchscreen *touchScreen, String calibrationFile);

  // Calibration
  bool calibrate(std::function<void(int16_t x, int16_t y)> callback);
  bool loadCalibration();

  void loop();
  inline void setTouchCallback(
      std::function<void(int16_t x, int16_t y)> callback) {
    this->touchCallback = callback;
  };
  inline void setDebouncedMillis(uint16_t debouncedMillis) {
    this->debouncedMillis = debouncedMillis;
  }

 private:
  XPT2046_Touchscreen *touchScreen;
  String calibrationFile;

  float dx = 0.0;
  float dy = 0.0;
  int ax = 0;
  int ay = 0;
  long lastStateChange = 0;
  uint32_t lastTouched = 0;
  TS_Point p1, p2;
  uint16_t debouncedMillis = 500;

  std::function<void(int16_t x, int16_t y)> touchCallback = nullptr;
  std::function<void()> calibrationStartCallback = nullptr;

  bool isCalibrated();
  bool saveCalibration();
};
