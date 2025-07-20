/**
 * @file      screen.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2022-09-16
 *
 */
#include "screen.h"
#include "WiFi.h"
#include "network.h"
#include "esp_camera.h"
#include "utilities.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2 = NULL;
static TimerHandle_t timerHandle = NULL;
static char buffer[256] = {0};
static bool screenOff = false;
static screen_off_cb_t off_cb = NULL;
/*
  Draw a string with specified pixel offset.
  The offset can be negative.
  Limitation: The monochrome font with 8 pixel per glyph
*/
void drawScrollString(int16_t offset, const char *s)
{
    static char buf[36];  // should for screen with up to 256 pixel width
    size_t len;
    size_t char_offset = 0;
    u8g2_uint_t dx = 0;
    size_t visible = 0;
    u8g2->setDrawColor(0);     // clear the scrolling area
    u8g2->drawBox(0, 49, u8g2->getDisplayWidth(), u8g2->getDisplayHeight() - 1);
    u8g2->setDrawColor(1);     // set the color for the text
    len = strlen(s);
    if ( offset < 0 ) {
        char_offset = (-offset) / 8;
        dx = offset + char_offset * 8;
        if ( char_offset >= u8g2->getDisplayWidth() / 8 )
            return;
        visible = u8g2->getDisplayWidth() / 8 - char_offset + 1;
        strncpy(buf, s, visible);
        buf[visible] = '\0';
        u8g2->setFont(u8g2_font_8x13_mf);
        u8g2->drawStr(char_offset * 8 - dx, 62, buf);
    } else {
        char_offset = offset / 8;
        if ( char_offset >= len )
            return;   // nothing visible
        dx = offset - char_offset * 8;
        visible = len - char_offset;
        if ( visible > u8g2->getDisplayWidth() / 8 + 1 )
            visible = u8g2->getDisplayWidth() / 8 + 1;
        strncpy(buf, s + char_offset, visible);
        buf[visible] = '\0';
        u8g2->setFont(u8g2_font_8x13_mf);
        u8g2->drawStr(-dx, 62, buf);
    }
}


void screenTimerCallback(TimerHandle_t timer)
{
    if (off_cb) {
        off_cb();
    }
    setScreenStatus(true);
}

void setupScreen(screen_off_cb_t cb, bool camera)
{
    off_cb = cb;
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
        
        // Show 
        u8g2->setFont(u8g2_font_inb19_mr);
        u8g2->drawStr(2, 35, "Kamera2");
        u8g2->sendBuffer();
        delay(3000);  // Show for 3 seconds
        
        // After 3 seconds, clear screen (go to rest mode)
        u8g2->clearBuffer();
        u8g2->sendBuffer();
    }
}


void startScreenTimer()
{
    if (!timerHandle) {
        timerHandle = xTimerCreate("App/timer", pdMS_TO_TICKS(5000), true, NULL, screenTimerCallback);
        xTimerStart(timerHandle, portMAX_DELAY);
    }
}

void resetScreenTimer()
{
    xTimerReset(timerHandle, portMAX_DELAY);
}

void setScreenStatus(bool en)
{
    if (screenOff == en)return;
    screenOff = en;
    if (u8g2) {
        u8g2->setPowerSave(en);
    }
}

void loopScreen(LilyGoTrigger trigger)
{
    static LilyGoTrigger lastTrigger = LILYGO_TRIGGER_FROM_NONE;
    static unsigned long lastWiFiUpdate = 0;
    static String lastSignalText = "";

    if (!u8g2) {
        return;
    }
    
    if (screenOff && trigger == LILYGO_TRIGGER_FROM_NONE) {
        return;
    }

    bool needsUpdate = false;
    
    // Check if trigger changed
    if (lastTrigger != trigger) {
        lastTrigger = trigger;
        needsUpdate = true;
        
        // Clear the entire screen first
        u8g2->clearBuffer();
        
        if (trigger == LILYGO_TRIGGER_FROM_PIR) {
            // Show "SMILE!" message
            u8g2->setFont(u8g2_font_logisoso16_tr);
            u8g2->drawStr(45, 30, "SMILE!");
        }
        // If trigger is NONE, screen stays clear (rest mode)
    }
    
    // Update WiFi signal every 5 seconds or when trigger changes
    if (millis() - lastWiFiUpdate > 5000 || needsUpdate) {
        int32_t rssi = WiFi.RSSI();
        char signalText[16];
        snprintf(signalText, sizeof(signalText), "WiFi:%ddBm", rssi);
        
        // Only update if text changed or we need a full update
        if (strcmp(signalText, lastSignalText.c_str()) != 0 || needsUpdate) {
            // Clear the bottom area for WiFi signal (make sure we clear enough space)
            u8g2->setDrawColor(0);
            u8g2->drawBox(0, 52, 128, 12);
            u8g2->setDrawColor(1);
            
            // Draw new WiFi signal text
            u8g2->setFont(u8g2_font_6x10_tf);
            u8g2->drawStr(5, 62, signalText);
            
            lastSignalText = String(signalText);
            needsUpdate = true;
        }
        
        lastWiFiUpdate = millis();
    }
    
    // Only send buffer if something changed
    if (needsUpdate) {
        u8g2->sendBuffer();
    }
}

void setScreenFlip(bool enable) {
    if (!u8g2) {
        Serial.println("ERROR: Screen not initialized");
        return;
    }
    
    Serial.printf("Setting screen flip to %s\n", enable ? "enabled (180°)" : "disabled (normal)");
    
    // U8g2 setFlipMode: 0 = normal, 1 = 180° rotation
    u8g2->setFlipMode(enable ? 1 : 0);
    
    // Force screen refresh to show the flip immediately
    u8g2->clearBuffer();
    
    // Redraw current content
    u8g2->setFont(u8g2_font_6x10_tf);
    u8g2->drawStr(5, 15, enable ? "Screen: Flipped" : "Screen: Normal");
    
    // Show WiFi status
    int32_t rssi = WiFi.RSSI();
    char signalText[16];
    snprintf(signalText, sizeof(signalText), "WiFi:%ddBm", rssi);
    u8g2->drawStr(5, 62, signalText);
    
    u8g2->sendBuffer();
    
    Serial.printf("✓ Screen flip %s\n", enable ? "enabled (180°)" : "disabled (normal)");
}





















