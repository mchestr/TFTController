#include "TFTWiFiSelector.hpp"

TFTWiFiSelector::TFTWiFiSelector(MiniGrafx* gfx, const char* titleFont,
                                 const char* selectorFont)
    : gfx(gfx), titleFont(titleFont), selectorFont(selectorFont) {
  scanNetworks();
}

TFTWiFiSelector::~TFTWiFiSelector() { wifiScanner.detach(); }

void TFTWiFiSelector::scanNetworks() {
  scanningNetworks = true;
  WiFi.scanNetworksAsync(std::bind(&TFTWiFiSelector::scanNetworksCallback, this,
                                   std::placeholders::_1));
}

void TFTWiFiSelector::scanNetworksCallback(int result) {
  switch (result) {
    case WIFI_SCAN_RUNNING:
      return;
    case WIFI_SCAN_FAILED:
      logf("WiFi scan failed");
      scanningNetworks = false;
      return;
    case 0:
      scanningNetworks = false;
      return;
    default:
      scanningNetworks = false;
      scannedNetworks = result;
      lastNetworkScan = millis();
      selectedNetwork = -1;
      logf("Scan complete, got %d\n", result);
  }
}

void TFTWiFiSelector::draw(String title) {
  if (!scanningNetworks &&
      (lastNetworkScan == 0 || millis() - lastNetworkScan > 60000)) {
    scanNetworks();
  }
  drawTitle(title);
  if (!scanningNetworks) {
    drawNetworks();
    drawNext();
  }
}

void TFTWiFiSelector::touchCallback(int16_t x, int16_t y) {
  if (y > 50 && y < screenHeight - 30) {
    selectedNetwork = (y - 60) / 20;
  } else if (selectedNetwork >= 0 && y > screenHeight - 30) {
    drawNext(true);
    submitCallback(WiFi.SSID(selectedNetwork));
  }
}

void TFTWiFiSelector::drawTitle(String title) {
  gfx->setColor(scanningNetworks ? 2 : 1);
  gfx->setFont(titleFont);
  gfx->setTextAlignment(TEXT_ALIGN_CENTER);
  gfx->drawString(screenWidth / 2, 20,
                  scanningNetworks ? "Scanning..." : title);
}

void TFTWiFiSelector::drawNetworks() {
  gfx->setColor(1);
  gfx->drawRect(10, 60, screenWidth - 20, screenHeight - 105);
  gfx->setTextAlignment(TEXT_ALIGN_LEFT);
  gfx->setFont(selectorFont);
  for (uint8_t i = 0; i < scannedNetworks; i++) {
    if (i > 10) {
      break;
    }
    bool selected = i == selectedNetwork;
    gfx->setColor(1);
    gfx->drawRect(10, 60 + i * 20, screenWidth - 20, 20);
    if (selected) gfx->fillRect(10, 60 + i * 20, screenWidth - 20, 20);
    gfx->setColor(selected ? 3 : 2);
    gfx->drawString(20, 60 + i * 20,
                    String(i + 1) + ". " + WiFi.SSID(i) + " (" +
                        String(rssiToPercentage(WiFi.RSSI(i))) + "%)");
  }
}

void TFTWiFiSelector::drawNext(bool inverted) {
  if (selectedNetwork >= 0) {
    gfx->setFont(titleFont);
    gfx->setColor(inverted ? 3 : 1);
    gfx->fillRect(10, screenHeight - 30, screenWidth - 20, 25);
    gfx->setColor(inverted ? 1 : 3);
    gfx->setTextAlignment(TEXT_ALIGN_CENTER);
    gfx->drawString(screenWidth / 2, screenHeight - 25, F("NEXT"));
  }
}

uint8_t TFTWiFiSelector::rssiToPercentage(int32_t rssi) {
  uint8_t quality;
  if (rssi <= -100) {
    quality = 0;
  } else if (rssi >= -50) {
    quality = 100;
  } else {
    quality = 2 * (rssi + 100);
  }
  return quality;
}
