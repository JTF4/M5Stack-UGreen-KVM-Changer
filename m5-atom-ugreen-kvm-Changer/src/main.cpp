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

// HTTP Webpage Setup
WiFiServer server(80);
void WiFiEvent(WiFiEvent_t event);
void printPage(WiFiClient client);
String request = "";

// TCP Server Setup
WiFiServer serverTCP(23);
WiFiClient RemoteClient;
String CheckMessage(String message);
void checkForConnections();
void receiveData();

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
int currentInput = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);

  //Initialize IR
  Serial.begin(115200, SERIAL_8N1);

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

  logger("IP Address:", "info");
  logger(String(WiFi.localIP()), "info");
  logger("\n", "info");
  logger("Mac Address:", "info");
  logger(String(WiFi.macAddress()), "info");

  preferences.begin("M5-KVM", false);
  String new_deviceMode = preferences.getString("DeviceMode");
  new_deviceMode.toCharArray(deviceMode, 20);
  logger("Device Mode:" + new_deviceMode, "info");
  // added to clear out corrupt prefs
  //preferences.clear();
  preferences.end();

  if (String(deviceMode) == "HTTP") {
    server.begin();
    logger("HTTP server started", "info");
  } else if (String(deviceMode) == "TCP") {
    serverTCP.begin();
    logger("TCP server started", "info");
  } else {
    server.begin();
    logger("Mode Discovery Error", "info");
    logger("HTTP server started", "info");
  }
  
  

}

void loop() {

  if (millis() >= 21600000) {
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

  if (btnM5.isClick()) {
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
  }

  if (String(deviceMode) == "HTTP") {
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
  } else if (String(deviceMode) == "TCP") {
    checkForConnections();
    receiveData();
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
  preferences.begin("M5-KVM", false);
  preferences.putString("DeviceMode", str_devMode);
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

// Webpage Information
String deviceIP = String(WiFi.localIP());

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
String html_1 = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'><style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  background-color: #4CAF50; font-size: 120%;}</style><title>TV Control</title></head><body><div id='main'><h2>TV Control</h2>";
String html_2 = "<form id='F1' action='INPUT_1'><input class='button' onclick='" + deviceIP + ";' type='submit' value='Input 1' ></form><br>";
String html_3 = "<form id='F1' action='INPUT_2'><input class='button' type='submit' value='Input 2' ></form><br>";
String html_4 = "<form id='F1' action='INPUT_3'><input class='button' type='submit' value='Input 3' ></form><br>";
String html_5 = "<form id='F1' action='NEXT'><input class='button' type='submit' value='Next Input' ></form><br>";
String html_6 = "</div></body></html>";
String input_1 = "<t>The TV Is currently using Input 1</t>";
String input_2 = "<t>The TV Is currently using Input 2</t>";
String input_3 = "<t>The TV Is currently using Input 3</t>";

void printPage(WiFiClient client) {
  client.print(header);
  client.print(html_1);
  client.print(html_2);
  client.print(html_3);
  client.print(html_4);
  client.print(html_5);
  client.print(html_6);
  if (currentInput == 1) {
    client.print(input_1);
  } else if (currentInput == 2) {
    client.print(input_2);
  } else if (currentInput == 3) {
    client.print(input_3);
  }
  delay(500);
}

void checkForConnections() {
  if (serverTCP.hasClient()) {
    // If we are already connected to another computer, 
    // then reject the new connection. Otherwise accept
    // the connection. 
    if (RemoteClient.connected()) {
      Serial.println("Connection rejected");
      serverTCP.available().stop();
    }
    else {
      Serial.println("Connection accepted");
      RemoteClient = serverTCP.available();
    }
  }
}

void receiveData() {
  while (RemoteClient.connected() && RemoteClient.available()) {
    String message = RemoteClient.readStringUntil('\n');
    message.trim();
    String response = CheckMessage(message);
    Serial.println(response);
    char responseCharArray[response.length() + 1];
    strcpy(responseCharArray, response.c_str());
    RemoteClient.write(responseCharArray);
  }
}

String CheckMessage(String message) {
  String response = "Invalid command.\n";
  
  Serial.println("Checking command: " + message);
  if (message == "input1") {
    logger("Input 1", "info");
    irsend.sendRaw(input1Data, 67, 38); // Send IR Data
    delay(1000);
    irsend.sendRaw(input1Data, 67, 38); // Send IR Data
    response = "Input 1\n";
  } else if (message == "input2") {
    logger("Input 2", "info");
    irsend.sendRaw(input2Data, 67, 38); // Send IR Data
    delay(1000);
    irsend.sendRaw(input2Data, 67, 38); // Send IR Data
    response = "Input 2\n";
  } else if (message == "input3") {
    logger("Input 3", "info");
    irsend.sendRaw(input3Data, 67, 38); // Send IR Data
    delay(1000);
    irsend.sendRaw(input3Data, 67, 38); // Send IR Data
    response = "Input 3\n";
  } else if (message == "next") {
    logger("Next", "info");
    irsend.sendRaw(nextData, 67, 38); // Send IR Data
    delay(1000);
    irsend.sendRaw(nextData, 67, 38); // Send IR Data
    response = "Next Input\n";
  }

  return response;
}

