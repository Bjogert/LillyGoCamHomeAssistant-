/**
 * @file      SimpleCustomText.ino
 * @author    Simple custom text example for LillyCam
 * @license   MIT
 * @copyright Copyright (c) 2023
 * @date      2023-07-06
 *
 * This is a simple example showing how to add custom text to the OLED display
 * by modifying the existing screen.cpp file minimally.
 */

#include <string.h>
#include <stdio.h>
#include "utilities.h"
#include <WiFi.h>
#include <U8g2lib.h>

// OLED display object (from utilities.h)
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2;

// Add these functions to your screen.cpp file or create a new file

// Simple function to display custom text on the OLED
void displaySimpleText(const char* line1, const char* line2 = "", const char* line3 = "", int displayTime = 2000) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Line 1 - Large font
    if (strlen(line1) > 0) {
        u8g2->setFont(u8g2_font_logisoso16_tr);
        u8g2->drawStr(5, 20, line1);
    }
    
    // Line 2 - Medium font
    if (strlen(line2) > 0) {
        u8g2->setFont(u8g2_font_fur11_tf);
        u8g2->drawStr(5, 40, line2);
    }
    
    // Line 3 - Small font
    if (strlen(line3) > 0) {
        u8g2->setFont(u8g2_font_6x10_tf);
        u8g2->drawStr(5, 55, line3);
    }
    
    u8g2->sendBuffer();
    
    if (displayTime > 0) {
        delay(displayTime);
    }
}

// Function to display sensor-like data
void displaySensorReading(const char* sensorName, float value, const char* unit) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Sensor name
    u8g2->setFont(u8g2_font_logisoso16_tr);
    u8g2->drawStr(5, 20, sensorName);
    
    // Value
    u8g2->setFont(u8g2_font_inb19_mr);
    char valueStr[20];
    snprintf(valueStr, sizeof(valueStr), "%.1f", value);
    u8g2->drawStr(5, 45, valueStr);
    
    // Unit
    u8g2->setFont(u8g2_font_fur11_tf);
    u8g2->drawStr(80, 45, unit);
    
    u8g2->sendBuffer();
}

// Function to display time
void displayTime(int hours, int minutes, int seconds) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Time display
    u8g2->setFont(u8g2_font_inb19_mr);
    char timeStr[10];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hours, minutes, seconds);
    
    // Center the time
    int textWidth = u8g2->getStrWidth(timeStr);
    int x = (128 - textWidth) / 2;
    u8g2->drawStr(x, 35, timeStr);
    
    u8g2->sendBuffer();
}

// Function to display WiFi info
void displayWiFiInfo() {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    u8g2->setFont(u8g2_font_logisoso16_tr);
    u8g2->drawStr(5, 20, "WiFi Info");
    
    u8g2->setFont(u8g2_font_6x10_tf);
    
    if (WiFi.status() == WL_CONNECTED) {
        // SSID
        char ssidStr[25];
        snprintf(ssidStr, sizeof(ssidStr), "SSID: %s", WiFi.SSID().c_str());
        u8g2->drawStr(5, 35, ssidStr);
        
        // IP Address
        char ipStr[25];
        snprintf(ipStr, sizeof(ipStr), "IP: %s", WiFi.localIP().toString().c_str());
        u8g2->drawStr(5, 48, ipStr);
        
        // Signal strength
        char rssiStr[20];
        snprintf(rssiStr, sizeof(rssiStr), "RSSI: %d dBm", WiFi.RSSI());
        u8g2->drawStr(5, 61, rssiStr);
    } else {
        u8g2->drawStr(5, 35, "Not Connected");
    }
    
    u8g2->sendBuffer();
}

/*
 * Usage Examples:
 * 
 * Add these function calls anywhere in your code where you want to display text:
 * 
 * displaySimpleText("Hello", "World", "LillyCam", 3000);
 * displaySensorReading("Temp", 24.5, "°C");
 * displayTime(14, 30, 45);
 * displayWiFiInfo();
 * 
 * You can call these functions:
 * - In response to button presses
 * - When sensors are read
 * - On timer intervals
 * - When PIR motion is detected
 * - When voice commands are received
 * 
 * For example, in your main loop or loopPeripherals function:
 * 
 * static unsigned long lastCustomDisplay = 0;
 * if (millis() - lastCustomDisplay > 10000) { // Every 10 seconds
 *     displaySimpleText("Status", "System OK", "Running...", 2000);
 *     lastCustomDisplay = millis();
 * }
 * 
 * Or in response to triggers:
 * 
 * if (pirTriggered) {
 *     displaySimpleText("Motion", "Detected", "", 2000);
 *     pirTriggered = false;
 * }
 * 
 * To integrate into existing code:
 * 1. Add these functions to your screen.cpp file
 * 2. Add function declarations to your screen.h file
 * 3. Call the functions where needed in your main code
 */

void setup() {
    Serial.begin(115200);
    
    // Initialize I2C for the OLED display
    Wire.begin(I2C_SDA, I2C_SCL);
    
    // Initialize the OLED display
    Wire.beginTransmission(0x3C);
    if (Wire.endTransmission() == 0) {
        Serial.println("Started OLED");
        u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R2, U8X8_PIN_NONE);
        u8g2->begin();
        u8g2->clearBuffer();
        u8g2->setFlipMode(0);
        u8g2->setFontMode(1); // Transparent
        u8g2->setDrawColor(1);
        u8g2->setFontDirection(0);
        
        // Show startup message
        displaySimpleText("Simple", "Custom", "Text Demo", 3000);
        
        Serial.println("SimpleCustomText example started");
    } else {
        Serial.println("Failed to initialize OLED display");
        while (1);
    }
    
    // Initialize WiFi (optional)
    // WiFi.begin("your_ssid", "your_password");
}

void loop() {
    static unsigned long lastUpdate = 0;
    static int demoStep = 0;
    
    // Demo different display functions every 5 seconds
    if (millis() - lastUpdate > 5000) {
        switch (demoStep) {
            case 0:
                displaySimpleText("Hello", "World", "LillyCam");
                break;
            case 1:
                displaySensorReading("Temp", 24.5, "°C");
                break;
            case 2:
                displayTime(14, 30, 45);
                break;
            case 3:
                displayWiFiInfo();
                break;
            case 4:
                displaySimpleText("Demo", "Complete", "Restarting...");
                break;
        }
        
        demoStep = (demoStep + 1) % 5;
        lastUpdate = millis();
    }
    
    delay(100);
}
