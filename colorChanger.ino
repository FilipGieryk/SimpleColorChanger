#include <SPI.h>
#include <RFID.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiS3.h>


const char ssid[] = "MI9";
const char pass[] = "1234567890";


int status = WL_IDLE_STATUS;
WiFiServer server(80);
// RFID

#define SS_PIN 10
#define RST_PIN 9


RFID rfid(SS_PIN, RST_PIN);

String rfidCard;



// Oled

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
const int PIN_RED   = 5;
const int PIN_GREEN = 6;
const int PIN_BLUE  = 7;

void setup(){
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setTextSize(1);
  display.setTextColor(WHITE); 
  display.clearDisplay();
  display.display();
  display.println("Zbliz karte");
  display.display();
  SPI.begin();
  rfid.init();
pinMode(PIN_RED,   OUTPUT);
pinMode(PIN_GREEN, OUTPUT);
pinMode(PIN_BLUE,  OUTPUT);

}


void loop() {
  checkCard();
}

void checkCard(){
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      if (rfidCard == "195 112 21 247") {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("welcome");
        display.display();
        connectToWifi();
        connection();

      } else {
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("wrong card");
        display.display();
      }
    }
    delay(2000);
    rfid.halt();
  }
}
void connectToWifi(){
  while (status != WL_CONNECTED) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Connecting to wifi");
    display.display();
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();
  printWifiStatus();
}

void connection(){
  while(true){
WiFiClient client = server.available();
  if (client) {
  String HTTP_req = "";
    while (client.connected()) {
      if (client.available()) {
        HTTP_req = client.readStringUntil('\n');
        break;
      }
    }
    while (client.connected()) {
      if (client.available()) {
        String HTTP_header = client.readStringUntil('\n');

        if (HTTP_header.equals("\r"))
          break;
      }
    }
    
    if (HTTP_req.indexOf("POST") != -1) {
        String post_data = client.readStringUntil('\r');
        int color_index = post_data.indexOf("color=");
        if (color_index != -1) {
          String hex_color = post_data.substring(color_index + 6, color_index + 13);
          display.clearDisplay();
          display.setCursor(0, 10);
          display.println("Setting LED color to: " + hex_color);
          display.display();
          setLEDColor(hexToR(hex_color), hexToG(hex_color), hexToB(hex_color));
 }
      }
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();                    
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    client.println("</head>");
    client.println("<body>");
    client.println("<form method=\"POST\">");
    client.println("Enter hex color value: <input type=\"text\" name=\"color\" maxlength=\"7\">");
    client.println("<input type=\"submit\" value=\"Set Color\">");
    client.println("</form>");
    client.println("</body>");
    client.println("</html>");
    client.flush();

  display.setCursor(0, 0);
  display.print("connected");
  display.display();
    delay(10);
    client.stop();
  }
  }
}
void printWifiStatus() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("IP Address: ");
  display.setCursor(0, 10);
  display.println(WiFi.localIP());
  display.display();
}
void setLEDColor(int red, int green, int blue) {

  analogWrite(PIN_RED, red);
  analogWrite(PIN_GREEN, green);
  analogWrite(PIN_BLUE, blue);
}

int hexToR(String hexColor) {
  return (int)strtol(hexColor.substring(0, 2).c_str(), NULL, 16);
}

int hexToG(String hexColor) {
  return (int)strtol(hexColor.substring(2, 4).c_str(), NULL, 16);
}

int hexToB(String hexColor) {
  return (int)strtol(hexColor.substring(4, 6).c_str(), NULL, 16);
}
