/**
 * @file      screen_custom.cpp
 * @author    Modified for custom text display
 * @license   MIT
 * @copyright Copyright (c) 2023
 * @date      2023-07-06
 *
 * Enhanced screen functionality with custom text display capabilities
 */

#include "screen_custom.h"
#include "WiFi.h"
#include "network.h"
#include "esp_camera.h"
#include "utilities.h"

// Display a custom message with title
void displayCustomMessage(const char* title, const char* message, int displayTime) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Draw title
    u8g2->setFont(u8g2_font_logisoso16_tr);
    drawCenteredText(title, 20, u8g2_font_logisoso16_tr);
    
    // Draw separator line
    u8g2->drawHLine(10, 25, 108);
    
    // Draw message
    u8g2->setFont(u8g2_font_fur11_tf);
    
    // Handle multi-line message
    const char* line1 = message;
    char line2[64] = "";
    char line3[64] = "";
    
    // Simple word wrapping - split at first space after 16 characters
    int len = strlen(message);
    if (len > 16) {
        int splitPos = 16;
        while (splitPos < len && message[splitPos] != ' ') {
            splitPos++;
        }
        if (splitPos < len) {
            strncpy(line2, message + splitPos + 1, 15);
            line2[15] = '\0';
            
            // Check if we need a third line
            if (strlen(message + splitPos + 1) > 15) {
                int secondSplit = splitPos + 17;
                while (secondSplit < len && message[secondSplit] != ' ') {
                    secondSplit++;
                }
                if (secondSplit < len) {
                    strncpy(line3, message + secondSplit + 1, 15);
                    line3[15] = '\0';
                }
            }
        }
    }
    
    // Draw the text lines
    char tempLine1[17];
    strncpy(tempLine1, line1, 16);
    tempLine1[16] = '\0';
    
    u8g2->drawStr(5, 40, tempLine1);
    if (strlen(line2) > 0) {
        u8g2->drawStr(5, 52, line2);
    }
    if (strlen(line3) > 0) {
        u8g2->drawStr(5, 64, line3);
    }
    
    u8g2->sendBuffer();
    
    if (displayTime > 0) {
        delay(displayTime);
    }
}

// Display sensor data in a formatted layout
void displaySensorData(float temperature, float humidity, float pressure) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Title
    u8g2->setFont(u8g2_font_logisoso16_tr);
    drawCenteredText("Sensors", 18, u8g2_font_logisoso16_tr);
    
    // Temperature
    u8g2->setFont(u8g2_font_fur11_tf);
    char tempStr[20];
    snprintf(tempStr, sizeof(tempStr), "Temp: %.1f°C", temperature);
    u8g2->drawStr(5, 35, tempStr);
    
    // Humidity
    char humStr[20];
    snprintf(humStr, sizeof(humStr), "Humid: %.1f%%", humidity);
    u8g2->drawStr(5, 48, humStr);
    
    // Pressure
    char pressStr[20];
    snprintf(pressStr, sizeof(pressStr), "Press: %.0f hPa", pressure);
    u8g2->drawStr(5, 61, pressStr);
    
    // Draw icons
    drawTemperatureIcon(110, 25);
    
    u8g2->sendBuffer();
}

// Display network information
void displayNetworkInfo(const char* ssid, const char* ipAddress, int rssi) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Title
    u8g2->setFont(u8g2_font_logisoso16_tr);
    drawCenteredText("Network", 18, u8g2_font_logisoso16_tr);
    
    // SSID
    u8g2->setFont(u8g2_font_fur11_tf);
    char ssidStr[20];
    snprintf(ssidStr, sizeof(ssidStr), "SSID: %.12s", ssid);
    u8g2->drawStr(5, 35, ssidStr);
    
    // IP Address
    u8g2->setFont(u8g2_font_6x10_tf);
    char ipStr[20];
    snprintf(ipStr, sizeof(ipStr), "IP: %s", ipAddress);
    u8g2->drawStr(5, 48, ipStr);
    
    // Signal strength
    char rssiStr[20];
    snprintf(rssiStr, sizeof(rssiStr), "RSSI: %d dBm", rssi);
    u8g2->drawStr(5, 61, rssiStr);
    
    // Draw WiFi icon
    drawWiFiIcon(110, 25, rssi);
    
    u8g2->sendBuffer();
}

// Display camera information
void displayCameraInfo(const char* resolution, int fps, const char* format) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Title
    u8g2->setFont(u8g2_font_logisoso16_tr);
    drawCenteredText("Camera", 18, u8g2_font_logisoso16_tr);
    
    // Resolution
    u8g2->setFont(u8g2_font_fur11_tf);
    char resStr[20];
    snprintf(resStr, sizeof(resStr), "Res: %s", resolution);
    u8g2->drawStr(5, 35, resStr);
    
    // FPS
    char fpsStr[20];
    snprintf(fpsStr, sizeof(fpsStr), "FPS: %d", fps);
    u8g2->drawStr(5, 48, fpsStr);
    
    // Format
    char formatStr[20];
    snprintf(formatStr, sizeof(formatStr), "Format: %s", format);
    u8g2->drawStr(5, 61, formatStr);
    
    u8g2->sendBuffer();
}

// Display system status with progress bars
void displaySystemStatus(float cpuUsage, float memoryUsage, float batteryLevel) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Title
    u8g2->setFont(u8g2_font_logisoso16_tr);
    drawCenteredText("System", 18, u8g2_font_logisoso16_tr);
    
    // CPU Usage
    u8g2->setFont(u8g2_font_6x10_tf);
    char cpuStr[15];
    snprintf(cpuStr, sizeof(cpuStr), "CPU: %.1f%%", cpuUsage);
    u8g2->drawStr(5, 32, cpuStr);
    drawProgressBar(60, 25, 60, 8, (int)cpuUsage);
    
    // Memory Usage
    char memStr[15];
    snprintf(memStr, sizeof(memStr), "MEM: %.1f%%", memoryUsage);
    u8g2->drawStr(5, 45, memStr);
    drawProgressBar(60, 38, 60, 8, (int)memoryUsage);
    
    // Battery Level
    char batStr[15];
    snprintf(batStr, sizeof(batStr), "BAT: %.1f%%", batteryLevel);
    u8g2->drawStr(5, 58, batStr);
    drawProgressBar(60, 51, 60, 8, (int)batteryLevel);
    drawBatteryIcon(5, 60, (int)batteryLevel);
    
    u8g2->sendBuffer();
}

// Display date and time
void displayDateTime(int year, int month, int day, int hour, int minute, int second) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Date
    u8g2->setFont(u8g2_font_logisoso16_tr);
    char dateStr[12];
    snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%02d", day, month, year % 100);
    drawCenteredText(dateStr, 25, u8g2_font_logisoso16_tr);
    
    // Time
    u8g2->setFont(u8g2_font_inb19_mr);
    char timeStr[10];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hour, minute, second);
    drawCenteredText(timeStr, 50, u8g2_font_inb19_mr);
    
    u8g2->sendBuffer();
}

// Display custom logo text
void displayCustomLogo(const char* logoText) {
    if (!u8g2) return;
    
    u8g2->clearBuffer();
    
    // Main logo text
    u8g2->setFont(u8g2_font_inb19_mr);
    drawCenteredText(logoText, 35, u8g2_font_inb19_mr);
    
    // Draw decorative frame
    drawFrame(5, 5, 118, 54);
    
    u8g2->sendBuffer();
}

// Utility function to draw centered text
void drawCenteredText(const char* text, int y, const uint8_t* font) {
    if (!u8g2) return;
    
    u8g2->setFont(font);
    int textWidth = u8g2->getStrWidth(text);
    int x = (128 - textWidth) / 2;
    u8g2->drawStr(x, y, text);
}

// Draw a progress bar
void drawProgressBar(int x, int y, int width, int height, int progress) {
    if (!u8g2) return;
    
    // Draw border
    u8g2->drawFrame(x, y, width, height);
    
    // Draw fill
    int fillWidth = (progress * (width - 2)) / 100;
    if (fillWidth > 0) {
        u8g2->drawBox(x + 1, y + 1, fillWidth, height - 2);
    }
}

// Draw battery icon
void drawBatteryIcon(int x, int y, int batteryLevel) {
    if (!u8g2) return;
    
    // Battery outline
    u8g2->drawFrame(x, y, 14, 8);
    u8g2->drawBox(x + 14, y + 2, 2, 4);
    
    // Battery fill
    int fillWidth = (batteryLevel * 12) / 100;
    if (fillWidth > 0) {
        u8g2->drawBox(x + 1, y + 1, fillWidth, 6);
    }
}

// Draw WiFi icon
void drawWiFiIcon(int x, int y, int signalStrength) {
    if (!u8g2) return;
    
    // Convert RSSI to signal bars (0-4)
    int bars = 0;
    if (signalStrength > -50) bars = 4;
    else if (signalStrength > -60) bars = 3;
    else if (signalStrength > -70) bars = 2;
    else if (signalStrength > -80) bars = 1;
    
    // Draw signal bars
    for (int i = 0; i < 4; i++) {
        if (i < bars) {
            u8g2->drawBox(x + i * 3, y + (3 - i) * 2, 2, 2 + i * 2);
        } else {
            u8g2->drawFrame(x + i * 3, y + (3 - i) * 2, 2, 2 + i * 2);
        }
    }
}

// Draw temperature icon
void drawTemperatureIcon(int x, int y) {
    if (!u8g2) return;
    
    // Simple thermometer icon
    u8g2->drawCircle(x + 2, y + 10, 3);
    u8g2->drawBox(x + 1, y, 2, 8);
    u8g2->drawBox(x + 2, y + 7, 1, 4);
}

// Draw decorative frame
void drawFrame(int x, int y, int width, int height) {
    if (!u8g2) return;
    
    u8g2->drawFrame(x, y, width, height);
    u8g2->drawFrame(x + 2, y + 2, width - 4, height - 4);
}
