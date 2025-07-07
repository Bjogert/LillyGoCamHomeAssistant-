# Custom OLED Text Display for LillyCam

This guide shows you how to add custom text to the OLED screen on your LillyCam device.

## Hardware Information

- **Display**: 128x64 pixel OLED display
- **Controller**: SSD1306
- **Interface**: I2C
- **I2C Address**: 0x3C
- **Pins**: SDA=7, SCL=6

## Quick Start

### Method 1: Simple Text Display (Easiest)

1. Copy the functions from `SimpleCustomText.ino` to your project
2. Add these function calls in your code:

```cpp
// Display simple text
displaySimpleText("Hello", "World", "LillyCam", 3000);

// Display sensor reading
displaySensorReading("Temp", 24.5, "°C");

// Display time
displayTime(14, 30, 45);

// Display WiFi info
displayWiFiInfo();
```

### Method 2: Advanced Custom Display (More Features)

1. Copy `screen_custom.h` and `screen_custom.cpp` to your AllFunction folder
2. Add `#include "screen_custom.h"` to your main file
3. Use the advanced functions:

```cpp
// Custom message with automatic word wrapping
displayCustomMessage("Title", "This is a longer message that will wrap", 3000);

// Sensor data with icons
displaySensorData(24.5, 60.2, 1013.25);

// Network info with signal strength indicator
displayNetworkInfo("MyWiFi", "192.168.1.100", -45);

// System status with progress bars
displaySystemStatus(45.2, 67.8, 85.3);

// Date and time display
displayDateTime(2023, 7, 6, 14, 30, 45);

// Custom logo
displayCustomLogo("My Cam");
```

### Method 3: Standalone Custom Display

1. Copy `CustomOLEDText.ino` to a new folder
2. Update your `platformio.ini` to point to this folder:
   ```ini
   src_dir = examples/CustomOLEDText
   ```
3. Upload and run

## Integration with Existing Code

### Add to AllFunction Example

Add this code to your `AllFunction.ino` file:

```cpp
// Add to global variables
unsigned long lastCustomDisplay = 0;
bool showCustomText = false;

// Add to loopPeripherals function
void loopPeripherals(void *ptr) {
    // ... existing code ...
    
    // Custom display every 10 seconds
    if (millis() - lastCustomDisplay > 10000) {
        displaySimpleText("Status", "System OK", "Running...", 2000);
        lastCustomDisplay = millis();
    }
    
    // ... rest of existing code ...
}
```

### Respond to Events

```cpp
// When PIR motion is detected
void pir_interrupt_event() {
    displaySimpleText("Motion", "Detected", "", 2000);
    // ... existing code ...
}

// When voice is detected
void onVoiceDetected() {
    displaySimpleText("Voice", "Command", "Detected", 2000);
}

// When button is pressed
void onButtonPress() {
    displayWiFiInfo();
    delay(3000); // Show for 3 seconds
}
```

## Available Fonts

The U8g2 library provides many fonts. Here are some useful ones:

- `u8g2_font_logisoso16_tr` - Large title text
- `u8g2_font_inb19_mr` - Large bold text
- `u8g2_font_fur11_tf` - Medium text
- `u8g2_font_6x10_tf` - Small text
- `u8g2_font_8x13_mf` - Fixed width text
- `u8g2_font_micro_tr` - Very small text

## Display Functions Reference

### Simple Functions
- `displaySimpleText(line1, line2, line3, displayTime)` - Display up to 3 lines
- `displaySensorReading(name, value, unit)` - Display sensor data
- `displayTime(hours, minutes, seconds)` - Display time
- `displayWiFiInfo()` - Display WiFi connection info

### Advanced Functions
- `displayCustomMessage(title, message, displayTime)` - Auto-wrapping message
- `displaySensorData(temp, humidity, pressure)` - Multi-sensor display
- `displayNetworkInfo(ssid, ip, rssi)` - Network information
- `displaySystemStatus(cpu, memory, battery)` - System status with progress bars
- `displayDateTime(year, month, day, hour, minute, second)` - Date/time
- `displayCustomLogo(logoText)` - Custom logo display

### Utility Functions
- `drawCenteredText(text, y, font)` - Center text horizontally
- `drawProgressBar(x, y, width, height, progress)` - Progress bar
- `drawBatteryIcon(x, y, level)` - Battery icon
- `drawWiFiIcon(x, y, signal)` - WiFi signal icon

## Tips and Best Practices

1. **Display Time**: Use reasonable display times (2-5 seconds) to avoid blocking
2. **Text Length**: Keep text short - the display is only 128 pixels wide
3. **Font Selection**: Use larger fonts for important info, smaller for details
4. **Refresh Rate**: Don't update too frequently to avoid flickering
5. **Memory**: Each display update uses some memory, so don't store too many buffers

## Example Projects

### Temperature Monitor
```cpp
void displayTemperature() {
    float temp = readTemperatureSensor(); // Your sensor reading
    displaySensorReading("Temp", temp, "°C");
}
```

### Clock Display
```cpp
void displayClock() {
    // Get current time (implement your time keeping)
    int h = getCurrentHour();
    int m = getCurrentMinute();
    int s = getCurrentSecond();
    displayTime(h, m, s);
}
```

### Status Monitor
```cpp
void displayStatus() {
    if (WiFi.status() == WL_CONNECTED) {
        displaySimpleText("Online", WiFi.localIP().toString().c_str(), "Connected");
    } else {
        displaySimpleText("Offline", "No Connection", "");
    }
}
```

## Troubleshooting

1. **Display not working**: Check I2C connections and address (0x3C)
2. **Text not appearing**: Ensure `u8g2->sendBuffer()` is called
3. **Garbled text**: Check font selection and text bounds
4. **System crashes**: Avoid long delays in interrupt handlers

## Screen Specifications

- **Resolution**: 128x64 pixels
- **Colors**: Monochrome (black/white)
- **Viewing Area**: ~25mm x 12mm
- **Character Capacity**: ~16 characters per line (depending on font)
- **Maximum Lines**: 8 lines with small font, 4 lines with medium font

This should give you everything you need to add custom text to your LillyCam's OLED display!
