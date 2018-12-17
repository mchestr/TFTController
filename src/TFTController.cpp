#include <TFTController.h>

std::vector<TFTCallback*> __attribute__((init_priority(101))) TFTController::callbacks;

TFTController::TFTController(XPT2046_Touchscreen *touchScreen)
    : touchScreen(touchScreen), calibrationFile("/calibration.txt") {}

TFTController::TFTController(XPT2046_Touchscreen *touchScreen,
                             String calibrationFile)
    : touchScreen(touchScreen), calibrationFile(calibrationFile) {}

void TFTController::loop() {
  // if (!touchCallback) return;

  if (touchScreen->touched() && millis() - lastTouched > debouncedMillis) {

    lastTouched = millis();
    TS_Point p = touchScreen->getPoint();
    int x = (p.y - ax) * dx;
    int y = 320 - (p.x - ay) * dy;
    logf("Touched: (%d,%d)\n", x, y);
    checkCallbacks(x, y);
  }
}

bool TFTController::calibrate(
    std::function<void(int16_t x, int16_t y)> callback) {
  int pointsCalibrated = 0;
  while (pointsCalibrated != 2) {
    if (pointsCalibrated == 0) {
      callback(10, 10);
      if (touchScreen->touched()) {
        p1 = touchScreen->getPoint();
        ;
        logf("First Point: (%d, %d)\n", p1.x, p1.y);
        pointsCalibrated++;
        lastStateChange = millis();
      }

    } else if (pointsCalibrated == 1) {
      callback(230, 310);
      if (touchScreen->touched() && (millis() - lastStateChange > 1000)) {
        p2 = touchScreen->getPoint();
        ;
        logf("Second Point: (%d, %d)\n", p2.x, p2.y);
        pointsCalibrated++;
        lastStateChange = millis();
        dx = 240.0 / abs(p1.y - p2.y);
        dy = 320.0 / abs(p1.x - p2.x);
        ax = p1.y < p2.y ? p1.y : p2.y;
        ay = p1.x < p2.x ? p1.x : p2.x;
      }
    }
    yield();
  }
  logf("Calibrated with (dx:%.2f,dy:%.2f,ax:%d,ay:%d)\n", dx, dy, ax, ay);
  return saveCalibration();
}

bool TFTController::loadCalibration() {
  if (!SPIFFS.begin()) {
    return false;
  }

  File f = SPIFFS.open(calibrationFile, "r");
  if (!f) {
    return false;
  }

  dx = f.readStringUntil('\n').toFloat();
  dy = f.readStringUntil('\n').toFloat();
  ax = f.readStringUntil('\n').toInt();
  ay = f.readStringUntil('\n').toInt();

  f.close();
  logf("Calibrated with (dx:%.2f,dy:%.2f,ax:%d,ay:%d)\n", dx, dy, ax, ay);
  return true;
}

bool TFTController::saveCalibration() {
  if (!SPIFFS.begin()) {
    return false;
  }

  SPIFFS.remove(calibrationFile);
  File f = SPIFFS.open(calibrationFile, "w");
  if (!f) {
    return false;
  }

  f.println(dx);
  f.println(dy);
  f.println(ax);
  f.println(ay);

  f.close();
  return true;
}


void TFTController::checkCallbacks(int16_t x, int16_t y) {
  for (TFTCallback* callback : callbacks) {
    if (callback->checkAndRun(x, y)) {
      return;
    }
  }
}

TFTCallback::TFTCallback(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax,
                         std::function<void(int16_t, int16_t)> callback, uint8_t priority)
    : xMin(xMin),
      xMax(xMax),
      yMin(yMin),
      yMax(yMax),
      callback(callback),
      priority(priority){
  // add callback to all callbacks and sort on priority
  TFTController::callbacks.push_back(this);
  std::sort(TFTController::callbacks.begin(), TFTController::callbacks.end());
}

bool TFTCallback::checkAndRun(int16_t x, int16_t y) {
  if (enabled && x >= xMin && x < xMax && y >= yMin && y < yMax) {
    callback(x, y);
    return true;
  }
  return false;
}
