#include <Arduino.h>
#include <ArduinoOTA.h>

#include <ILI9341_SPI.h>
#include <MiniGrafx.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

#include <TFTController.h>
#include <TFTKeyboard.hpp>
#include "ArialRounded.h"

#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define BITS_PER_PIXEL 2
#define TFT_DC D4
#define TFT_CS D8
#define TFT_TOUCH_CS D2
#define TFT_TOUCH_IRQ D1
#define TEMP_PIN D3

#ifdef DEBUG
#define logf(...) Serial.printf(__VA_ARGS__)
#else
#define logf(...)
#endif

#define MINI_BLACK 0
#define MINI_WHITE 1
#define MINI_YELLOW 2
#define MINI_BLUE 3
uint16_t palette[] = {ILI9341_BLACK, ILI9341_WHITE, ILI9341_YELLOW, 0x7E3C};

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
XPT2046_Touchscreen ts(TFT_TOUCH_CS, TFT_TOUCH_IRQ);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);
TFTController touchController(&ts);
TFTKeyboard keyboard(&gfx, Roboto_Thin_12, Roboto_Medium_16);

void calibrationCallback(int16_t x, int16_t y);
void touchCallback(int16_t x, int16_t y);
void submitCallback(String value);
