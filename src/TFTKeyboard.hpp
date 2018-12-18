#include <Arduino.h>
#include <MiniGrafx.h>
#include <functional>

#ifdef DEBUG
#define logf(...) Serial.printf(__VA_ARGS__)
#else
#define logf(...)
#endif

class TFTKeyboard {
 public:
  TFTKeyboard(MiniGrafx *gfx, const char *titleFont, const char *keyboardFont);
  ~TFTKeyboard();
  void setSubmitCallback(std::function<void(String value)> callback) {
    this->submitCallback = callback;
  }
  void draw(String title, bool isPassword = false, String value = "");
  void touchCallback(int16_t x, int16_t y);
  String getBuffer() { return this->buffer; }

 private:
  MiniGrafx *gfx;
  const char *titleFont;
  const char *keyboardFont;
  String buffer = "";
  bool isUpper = false;
  uint8_t keysPerRow = 10;
  uint8_t keyWidth = 24;
  uint16_t screenWidth = 240;
  uint16_t screenHeight = 320;
  const char lower[48] = {
      '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', 'q',  'w',
      'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a',  's', 'd',  'f',
      'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v',  'b', 'n',  'm',
      ',', '.', '/', '`', '-', '=', '[', ']', '\\', ';', '\'',
  };
  const char upper[48] = {
      '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 'Q', 'W',
      'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F',
      'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
      '<', '>', '?', '~', '_', '+', '{', '}', '|', ':', '"',
  };

  std::function<void(String value)> submitCallback;

  void drawTitle(String title);
  void drawInput(bool isPassword);
  void drawShift(bool inverted = false);
  void drawSpace(bool inverted = false);
  void drawSubmit(bool inverted = false);
  void drawBackspace(bool inverted = false);
  void drawKey(String key, uint16_t x, uint16_t y, uint16_t width,
               uint16_t height, bool inverted = false);

  inline bool isShiftTouched(int16_t col, int16_t row);
  inline bool isSpaceTouched(int16_t col, int16_t row);
  inline bool isSubmitTouched(int16_t col, int16_t row);
  inline bool isBackspaceTouched(int16_t col, int16_t row);
  inline bool isKeyTouched(int16_t col, int16_t row);

  inline uint16_t columnToX(uint8_t col) { return col * keyWidth; }
  inline uint16_t rowToY(uint8_t row) { return row * keyWidth; }
  inline uint8_t xToColumn(uint16_t x) { return x / keyWidth; };
  inline uint8_t yToRow(uint16_t y) { return y / keyWidth; };
};
