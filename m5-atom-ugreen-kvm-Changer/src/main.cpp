#include <Arduino.h>
#include <WiFi.h>
#include <M5Stack.h>
#include <PinButton.h>
#include <IRsend.h>
#include <stdint.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <inttypes.h> /* strtoimax */


// WiFi Setup
bool networkConnected = false;
WiFiManager wm; // global wm instance
bool portalRunning = false;
const char * netName = "M5-KVM";
void connectToNetwork();

// Logger Setup
void logger(String strLog, String strType);

// Device Mode
char deviceMode[20] = "HTTP";

void rebootNow() {
  logger("Rebooting", "Info");
  ESP.restart();
}

// Preferences Setup
Preferences preferences;

// M5 Variables
PinButton btnM5(39);

// IR Variables
uint16_t input1Data[71] = {9064, 4502,  586, 550,  584, 550,  584, 550,  582, 550,  584, 550,  584, 550,  582, 552,  582, 1658,  584, 1656,  584, 1658,  584, 1658,  584, 1658,  606, 1636,  584, 1658,  584, 1658,  584, 552,  582, 552,  582, 1658,  584, 552,  582, 552,  582, 550,  582, 552,  582, 550,  584, 550,  584, 1658,  584, 552,  582, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 39996,  9062, 2234,  584};  // NEC 1FE40BF
uint16_t input2Data[71] = {9062, 4500,  586, 552,  582, 550,  584, 550,  584, 550,  584, 552,  582, 552,  582, 550,  582, 1660,  582, 1658,  584, 1658,  584, 1658,  582, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 550,  584, 550,  582, 552,  582, 1658,  584, 552,  582, 550,  582, 552,  582, 552,  582, 552,  582, 1660,  582, 1660,  584, 550,  582, 1660,  584, 1658,  584, 1658,  582, 1660,  582, 1658,  584, 39998,  9062, 2232,  584};  // NEC 1FE20DF
uint16_t input3Data[71] = {9066, 4502,  608, 526,  584, 550,  584, 552,  606, 526,  584, 550,  584, 550,  606, 528,  606, 1634,  586, 1658,  608, 1634,  608, 1634,  584, 1658,  584, 1658,  584, 1658,  608, 1634,  584, 550,  584, 550,  606, 1634,  584, 1658,  608, 526,  606, 528,  582, 550,  608, 526,  584, 550,  582, 1658,  584, 550,  584, 550,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  608, 39978,  9088, 2208,  608};  // NEC 1FE609F
uint16_t nextData[71] = {9066, 4504,  584, 550,  584, 552,  582, 552,  582, 552,  582, 552,  582, 552,  582, 552,  582, 1660,  582, 1660,  582, 1660,  584, 1660,  584, 1660,  582, 1660,  584, 1660,  584, 1660,  584, 552,  582, 552,  584, 552,  582, 552,  584, 1660,  582, 552,  582, 552,  584, 552,  582, 552,  582, 1660,  582, 1660,  582, 1662,  580, 552,  582, 1660,  582, 1660,  582, 1660,  582, 1660,  582, 40010,  9066, 2234,  584};  // NEC 1FE10EF

//IR Setup
uint16_t kIRLed = 12;
IRsend irsend(kIRLed);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);

  //Initialize IR
  Serial.begin(115200, SERIAL_8N1);

  preferences.begin("M5-KVM", false);

  // added to clear out corrupt prefs
  //preferences.clear();
  logger("Reading preferences", "info-quiet");
  if(preferences.getString("DevMode").length() > 0) {
    String new_deviceMode = preferences.getString("DevMode");
    new_deviceMode.toCharArray(deviceMode, 20);
  }
 
  preferences.end();

  // Initialize the M5StickC object
  logger("Initializing M5StickC object.", "info-quiet");
  M5.begin();
  logger("IR Server booting.", "info");
  irsend.begin();
  logger("IR Module Initialized", "info");
  
  delay(100);

  connectToNetwork(); //starts Wifi connection
  while (!networkConnected) {
    delay(200);
  }

}

void loop() {

  if (millis() >= 86400000) {
    rebootNow();
  }

  if(portalRunning){
    wm.process();
  }

  btnM5.update();

  if (btnM5.isLongClick()) {
    logger("Resetting Device", "Info");
    wm.resetSettings();
    ESP.restart();
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


String getParam(String name) {
  //read parameter from server, for customhmtl input
  String value;
  if (wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}


void saveParamCallback() {
  logger("[CALLBACK] saveParamCallback fired", "info-quiet");
  logger("PARAM tally Arbiter Server = " + getParam("devMode"), "info-quiet");
  String str_devMode = getParam("devMode");

  //str_taHost.toCharArray(tallyarbiter_host, 40);
  //saveEEPROM();
  logger("Saving new settings.", "info-quiet");
  logger(str_devMode, "info-quiet");
  preferences.begin("tally-arbiter", false);
  preferences.putString("devMode", str_devMode);
  preferences.end();

}

void connectToNetwork() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  wm.setConnectTimeout(5000);

  logger("Connecting to SSID: " + String(WiFi.SSID()), "info");

  //reset settings - wipe credentials for testing
  //wm.resetSettings();

  WiFiManagerParameter custom_devMode("devMode", "Device Mode", deviceMode, 20);

  wm.addParameter(&custom_devMode);

  wm.setSaveParamsCallback(saveParamCallback);

  // custom menu via array or vector
  std::vector<const char *> menu = {"wifi","param","info","sep","restart","exit"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");

  wm.setConfigPortalTimeout(120); // auto close configportal after n seconds

  bool res;

  res = wm.autoConnect(netName); // AP name for setup

  if (!res) {
    logger("Failed to connect", "error");
    ESP.restart();
    return;
  } else {
    //if you get here you have connected to the WiFi
    logger("connected...yay :)", "info");
    networkConnected = true;

    //TODO: fix MDNS discovery
    /*
    int nrOfServices = MDNS.queryService("tally-arbiter", "tcp");
    if (nrOfServices == 0) {
      logger("No server found.", "error");
    } else {
      logger("Number of servers found: ", "info");
      Serial.print(nrOfServices);
     
      for (int i = 0; i < nrOfServices; i=i+1) {
 
        Serial.println("---------------");
       
        Serial.print("Hostname: ");
        Serial.println(MDNS.hostname(i));
 
        Serial.print("IP address: ");
        Serial.println(MDNS.IP(i));
 
        Serial.print("Port: ");
        Serial.println(MDNS.port(i));
 
        Serial.println("---------------");
      }
    }
    */
  }
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