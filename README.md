# ESP32 Project with Multiple Board Support

This project is set up for both **ESP32CAM** and **ESP32 WROOM 32** boards using PlatformIO. You can easily switch between the two boards by specifying the environment in the `platform.ini` file.

## Requirements

- [PlatformIO IDE](https://platformio.org/install) installed (e.g., in VS Code or through CLI).
- USB cable for uploading code to your ESP32 board.

## PlatformIO Configuration

The `platform.ini` file contains separate configurations (environments) for each board:

### ESP32CAM Environment

```ini
[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
monitor_speed = 115200
lib_deps = 
    me-no-dev/ESP Async WebServer @ ^1.2.3
    me-no-dev/AsyncTCP @ ^1.1.1
    https://github.com/crankyoldgit/IRremoteESP8266.git
    bblanchon/ArduinoJson @ ^6.21.0
    tzapu/WiFiManager@^2.0.17

[env:esp32wroom32]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps = 
    me-no-dev/ESP Async WebServer @ ^1.2.3
    me-no-dev/AsyncTCP @ ^1.1.1
    https://github.com/crankyoldgit/IRremoteESP8266.git
    bblanchon/ArduinoJson @ ^6.21.0
    tzapu/WiFiManager@^2.0.17


Instructions
1. Setup
Make sure PlatformIO is installed. You can use the PlatformIO IDE in Visual Studio Code or the command-line interface (CLI).

If you're using VS Code, simply open this project folder, and PlatformIO will automatically detect the platform.ini file.

2. Build and Upload for ESP32CAM
To build and upload firmware for the ESP32CAM board:

In PlatformIO IDE:

Select the esp32cam environment in the sidebar under Project Tasks.
Click on Upload to build and upload the firmware.
From the command line:
pio run -e esp32cam --target upload

3. Build and Upload for ESP WROOM 32
To build and upload firmware for the ESP WROOM 32 board:

In PlatformIO IDE:

Select the esp32wroom32 environment in the sidebar under Project Tasks.
Click on Upload to build and upload the firmware.
From the command line:
pio run -e esp32wroom32 --target upload

4. Monitoring Serial Output
Once the firmware is uploaded, you can monitor the serial output to debug the device.

In PlatformIO IDE:

Select Monitor under the specific environment (esp32cam or esp32wroom32).
From the command line:

pio device monitor -e esp32cam
or

pio device monitor -e esp32wroom32

5. Libraries Used
This project uses the following libraries:

ESP Async WebServer
AsyncTCP
IRremoteESP8266
ArduinoJson
WiFiManager

6. Automatic SDK Management
PlatformIO automatically downloads and installs the necessary SDKs, libraries, and toolchains for the specified boards. You do not need to manually download or install any board SDKs. Simply configure the platform.ini and let PlatformIO handle the rest.

Troubleshooting
Build errors: Ensure your board is correctly selected in the environment (esp32cam or esp32wroom32).
Library issues: Ensure all libraries are properly listed under lib_deps in the platform.ini file.
Connection problems: Check the USB cable and port selection.
License
This project is licensed under the MIT License.