#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <ESPmDNS.h>

// fill in according to users wifi
const char* ssid = "Enter ssid";
const char* password = "password";

WebServer server(80);

String message = "";

//define screen measurements
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//using adafruit 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setFont(&FreeSansBold9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(10, 10);
  display.print("Enter a Message");
  display.display();

  Serial.begin(115200);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // Connect to WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected..!");

  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  // Server routes
  server.on("/", handle_OnConnect);

  server.on("/submit", handle_Submit);

  server.onNotFound(handle_NotFound);

  server.begin();

  Serial.println("HTTP server started");

  if(!MDNS.begin("carterg9")){
    Serial.print("Error setting up MDNS Responder");
    while(1){
      delay(1000);
    }
  }
  Serial.print("mDNS responder started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {


  server.send(200, "text/html", createHTML());

//reset display
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.display();
}

void handle_Submit() {

  // Save textbox value
  if (server.hasArg("msg")) {
    message = server.arg("msg");
  }

  Serial.println("Message received:");
  Serial.println(message);

  // Display message on OLED
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 35);
  display.print(message);

  display.display();

  // Reload webpage
  server.send(200, "text/html", createHTML());
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
//create hosted webpage with html
String createHTML() {

  String str = "<!DOCTYPE html><html>";

  str += "<head>";
  str += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";

  str += "<style>";

  str += "body {";
  str += "font-family: Arial;";
  str += "text-align: center;";
  str += "margin-top: 50px;";
  str += "}";

  str += ".title {";
  str += "font-size: 30px;";
  str += "font-weight: bold;";
  str += "margin-bottom: 40px;";
  str += "}";

  str += ".counter {";
  str += "font-size: 80px;";
  str += "color: blue;";
  str += "margin-bottom: 40px;";
  str += "}";

  str += "input[type=text] {";
  str += "padding: 10px;";
  str += "width: 250px;";
  str += "font-size: 18px;";
  str += "}";

  str += "input[type=submit] {";
  str += "padding: 10px 20px;";
  str += "font-size: 18px;";
  str += "margin-top: 10px;";
  str += "}";

  str += "</style>";
  str += "</head>";

  str += "<body>";


  // Textbox form
  str += "<form action=\"/submit\" method=\"GET\">";

  str += "<input type=\"text\" ";
  str += "name=\"msg\" ";
  str += "placeholder=\"Type a message\">";

  str += "<br><br>";

  str += "<input type=\"submit\" value=\"Send\">";

  str += "</form>";

  // Show current message
  str += "<p><b>Current Message:</b> ";
  str += message;
  str += "</p>";

  str += "</body>";
  str += "</html>";

  return str;
}

