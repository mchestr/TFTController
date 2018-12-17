#include <FS.h>
#include <XPT2046_Touchscreen.h>
#include <algorithm>
#include <functional>

#ifdef DEBUG
#define logf(...) Serial.printf(__VA_ARGS__)
#else
#define logf(...)
#endif

class TFTCallback {
 public:
  TFTCallback(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax,
              std::function<void(int16_t, int16_t)> callback, uint8_t priority);
  bool checkAndRun(int16_t x, int16_t y);
  void disable() { this->enabled = false; }
  void enable() { this->enabled = true; }
  void setEnabled(bool enabled) { this->enabled = enabled; }
  uint8_t getPriority() { return this->priority; }
  void setPriority(uint8_t priority) { this->priority = priority; }

  bool operator<(const TFTCallback &other) const {
    return priority < other.priority;
  }

 private:
  int16_t xMin;
  int16_t xMax;
  int16_t yMin;
  int16_t yMax;
  std::function<void(int16_t, int16_t)> callback;
  uint8_t priority = 0;

  bool enabled = false;
};

class TFTController {
 public:
  TFTController(XPT2046_Touchscreen *touchScreen);
  TFTController(XPT2046_Touchscreen *touchScreen, String calibrationFile);
  static std::vector<TFTCallback *> callbacks;

  // Calibration
  bool calibrate(std::function<void(int16_t x, int16_t y)> callback);
  bool loadCalibration();

  void loop();
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

  std::function<void()> calibrationStartCallback = nullptr;

  bool isCalibrated();
  bool saveCalibration();
  void checkCallbacks(int16_t x, int16_t y);
};
