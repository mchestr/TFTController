#include "TFTWizard.h"

TFTWizard::TFTWizard(MiniGrafx* gfx, const char* titleFont,
                     const char* keyboardFont, const char* regularFont)
    : gfx(gfx),
      titleFont(titleFont),
      keyboardFont(keyboardFont),
      regularFont(regularFont) {
  keyboard = new TFTKeyboard(gfx, titleFont, keyboardFont);
  keyboard->setSubmitCallback(
      std::bind(&TFTWizard::passwordCallback, this, std::placeholders::_1));
  wifiSelector = new TFTWiFiSelector(gfx, titleFont, regularFont);
  wifiSelector->setSubmitCallback(
      std::bind(&TFTWizard::wifiCallback, this, std::placeholders::_1));
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
  gfx->fillBuffer(0);
  switch (state) {
    case 0:
      wifiSelector->draw(F("Select WiFi Network"));
      break;
    case 1:
      if (!wifiStepInitialized) {
        keyboard->setDefaultValue(password);
        wifiStepInitialized = true;
      }
      keyboard->draw("'"+ ssid + "' WiFi Password", true);
      break;
    default:
      if (additionalSteps > state - 2) {
        if (!initializeStepsCalled[state-2]) {
          initializeSteps[state-2](keyboard);
          initializeStepsCalled[state-2] = true;
        }
        renderSteps[state-2](keyboard);
      } else {
        callback(ssid, password);
      }
  }
  gfx->commit();
}

void TFTWizard::submitWrapper(String value) {
  callbackSteps[state-2](value);
  state++;
}

void TFTWizard::touchCallback(int16_t x, int16_t y) {
  if (!_inProgress) return;
  switch (state) {
    case 0:
      wifiSelector->touchCallback(x, y);
      break;
    case 1:
      keyboard->touchCallback(x, y);
      break;
    default:
      if (additionalSteps > state - 2) {
        keyboard->setSubmitCallback(std::bind(&TFTWizard::submitWrapper, this, std::placeholders::_1));
        keyboard->touchCallback(x, y);
      }
  }
}

void TFTWizard::addStep(std::function<void(TFTKeyboard*)> initialize, std::function<void(TFTKeyboard*)> render, std::function<void(String)> callback) {
  renderSteps[additionalSteps] = render;
  callbackSteps[additionalSteps] = callback;
  initializeSteps[additionalSteps] = initialize;
  initializeStepsCalled[additionalSteps] = false;
  additionalSteps++;
}

void TFTWizard::setDefaultWiFiPassword(String password) {
  this->password = password;
}
