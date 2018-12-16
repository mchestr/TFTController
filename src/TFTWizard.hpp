#include <MiniGrafx.h>
#include "TFTKeyboard.hpp"
#include "TFTWiFiSelector.hpp"
#include <functional>

class TFTWizard {
 public:
  TFTWizard(MiniGrafx* gfx, const char* titleFont, const char* keyboardFont,
            const char* regularFont);
  ~TFTWizard();
  void draw();
  void start() {this->_inProgress = true;}
  bool inProgress() {return _inProgress;}
  void setCallback(std::function<void(String, String)> callback) {this->callback = callback;};
  void reset();
  void touchCallback(int16_t x, int16_t y);

 private:
  MiniGrafx *gfx;
  const char *titleFont;
  const char *keyboardFont;
  const char *regularFont;
  uint16_t screenWidth = 240;
  uint16_t screenHeight = 320;

  String ssid;
  String password;
  std::function<void(String, String)> callback;

  TFTKeyboard *keyboard;
  TFTWiFiSelector *wifiSelector;
  uint8_t state = 0;
  bool _inProgress = false;
  void drawWiFiSelect();
  void drawWiFiPassword();
  void wifiCallback(String ssid);
  void passwordCallback(String password);
};
