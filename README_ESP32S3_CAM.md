# ESP32-S3 CAM WebServer with MQTT Home Assistant Integration

This branch contains a clean implementation for **ESP32-S3 CAM boards** (like SKDJBJG ESP32-S3 WROOM N16R8 CAM) with **OV2640 camera**.

## 🎯 What This Branch Does

- **Stripped out all LilyGo dependencies** (no OLED, no PIR, no power management, no buttons)
- **Arduino CameraWebServer implementation** - robust and proven web streaming
- **MQTT Home Assistant integration** - full discovery and camera parameter control
- **Simple web interface** for camera viewing and control
- **Optimized for basic ESP32-S3 CAM boards**

## 🔧 Hardware Support

- **Board**: SKDJBJG ESP32-S3 WROOM N16R8 CAM (or similar ESP32-S3 CAM boards)
- **Camera**: OV2640 
- **Memory**: Uses PSRAM for better performance
- **Features**: WiFi connectivity, web streaming, MQTT control

## 📋 Configuration Required

### 1. WiFi Settings
Edit in `ESP32S3_CameraWebServer.cpp`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 2. MQTT Settings
```cpp
const char* mqtt_server = "192.168.1.100";  // Your MQTT broker IP
const char* mqtt_user = "homeassistant";     
const char* mqtt_password = "mqtt_password"; 
```

### 3. Upload Settings
Update your COM port in `platformio.ini`:
```ini
upload_port = COM12  ; Change to your port
monitor_port = COM12
```

## 🚀 Features

### Web Server
- **Stream URL**: `http://ESP_IP/stream`
- **Capture URL**: `http://ESP_IP/capture`
- **Web Interface**: `http://ESP_IP/`

### MQTT Topics (Home Assistant)
- **Quality control**: `homeassistant/camera/esp32s3cam/quality/set`
- **Brightness**: `homeassistant/camera/esp32s3cam/brightness/set`
- **Contrast**: `homeassistant/camera/esp32s3cam/contrast/set`
- **Saturation**: `homeassistant/camera/esp32s3cam/saturation/set`
- **Frame size**: `homeassistant/camera/esp32s3cam/framesize/set`

### Home Assistant Integration
- **Automatic discovery** - camera appears automatically in HA
- **Real-time streaming** via HTTP
- **Parameter control** via MQTT
- **Availability status**

## 📦 Files in This Implementation

### Core Files
- `ESP32S3_CameraWebServer.cpp` - Main application logic
- `app_httpd_simple.cpp` - HTTP server implementation (simplified Arduino version)
- `simple_camera_index.h` - Web interface HTML
- `esp32_s3_cam_pins.h` - Pin definitions for ESP32-S3 CAM
- `main.cpp` - PlatformIO entry point

### Configuration
- `platformio.ini` - Build configuration for ESP32-S3

## 🔨 Build and Upload

```powershell
# Build the project
pio run -e esp32s3cam

# Upload to your board
pio run -e esp32s3cam -t upload

# Monitor serial output
pio device monitor -p COM12
```

## 🎭 What Was Removed from Original

- ❌ OLED screen functionality (`screen.h/cpp`, `screen_custom.h/cpp`)
- ❌ PIR motion detection
- ❌ Physical button controls (`button.h/cpp`)
- ❌ Power management (`power.h/cpp`, XPowersLib)
- ❌ Complex peripherals management
- ❌ Deep sleep functionality
- ❌ All LilyGo-specific libraries

## ✅ What You Get

- ✅ **Robust camera streaming** (Arduino's proven implementation)
- ✅ **MQTT integration** for Home Assistant
- ✅ **Simple web interface**
- ✅ **Camera parameter control**
- ✅ **Automatic HA discovery**
- ✅ **Clean, minimal codebase**

## 🏠 Home Assistant Setup

1. Make sure MQTT integration is configured in HA
2. Flash this code to your ESP32-S3 CAM
3. The camera will **automatically appear** in Home Assistant
4. Use MQTT or HA interface to control camera parameters

## 📡 Serial Output Example

When working correctly, you should see:
```
ESP32-S3 CAM with CameraWebServer + MQTT
psram is found !
Camera initialized successfully  
WiFi connecting.....
WiFi connected
Camera Ready! Use 'http://192.168.1.XXX' to connect
MQTT connected
Home Assistant discovery published
System ready!
```

## 🛠 Troubleshooting

**Camera won't initialize**: Check pin definitions in `esp32_s3_cam_pins.h`
**WiFi won't connect**: Verify SSID and password
**MQTT issues**: Check broker IP and credentials
**Upload fails**: Verify COM port and board selection

This is a **clean, production-ready** implementation for basic ESP32-S3 CAM boards! 🎉
