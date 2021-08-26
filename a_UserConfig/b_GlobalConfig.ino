// M5Stack UGreen KVM Changer
#include <WiFi.h>
#include <M5Stack.h>
#include <PinButton.h>
#include <IRsend.h>

// M5Stick Button Config
PinButton btnM5(37);
PinButton btnAction(39);

bool networkConnected = false;
WiFiServer server(80);
void WiFiEvent(WiFiEvent_t event);
void printPage(WiFiClient client);
String request = "";

int currentInput = 0;

int currentScreen = 0;
int currentBrightness = 11;

//IR Setup
IRsend irsend(kIRLed);

//IR Commands
uint16_t input1Data[71] = {9064, 4502,  586, 550,  584, 550,  584, 550,  582, 550,  584, 550,  584, 550,  582, 552,  582, 1658,  584, 1656,  584, 1658,  584, 1658,  584, 1658,  606, 1636,  584, 1658,  584, 1658,  584, 552,  582, 552,  582, 1658,  584, 552,  582, 552,  582, 550,  582, 552,  582, 550,  584, 550,  584, 1658,  584, 552,  582, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 39996,  9062, 2234,  584};  // NEC 1FE40BF
uint16_t input2Data[71] = {9062, 4500,  586, 552,  582, 550,  584, 550,  584, 550,  584, 552,  582, 552,  582, 550,  582, 1660,  582, 1658,  584, 1658,  584, 1658,  582, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 550,  584, 550,  582, 552,  582, 1658,  584, 552,  582, 550,  582, 552,  582, 552,  582, 552,  582, 1660,  582, 1660,  584, 550,  582, 1660,  584, 1658,  584, 1658,  582, 1660,  582, 1658,  584, 39998,  9062, 2232,  584};  // NEC 1FE20DF
uint16_t input3Data[71] = {9066, 4502,  608, 526,  584, 550,  584, 552,  606, 526,  584, 550,  584, 550,  606, 528,  606, 1634,  586, 1658,  608, 1634,  608, 1634,  584, 1658,  584, 1658,  584, 1658,  608, 1634,  584, 550,  584, 550,  606, 1634,  584, 1658,  608, 526,  606, 528,  582, 550,  608, 526,  584, 550,  582, 1658,  584, 550,  584, 550,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  584, 1658,  608, 39978,  9088, 2208,  608};  // NEC 1FE609F
uint16_t nextData[71] = {9066, 4504,  584, 550,  584, 552,  582, 552,  582, 552,  582, 552,  582, 552,  582, 552,  582, 1660,  582, 1660,  582, 1660,  584, 1660,  584, 1660,  582, 1660,  584, 1660,  584, 1660,  584, 552,  582, 552,  584, 552,  582, 552,  584, 1660,  582, 552,  582, 552,  584, 552,  582, 552,  582, 1660,  582, 1660,  582, 1662,  580, 552,  582, 1660,  582, 1660,  582, 1660,  582, 1660,  582, 40010,  9066, 2234,  584};  // NEC 1FE10EF
