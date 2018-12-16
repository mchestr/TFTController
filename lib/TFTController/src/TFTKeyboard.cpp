#include "TFTKeyboard.hpp"

TFTKeyboard::TFTKeyboard(MiniGrafx* gfx, const char* keyboardFont, const char* titleFont)
    : gfx(gfx), keyboardFont(keyboardFont), titleFont(titleFont) {}

void TFTKeyboard::draw(String title, bool isPassword) {
  uint16_t col = 0;
  uint16_t row = 6;
  gfx->setFont(keyboardFont);
  uint8_t size = isUpper ? sizeof(upper) : sizeof(lower);
  for (uint8_t i = 0; i < size - 1; i++) {
    if (col >= keysPerRow) {
      col = 0;
      row++;
    }
    drawKey(isUpper ? String(upper[i]) : String(lower[i]), columnToX(col++),
            rowToY(row), keyWidth, keyWidth);
  }
  drawSpace();
  drawShift();
  drawSubmit();
  drawBackspace();
  drawTitle(title);
  drawInput(isPassword);
}

void TFTKeyboard::touchCallback(int16_t x, int16_t y) {
  logf("\nTouched at: (%d, %d)", x, y);

  // screen seems to be inverted, starts at 240 on the left
  uint16_t col = (screenWidth / keyWidth) - (x / keyWidth) - 1;
  uint16_t row = y / keyWidth;
  logf(",col:%d,row:%d\n", col, row);

  if (isKeyTouched(col, row)) {
    uint8_t charPos = (row - 6) * keysPerRow + col;
    String toDisplay =
        isUpper ? String(upper[charPos]) : String(lower[charPos]);
    drawKey(toDisplay, keyWidth * col, row * keyWidth, keyWidth, keyWidth,
            true);
    buffer += toDisplay;
  } else if (isSpaceTouched(col, row)) {
    drawSpace(true);
    buffer += " ";
  } else if (isShiftTouched(col, row)) {
    drawShift(true);
    isUpper = !isUpper;
  } else if (isSubmitTouched(col, row)) {
    drawSubmit(true);
    if (submitCallback) submitCallback(buffer);
    buffer = "";
  } else if (isBackspaceTouched(col, row)) {
    drawBackspace(true);
    if (buffer.length() <= 1) {
      buffer = "";
    } else {
      buffer = buffer.substring(0, buffer.length() - 1);
    }
  }
  gfx->commit();
}

void TFTKeyboard::drawKey(String key, uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height, bool inverted) {
  gfx->setColor(inverted ? 3 : 1);
  gfx->fillRect(x, y, width, height);
  gfx->setColor(0);
  gfx->drawRect(x, y, width, height);
  gfx->setTextAlignment(TEXT_ALIGN_CENTER);
  gfx->drawString(x + width / 2, y, key);
}

void TFTKeyboard::drawSpace(bool inverted) {
  drawKey("SPACE", columnToX(7), rowToY(10), keyWidth * 3, keyWidth, inverted);
}

void TFTKeyboard::drawShift(bool inverted) {
  uint8_t width = keyWidth * 2;
  drawKey("^\nSHIFT", columnToX(0), rowToY(11), width, width, inverted);
}

void TFTKeyboard::drawSubmit(bool inverted) {
  drawKey("SUBMIT", columnToX(3), rowToY(12), keyWidth * 4, keyWidth, inverted);
}

void TFTKeyboard::drawBackspace(bool inverted) {
  uint8_t width = keyWidth * 2;
  drawKey("<--\nBACK", columnToX(8), rowToY(11), width, width, inverted);
}

void TFTKeyboard::drawTitle(String title) {
  gfx->setColor(1);
  gfx->setFont(titleFont);
  gfx->setTextAlignment(TEXT_ALIGN_CENTER);
  gfx->drawString(screenWidth / 2, 20, title);
}

void TFTKeyboard::drawInput(bool isPassword) {
  String toDisplay = "";
  if (isPassword) {
    if (buffer.length() == 0) {
      return;
    }
    for (uint8_t i = 0; i < buffer.length() - 1; i++) {
      toDisplay += "*";
    }
    toDisplay += buffer.charAt(buffer.length() - 1);
  } else {
    toDisplay = buffer;
  }
  gfx->setColor(3);
  gfx->setTextAlignment(TEXT_ALIGN_CENTER);
  gfx->drawString(screenWidth / 2, 40, toDisplay);
}

inline bool TFTKeyboard::isShiftTouched(int16_t col, int16_t row) {
  return (row == 11 || row == 12) && (col == 0 || col == 1);
}

inline bool TFTKeyboard::isSpaceTouched(int16_t col, int16_t row) {
  return row == 10 && col >= 7;
}

inline bool TFTKeyboard::isSubmitTouched(int16_t col, int16_t row) {
  return row == 12 && (col >= 3 && col <= 6);
}

inline bool TFTKeyboard::isBackspaceTouched(int16_t col, int16_t row) {
  return (row == 11 || row == 12) && (col == 9 || col == 10);
}

inline bool TFTKeyboard::isKeyTouched(int16_t col, int16_t row) {
  return (row >= 6 && row < 10) || (row == 10 && col < 7);
}
