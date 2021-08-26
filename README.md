# M5Stack UGreen KVM Changer

Use any M5StickC, M5StickC+, or M5ATOM to control your UGreen HDMI KVM switcher over a network.

# Setup

## Download and Install the IDE
1. Download the Arduino IDE from `https://www.arduino.cc/en/software`
2. Install the Arduino IDE on your computer

## Setup the IDE Environment
1. Open preferences
2. Click on Additional Boards Manager URLs and paste `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json` into it.
3. Open boards manager, search ESP32, and click install
3. Open library manager, search for, and install the following libraries:
    - MultiButton by Martin Poelstra  
    - M5Stack by M5Stack
    - WiFi by Arduino

## Download the Software and Open in IDE
1. Open `https://github.com/JTF4/M5Stack-UGreen-KVM-Changer` and click download ZIP
2. Extract ZIP to your desired location (make you know where it is)
3. Open the `a_UserConfig` folder and double click the `a_UserConfig.ino` file

## Flashing the Software
1. Plug the M5Stack device into your computer
2. Go to `Tools -> Board -> Arduino ESP32` and select either `M5Stack-C`or `M5Stack-ATOM`
3. To to `Tools -> Port` and select your M5Stack device's port
4. Open `a_UserConfig



 