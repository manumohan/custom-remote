#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <IRsend.h>
#include <IRrecv.h> // Add this for receiving IR signals
#include <IRutils.h> // Helper functions to print received IR codes

// Replace with your network credentials
const char* ssid = "Mohan";
const char* password = "9447712906manu";

// Create an instance of the AsyncWebServer
AsyncWebServer server(80);

// Pin for IR LED and IR receiver
const int IR_LED_PIN = 14;
const int IR_RECV_PIN = 15;

IRsend irsend(IR_LED_PIN);
IRrecv irrecv(IR_RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);

  // Initialize the IR sender
  irsend.begin();

  // Initialize the IR receiver
  irrecv.enableIRIn();  // Start the receiver

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
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
