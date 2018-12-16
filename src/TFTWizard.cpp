#include "TFTWizard.h"

TFTWizard::TFTWizard(MiniGrafx* gfx,
                     const char* titleFont, const char* keyboardFont,
                     const char* regularFont)
    : gfx(gfx),
      titleFont(titleFont),
      keyboardFont(keyboardFont),
      regularFont(regularFont) {
  keyboard = new TFTKeyboard(gfx, titleFont, keyboardFont);
  keyboard->setSubmitCallback(std::bind(&TFTWizard::passwordCallback, this, std::placeholders::_1));
  wifiSelector = new TFTWiFiSelector(gfx, titleFont, regularFont);
  wifiSelector->setSubmitCallback(std::bind(&TFTWizard::wifiCallback, this, std::placeholders::_1));
}

TFTWizard::~TFTWizard() {
  delete keyboard;
  keyboard = nullptr;
  delete wifiSelector;
  wifiSelector = nullptr;
}

void TFTWizard::wifiCallback(String ssid) {
  state++;
  this->ssid = ssid;
}

void TFTWizard::passwordCallback(String password) {
  state++;
  this->password = password;
}

void TFTWizard::reset() {
  state = 0;
  ssid = -1;
  password = "";
}

void TFTWizard::draw() {
  switch (state) {
    case 0:
      wifiSelector->draw(F("Select WiFi Network"));
      break;
    case 1:
      keyboard->draw(F("Input WiFi Password"));
      break;
    case 2:
      if (callback) callback(ssid, password);
      _inProgress = false;
      break;
  }
}

void TFTWizard::touchCallback(int16_t x, int16_t y) {
  switch (state) {
    case 0:
      wifiSelector->touchCallback(x, y);
      break;
    case 1:
      keyboard->touchCallback(x, y);
      break;
  }
}
