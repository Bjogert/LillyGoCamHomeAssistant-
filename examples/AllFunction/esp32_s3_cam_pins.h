#ifndef ESP32_S3_CAM_PINS_H
#define ESP32_S3_CAM_PINS_H

// ===========================================
// ESP32-S3 CAM Pin Definitions for OV2640
// Standard SKDJBJG ESP32-S3 WROOM N16R8 CAM
// ===========================================

// Camera pins for ESP32-S3 with OV2640
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1  
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40   // SDA
#define SIOC_GPIO_NUM     39   // SCL

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    13
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     21

// Optional LED flash (if available on your board)
// #define LED_GPIO_NUM      4   // Uncomment if you have an LED flash

// Board identification
#define CAMERA_MODEL_ESP32S3_CAM
#define BOARD_NAME "ESP32-S3 CAM"

#endif
