// M5Stack UGreen KVM Changer
void showSettings() {
  //displays the current network connection
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.println("SSID: " + String(networkSSID));
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.println();
  M5.Lcd.println("Battery:");
  int batteryLevel = floor(100.0 * (((M5.Axp.GetVbatData() * 1.1 / 1000) - 3.0) / (4.07 - 3.0)));
  batteryLevel = batteryLevel > 100 ? 100 : batteryLevel;
  M5.Lcd.println(String(batteryLevel) + "%");
}

void updateBrightness() {
  if(currentBrightness >= 12) {
    currentBrightness = 7;
  }
  else {
    currentBrightness++;
  }
  M5.Axp.ScreenBreath(currentBrightness);
}
