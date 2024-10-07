#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <IRsend.h>
#include <IRrecv.h> // Add this for receiving IR signals
#include <IRutils.h> // Helper functions to print received IR codes
#include <ArduinoJson.h>

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

// Increase the buffer size to handle larger JSON data
DynamicJsonDocument irCodes(4096);  // Increase size as needed

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

  // Read and parse the mappings.json file
  File file = SPIFFS.open("/ir.json", "r");
  if (!file) {
    Serial.println("Failed to open ir.json for reading");
    return;
  }

  DeserializationError error = deserializeJson(irCodes, file);
  if (error) {
    Serial.println("Failed to parse JSON");
    return;
  }

  // Close the file
  file.close();

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Endpoint to trigger IR signals based on device and action
  server.on("/send_ir", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("device") && request->hasParam("action")) {
      String device = request->getParam("device")->value();
      String action = request->getParam("action")->value();

      if (irCodes.containsKey(device) && irCodes[device].containsKey(action)) {
        const char* irCode = irCodes[device][action];
        // Send the IR code
        irsend.sendNEC(strtol(irCode, NULL, 16), 32);
        delay(100); // Short delay before sending the repeat code
        
        // Send NEC repeat code
        irsend.sendNEC(0xFFFFFFFF, 0); // NEC repeat code

        request->send(200, "text/plain", "IR signal sent for " + device + ": " + action);
      } else {
        request->send(404, "text/plain", "IR code not found for " + device + ": " + action);
      }
    } else {
      request->send(400, "text/plain", "Bad Request: missing device or action parameter");
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
