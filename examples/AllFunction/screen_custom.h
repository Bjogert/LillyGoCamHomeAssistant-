/**
 * @file      screen_custom.h
 * @author    Modified for custom text display
 * @license   MIT
 * @copyright Copyright (c) 2023
 * @date      2023-07-06
 *
 * Enhanced screen functionality with custom text display capabilities
 */

#pragma once

#include <Wire.h>
#include <U8g2lib.h>

typedef void (*screen_off_cb_t)(void);

enum LilyGoTrigger {
    LILYGO_TRIGGER_FROM_NONE,
    LILYGO_TRIGGER_FROM_PIR,
    LILYGO_TRIGGER_FROM_VOICE,
    LILYGO_TRIGGER_FROM_CUSTOM, // Added for custom text display
};

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2;

// Original functions
void setupScreen(screen_off_cb_t cb, bool camera);
void loopScreen(LilyGoTrigger trigger);
void setScreenStatus(bool en);
void resetScreenTimer();
void startScreenTimer();

// Custom text display functions
void displayCustomMessage(const char* title, const char* message, int displayTime = 3000);
void displaySensorData(float temperature, float humidity, float pressure);
void displayNetworkInfo(const char* ssid, const char* ipAddress, int rssi);
void displayCameraInfo(const char* resolution, int fps, const char* format);
void displaySystemStatus(float cpuUsage, float memoryUsage, float batteryLevel);
void displayScrollingMessage(const char* message);
void displayDateTime(int year, int month, int day, int hour, int minute, int second);
void displayCustomLogo(const char* logoText);

// Utility functions for custom display
void drawCenteredText(const char* text, int y, const uint8_t* font);
void drawProgressBar(int x, int y, int width, int height, int progress);
void drawBatteryIcon(int x, int y, int batteryLevel);
void drawWiFiIcon(int x, int y, int signalStrength);
void drawTemperatureIcon(int x, int y);
void drawFrame(int x, int y, int width, int height);
