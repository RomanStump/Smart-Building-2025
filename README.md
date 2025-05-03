# Smart-Building-2025
Repository for smart building capstone 2025 at York College of Pennsylvania

A majority of this repository is for the lab bench sensor project presented in the smart building capstone.

All code was done in Arduino IDE and uploaded to an ESP32 Devkit V1. 

If not already installed, an ESP32 board add-on will need to be added. To do so, go to "File->Preferences" and enter the following URL into the "Additional Board Manager URLs" box:
httprs://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

After adding the URL, go to "Tools->Board->Boards Manager..." A window will pop up with a search bar at the top. Search "esp32" and install the latest version of "esp32 by Espressif Systems."

They are a couple libraries that may need to be downloaded to run the complete version of the lab bench sensor. To include libraries in the code, go to "Sketch->Include Library." scroll down to contributed libraries and include the following libraries: "ESP_NOW", "WiFi", and "Wire" These libraries shouled all be available and preinstalled on arduino IDE after setting up the ESP32 board manager. One other library that is included is "esp_wifi.h" which can be typed manually. 
