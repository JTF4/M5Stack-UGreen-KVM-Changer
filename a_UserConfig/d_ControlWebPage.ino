// M5Stack UGreen KVM Changer
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