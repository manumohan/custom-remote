#include <Arduino.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <IRsend.h>
#include <IRrecv.h> // Add this for receiving IR signals
#include <IRutils.h> // Helper functions to print received IR codes

// Create an instance of the AsyncWebServer
AsyncWebServer server(80);

// Pin for IR LED and IR receiver
const int IR_LED_PIN = 14;
const int IR_RECV_PIN = 15;
const int RESET_PIN = 0;  // Pin to reset Wi-Fi settings (GPIO 0 for example)

IRsend irsend(IR_LED_PIN);
IRrecv irrecv(IR_RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);

  // Set the reset pin as input with pull-up resistor
  pinMode(RESET_PIN, INPUT_PULLUP);

  // Check if the reset button is pressed
  if (digitalRead(RESET_PIN) == LOW) {
    Serial.println("Reset button pressed, clearing Wi-Fi credentials...");
    WiFiManager wifiManager;
    wifiManager.resetSettings();  // Reset stored credentials
    ESP.restart();  // Restart ESP32
  }

  // Create an instance of WiFiManager
  WiFiManager wifiManager;

  // Try to connect to Wi-Fi or start the configuration portal
  if (!wifiManager.autoConnect("ESP32_AP")) {
    Serial.println("Failed to connect, resetting...");
    ESP.restart();  // Restart ESP32 if it fails to connect
  }

  Serial.println("Connected to Wi-Fi!");

  // Initialize the IR sender
  irsend.begin();

  // Initialize the IR receiver
  irrecv.enableIRIn();  // Start the receiver

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Serve the ir.json file
  server.on("/ir.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/ir.json", "application/json");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/manifest.json", "application/manifest+json");
  });

  // Endpoint to trigger IR signals directly based on IR code from request
  server.on("/send_ir", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("irCode")) {
      String irCode = request->getParam("irCode")->value();
      Serial.println("IR Code: " + irCode);

      // Convert the IR code from string to long (assuming the code is in hexadecimal format)
      unsigned long codeValue = strtol(irCode.c_str(), NULL, 16);

      // Send the IR code
      irsend.sendNEC(codeValue, 32);
      delay(100); // Short delay before sending the repeat code
      
      // Optionally send NEC repeat code
      irsend.sendNEC(0xFFFFFFFF, 0); // NEC repeat code

      request->send(200, "text/plain", "IR signal sent with code: " + irCode);
    } else {
      request->send(400, "text/plain", "Bad Request: missing irCode parameter");
    }
  });

  // Start the server
  server.begin();
}

void loop() {
  // Check if an IR signal is received
  if (irrecv.decode(&results)) {
    // Print the received IR code in HEX
    Serial.println(resultToHexidecimal(&results));
    irrecv.resume();  // Ready to receive the next signal
  }
}
