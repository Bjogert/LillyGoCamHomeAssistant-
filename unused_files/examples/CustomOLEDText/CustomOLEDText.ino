/**
 * @file      CustomOLEDText.ino
 * @author    Custom example for adding text to OLED
 * @license   MIT
 * @copyright Copyright (c) 2023
 * @date      2023-07-06
 *
 * This example demonstrates how to add custom text to the OLED screen on the LillyCam.
 * The screen is a 128x64 pixel SSD1306 OLED display connected via I2C.
 * Features:
 * - Custom "Chicken Camera" boot message
 * - PIR motion sensor detection
 * - "Smile, you're on camera!" message on motion detection
 */

#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include "secrets.h"

// Hardware pin definitions for LillyCam
#define I2C_SDA                     (7)
#define I2C_SCL                     (6)
#define PIR_INPUT_PIN               (17)

// Create U8G2 object for the OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

// Variables for text scrolling
int16_t scrollOffset = 0;
char scrollingText[] = "Chicken Camera is ready! Monitoring for motion...";
unsigned long lastUpdate = 0;
const unsigned long scrollDelay = 100; // milliseconds between scroll updates

// PIR motion sensor variables
volatile bool motionDetected = false;
unsigned long motionDetectedTime = 0;
const unsigned long motionDisplayTime = 3000; // Show motion message for 3 seconds
bool showingMotionMessage = false;

// Power saving variables
unsigned long lastActivityTime = 0;
const unsigned long sleepTimeout = 30000; // Turn off display after 30 seconds of inactivity
bool displaySleeping = false;

// WiFi status variables
bool wifiConnected = false;
int signalStrength = 0;
unsigned long lastWifiCheck = 0;
const unsigned long wifiCheckInterval = 5000; // Check WiFi every 5 seconds

void pirInterrupt() {
  motionDetected = true;
  motionDetectedTime = millis();
  lastActivityTime = millis(); // Reset sleep timer on motion
}

void setup() {
  Serial.begin(115200);
  
  // Wait for Serial to be ready
  delay(1000);
  Serial.println("Starting Chicken Camera...");
  
  // Initialize I2C with the correct pins for LillyCam
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize PIR sensor pin
  pinMode(PIR_INPUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_INPUT_PIN), pirInterrupt, RISING);
  
  // Initialize WiFi
  initWiFi();
  
  // Check if OLED is connected
  Wire.beginTransmission(0x3C);
  if (Wire.endTransmission() == 0) {
    Serial.println("OLED display found!");
    
    // Initialize the display
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFlipMode(0);
    u8g2.setFontMode(1); // Transparent
    u8g2.setDrawColor(1);
    u8g2.setFontDirection(0);
    
    // Show initial welcome screen
    showWelcomeScreen();
    delay(3000);
    
    // Initialize scrolling text
    scrollOffset = -(int16_t)u8g2.getDisplayWidth();
    lastActivityTime = millis(); // Initialize activity timer
    
  } else {
    Serial.println("OLED display not found!");
  }
  
  Serial.println("Setup complete!");
}

// WiFi functions
void initWiFi() {
  Serial.println("Initializing WiFi...");
  WiFi.begin(ssid, password);
  
  // Try to connect for 10 seconds
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
    signalStrength = WiFi.RSSI();
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    wifiConnected = false;
  }
}

void checkWiFiStatus() {
  if (millis() - lastWifiCheck > wifiCheckInterval) {
    lastWifiCheck = millis();
    
    if (WiFi.status() == WL_CONNECTED) {
      if (!wifiConnected) {
        Serial.println("WiFi reconnected!");
      }
      wifiConnected = true;
      signalStrength = WiFi.RSSI();
    } else {
      if (wifiConnected) {
        Serial.println("WiFi disconnected!");
      }
      wifiConnected = false;
      signalStrength = 0;
    }
  }
}

const char* getSignalStrengthText(int rssi) {
  if (rssi > -50) return "Excellent";
  else if (rssi > -60) return "Good";
  else if (rssi > -70) return "Fair";
  else if (rssi > -80) return "Poor";
  else return "Very Poor";
}

// Power saving functions
void putDisplayToSleep() {
  if (!displaySleeping) {
    Serial.println("Putting display to sleep for power saving...");
    u8g2.setPowerSave(1); // Turn off display
    displaySleeping = true;
  }
}

void wakeUpDisplay() {
  if (displaySleeping) {
    Serial.println("Waking up display...");
    u8g2.setPowerSave(0); // Turn on display
    displaySleeping = false;
    lastActivityTime = millis(); // Reset activity timer
  }
}

void loop() {
  // Check WiFi status periodically
  checkWiFiStatus();
  
  // Check for motion detection
  if (motionDetected) {
    motionDetected = false;
    showingMotionMessage = true;
    wakeUpDisplay(); // Wake up display on motion
    Serial.println("Motion detected! Showing smile message.");
    showMotionDetectedMessage();
  }
  
  // Check if we should stop showing motion message
  if (showingMotionMessage && (millis() - motionDetectedTime > motionDisplayTime)) {
    showingMotionMessage = false;
    scrollOffset = -(int16_t)u8g2.getDisplayWidth(); // Reset scroll
    Serial.println("Motion message timeout, returning to normal display.");
  }
  
  // Check if display should go to sleep
  if (!displaySleeping && (millis() - lastActivityTime > sleepTimeout)) {
    putDisplayToSleep();
  }
  
  // Update scrolling text every scrollDelay milliseconds (only if not showing motion message and not sleeping)
  if (!showingMotionMessage && !displaySleeping && (millis() - lastUpdate > scrollDelay)) {
    lastUpdate = millis();
    updateScrollingText();
  }
  
  // Add other functionality here
  delay(10);
}

void showWelcomeScreen() {
  u8g2.clearBuffer();
  
  // Draw title
  u8g2.setFont(u8g2_font_inb19_mr);
  u8g2.drawStr(0, 25, "Chicken");
  
  // Draw subtitle
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.drawStr(15, 45, "Camera");
  
  // Draw underline
  u8g2.drawHLine(2, 30, 80);
  
  // Draw version info
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 60, "v1.0 - Motion Detection Ready");
  
  u8g2.sendBuffer();
}

void updateScrollingText() {
  u8g2.clearBuffer();
  
  // Static text area
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.drawStr(5, 20, "Chicken Cam");
  
  // Draw separator line
  u8g2.drawHLine(0, 25, 128);
  
  // Current WiFi status display
  u8g2.setFont(u8g2_font_6x10_tf);
  char wifiStr[30];
  
  if (wifiConnected) {
    // Show signal strength
    snprintf(wifiStr, sizeof(wifiStr), "WiFi: %s (%ddBm)", getSignalStrengthText(signalStrength), signalStrength);
    u8g2.drawStr(5, 40, wifiStr);
  } else {
    u8g2.drawStr(5, 40, "WiFi: Disconnected");
  }
  
  // Scrolling text area
  drawScrollingText(scrollOffset, scrollingText);
  
  // Update scroll position
  int textWidth = strlen(scrollingText) * 8;
  if (scrollOffset < textWidth) {
    scrollOffset += 2;
  } else {
    scrollOffset = -(int16_t)u8g2.getDisplayWidth();
  }
  
  u8g2.sendBuffer();
}

void drawScrollingText(int16_t offset, const char *text) {
  static char buf[36]; // Buffer for visible text
  size_t len = strlen(text);
  size_t charOffset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;
  
  // Clear scrolling area
  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 50, u8g2.getDisplayWidth(), 14);
  u8g2.setDrawColor(1);
  
  if (offset < 0) {
    charOffset = (-offset) / 8;
    dx = offset + charOffset * 8;
    if (charOffset >= u8g2.getDisplayWidth() / 8) {
      return;
    }
    visible = u8g2.getDisplayWidth() / 8 - charOffset + 1;
    if (visible > len) visible = len;
    strncpy(buf, text, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(charOffset * 8 - dx, 62, buf);
  } else {
    charOffset = offset / 8;
    if (charOffset >= len) {
      return;
    }
    dx = offset - charOffset * 8;
    visible = len - charOffset;
    if (visible > u8g2.getDisplayWidth() / 8 + 1) {
      visible = u8g2.getDisplayWidth() / 8 + 1;
    }
    strncpy(buf, text + charOffset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
}

// Function to display custom static text
void displayCustomText(const char* line1, const char* line2, const char* line3) {
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.drawStr(5, 20, line1);
  
  u8g2.setFont(u8g2_font_fur11_tf);
  u8g2.drawStr(5, 35, line2);
  
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(5, 50, line3);
  
  u8g2.sendBuffer();
}

// Function to display sensor readings or status
void displayStatus(float temperature, float humidity, bool wifiConnected) {
  u8g2.clearBuffer();
  
  // Title
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.drawStr(20, 18, "Status");
  
  // Temperature
  u8g2.setFont(u8g2_font_fur11_tf);
  char tempStr[20];
  snprintf(tempStr, sizeof(tempStr), "Temp: %.1f°C", temperature);
  u8g2.drawStr(5, 35, tempStr);
  
  // Humidity
  char humStr[20];
  snprintf(humStr, sizeof(humStr), "Humidity: %.1f%%", humidity);
  u8g2.drawStr(5, 48, humStr);
  
  // WiFi Status
  u8g2.setFont(u8g2_font_6x10_tf);
  if (wifiConnected) {
    u8g2.drawStr(5, 62, "WiFi: Connected");
  } else {
    u8g2.drawStr(5, 62, "WiFi: Disconnected");
  }
  
  u8g2.sendBuffer();
}

// Function to show motion detection message
void showMotionDetectedMessage() {
  u8g2.clearBuffer();
  
  // Draw attention-grabbing border
  u8g2.drawFrame(0, 0, 128, 64);
  u8g2.drawFrame(1, 1, 126, 62);
  
  // Draw main message
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.drawStr(35, 25, "SMILE!");
  
  // Draw subtitle
  u8g2.setFont(u8g2_font_fur11_tf);
  u8g2.drawStr(5, 45, "You're on camera!");
  
  // Draw simple camera icon (rectangle with circle)
  u8g2.drawFrame(5, 15, 20, 15);
  u8g2.drawCircle(15, 22, 4);
  
  u8g2.sendBuffer();
}
