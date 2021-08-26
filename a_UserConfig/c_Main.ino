// M5Stack UGreen KVM Changer
void setup() {
  Serial.begin(115200);
  while (!Serial);
  

  //Initialize IR
  #if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  #else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
  #endif  // ESP8266

  // Initialize the M5StickC object
  logger("Initializing M5StickC object.", "info-quiet");
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(1);
  logger("IR Server booting.", "info");
  irsend.begin();
  logger("IR Module Initialized", "info");
  
  delay(100);

  connectToNetwork();
  while (!networkConnected) {
    delay(200);
  }

  //boolean conn = WiFi.softAP(networkSSID, networkPass);

  server.begin();
  logger("Server Started", "info");
}

void loop() {
  // Basic Status Checks
  btnM5.update();
  btnAction.update();

  // Screen Switch
  if (btnM5.isClick()) {
    switch (currentScreen) {
      case 0:
        showSettings();
        M5.Axp.ScreenBreath(7);
        currentScreen = 1;
        break;
      case 1:
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Axp.ScreenBreath(0);
        currentScreen = 0;
        break;
    }
  }

  // Brightness Control
  if (btnAction.isClick()) {
    updateBrightness();
  }

  // Check if Client Connected
  WiFiClient client = server.available();
  if (!client) {return;}

  // Read HTTP Request
  request = client.readStringUntil('\r');

  if ( request.indexOf("INPUT_1") > 0 )  { 
      irsend.sendRaw(input1Data, 67, 38); // Send IR Data
      delay(1000);
      irsend.sendRaw(input1Data, 67, 38); // Send IR Data
      logger("Input 1", "debug");
      printPage(client);
      currentInput = 1;     
    } else if  ( request.indexOf("INPUT_2") > 0 ) { 
      irsend.sendRaw(input2Data, 67, 38);
      delay(1000);
      irsend.sendRaw(input2Data, 67, 38);
      logger("Input 2", "debug");
      printPage(client);
      currentInput = 2;   
    } else if  ( request.indexOf("INPUT_3") > 0 ) { 
      irsend.sendRaw(input3Data, 67, 38);
      delay(1000);
      irsend.sendRaw(input3Data, 67, 38);
      logger("Input 3", "debug");
      printPage(client);
      currentInput = 3;   
    } else if  ( request.indexOf("NEXT") > 0 ) { 
      irsend.sendRaw(nextData, 67, 38);
      if (currentInput == 1) {
        currentInput = 2;
        logger("Input 2", "debug");
      } else if (currentInput == 2) {
        currentInput = 3;
        logger("Input 3", "debug");
      } else if (currentInput == 3) {
        currentInput = 1;
        logger("Input 2", "debug");
      }
      printPage(client);
    }
    else if (!client) 
    {
      client.flush();
    } else 
    {
      printPage(client);
    }
  
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      logger("Network connected!", "info");
      logger(String(WiFi.localIP()), "info");
      networkConnected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      logger("Network connection lost!", "info");
      networkConnected = false;
      break;
  }
}

void connectToNetwork() {
  WiFi.setHostname(hostname);
  logger("Connecting to SSID: " + String(networkSSID), "info");

  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);

  WiFi.mode(WIFI_STA); //station
  WiFi.setSleep(false);

  WiFi.begin(networkSSID, networkPass);
}

void logger(String strLog, String strType) {
  if (strType == "info") {
    Serial.println(strLog);
    M5.Lcd.println(strLog);
  }
  else {
    Serial.println(strLog);
  }
}

