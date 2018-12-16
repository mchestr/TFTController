#include <MiniGrafx.h>
#include <functional>
#include "TFTKeyboard.hpp"
#include "TFTWiFiSelector.hpp"

class TFTWizard {
 public:
  TFTWizard(MiniGrafx *gfx, const char *titleFont, const char *keyboardFont,
            const char *regularFont);
  ~TFTWizard();
  void draw();
  void start() { this->_inProgress = true; }
  bool inProgress() { return _inProgress; }
  void setCallback(std::function<void(String, String)> callback) {
    this->callback = callback;
  };
  void reset();
  void touchCallback(int16_t x, int16_t y);
  void addStep(std::function<void(TFTKeyboard*)> render, std::function<void(String)> callback);

 private:
  MiniGrafx *gfx;
  const char *titleFont;
  const char *keyboardFont;
  const char *regularFont;
  uint16_t screenWidth = 240;
  uint16_t screenHeight = 320;

  String ssid;
  String password;
  uint8_t additionalSteps = 0;
  std::function<void(String, String)> callback;
  std::function<void(String)> callbackSteps[8];
  std::function<void(TFTKeyboard*)> renderSteps[8];

  TFTKeyboard *keyboard;
  TFTWiFiSelector *wifiSelector;
  uint8_t state = 0;
  bool _inProgress = false;
  void drawWiFiSelect();
  void drawWiFiPassword();
  void wifiCallback(String ssid);
  void passwordCallback(String password);
  void submitWrapper(String value);
};
