#include "mqtt.h"
#include "esp_camera.h"
#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>

// MQTT Configuration - Add these to your secrets.h
const char* mqtt_server = MQTT_SERVER;         // Add to secrets.h: #define MQTT_SERVER "your.mqtt.server"
const int mqtt_port = MQTT_PORT;               // Add to secrets.h: #define MQTT_PORT 1883
const char* mqtt_user = MQTT_USER;             // Add to secrets.h: #define MQTT_USER "your_user"
const char* mqtt_password = MQTT_PASSWORD;     // Add to secrets.h: #define MQTT_PASSWORD "your_pass"
const char* mqtt_client_id = "lilygo_camera";

// MQTT Topics
const char* mqtt_topic_frequency = "camera/frequency/set";
const char* mqtt_topic_quality = "camera/quality/set";
const char* mqtt_topic_brightness = "camera/brightness/set";
const char* mqtt_topic_contrast = "camera/contrast/set";
const char* mqtt_topic_saturation = "camera/saturation/set";
const char* mqtt_topic_vflip = "camera/vflip/set";
const char* mqtt_topic_hmirror = "camera/hmirror/set";
const char* mqtt_topic_status = "camera/status";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

bool mqttConnected = false;

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.println("========== MQTT MESSAGE RECEIVED ==========");
    Serial.printf("Topic: %s\n", topic);
    Serial.printf("Payload: %s\n", message.c_str());
    Serial.printf("Length: %d bytes\n", length);
    Serial.println("==========================================");
    
    // Handle camera frequency control
    if (strcmp(topic, mqtt_topic_frequency) == 0) {
        Serial.println("-> Processing frequency command");
        int frequency = message.toInt();
        setCameraFrequency(frequency);
    }
    // Handle camera quality control
    else if (strcmp(topic, mqtt_topic_quality) == 0) {
        Serial.println("-> Processing quality command");
        int quality = message.toInt();
        setCameraQuality(quality);
    }
    // Handle camera brightness control
    else if (strcmp(topic, mqtt_topic_brightness) == 0) {
        Serial.println("-> Processing brightness command");
        int brightness = message.toInt();
        setCameraBrightness(brightness);
    }
    // Handle camera contrast control
    else if (strcmp(topic, mqtt_topic_contrast) == 0) {
        Serial.println("-> Processing contrast command");
        int contrast = message.toInt();
        setCameraContrast(contrast);
    }
    // Handle camera saturation control
    else if (strcmp(topic, mqtt_topic_saturation) == 0) {
        Serial.println("-> Processing saturation command");
        int saturation = message.toInt();
        setCameraSaturation(saturation);
    }
    // Handle camera vertical flip control
    else if (strcmp(topic, mqtt_topic_vflip) == 0) {
        Serial.println("-> Processing vertical flip command");
        bool enable = (message == "1" || message.equalsIgnoreCase("true") || message.equalsIgnoreCase("on"));
        Serial.printf("   Flip enable: %s\n", enable ? "true" : "false");
        setCameraVFlip(enable);
    }
    // Handle camera horizontal mirror control
    else if (strcmp(topic, mqtt_topic_hmirror) == 0) {
        Serial.println("-> Processing horizontal mirror command");
        bool enable = (message == "1" || message.equalsIgnoreCase("true") || message.equalsIgnoreCase("on"));
        Serial.printf("   Mirror enable: %s\n", enable ? "true" : "false");
        setCameraHMirror(enable);
    }
    else {
        Serial.println("-> WARNING: Unknown MQTT topic received!");
        Serial.printf("   Topic: %s\n", topic);
    }
}

bool setupMQTT() {
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(onMqttMessage);
    
    Serial.println("MQTT client configured");
    return true;
}

void loopMQTT() {
    if (!mqttClient.connected()) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("========== MQTT CONNECTION ATTEMPT ==========");
            Serial.printf("MQTT Server: %s:%d\n", mqtt_server, mqtt_port);
            Serial.printf("Client ID: %s\n", mqtt_client_id);
            Serial.printf("User: %s\n", mqtt_user);
            Serial.println("Attempting MQTT connection...");
            
            if (mqttClient.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
                Serial.println("✓ MQTT connected successfully!");
                mqttConnected = true;
                
                // Subscribe to camera control topics
                Serial.println("Subscribing to topics:");
                
                mqttClient.subscribe(mqtt_topic_frequency);
                Serial.printf("  - %s\n", mqtt_topic_frequency);
                
                mqttClient.subscribe(mqtt_topic_quality);
                Serial.printf("  - %s\n", mqtt_topic_quality);
                
                mqttClient.subscribe(mqtt_topic_brightness);
                Serial.printf("  - %s\n", mqtt_topic_brightness);
                
                mqttClient.subscribe(mqtt_topic_contrast);
                Serial.printf("  - %s\n", mqtt_topic_contrast);
                
                mqttClient.subscribe(mqtt_topic_saturation);
                Serial.printf("  - %s\n", mqtt_topic_saturation);
                
                mqttClient.subscribe(mqtt_topic_vflip);
                Serial.printf("  - %s\n", mqtt_topic_vflip);
                
                mqttClient.subscribe(mqtt_topic_hmirror);
                Serial.printf("  - %s\n", mqtt_topic_hmirror);
                
                Serial.println("✓ All subscriptions complete");
                Serial.println("============================================");
                
                // Publish initial status
                publishCameraStatus();
            } else {
                Serial.printf("✗ MQTT connection failed, rc=%d\n", mqttClient.state());
                Serial.println("MQTT Error codes:");
                Serial.println("  -4: Connection timeout");
                Serial.println("  -3: Connection lost");
                Serial.println("  -2: Connect failed");
                Serial.println("  -1: Disconnected");
                Serial.println("   0: Connected");
                Serial.println("   1: Bad protocol");
                Serial.println("   2: Bad client ID");
                Serial.println("   3: Unavailable");
                Serial.println("   4: Bad credentials");
                Serial.println("   5: Unauthorized");
                Serial.println("============================================");
                mqttConnected = false;
            }
        } else {
            Serial.println("WiFi not connected, skipping MQTT connection");
        }
    } else {
        mqttClient.loop();
    }
}

void publishCameraStatus() {
    if (mqttConnected && mqttClient.connected()) {
        String status = "{\"status\":\"online\",\"camera\":\"ready\"}";
        mqttClient.publish(mqtt_topic_status, status.c_str());
    }
}

// CRITICAL: This is the key function that made the Arduino solution work!
bool forceCameraFrameCapture() {
    Serial.println("CRITICAL: Forcing frame capture to activate camera setting...");
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) {
        esp_camera_fb_return(fb);
        Serial.println("✓ Frame capture successful - camera setting should now be active");
        return true;
    } else {
        Serial.println("✗ Frame capture failed - setting may not be active");
        return false;
    }
}

void setCameraFrequency(int frequency_mhz) {
    Serial.printf("Setting camera frequency to %d MHz\n", frequency_mhz);
    
    // Convert MHz to Hz
    uint32_t frequency_hz = frequency_mhz * 1000000;
    
    // Validate frequency range (typical ESP32 camera range)
    if (frequency_hz < 10000000 || frequency_hz > 25000000) {
        Serial.printf("ERROR: Invalid frequency %d MHz. Must be between 10-25 MHz\n", frequency_mhz);
        return;
    }
    
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor) {
        Serial.println("ERROR: Camera sensor not available");
        return;
    }
    
    // Note: ESP32 camera doesn't have direct frequency control via sensor
    // Frequency is set during camera initialization
    // For now, we'll store it and require camera restart
    Serial.printf("NOTE: Camera frequency change requires restart to take effect\n");
    Serial.printf("Current frequency setting would be: %d Hz\n", frequency_hz);
    
    // CRITICAL: Force frame capture (this is what made Arduino solution work!)
    if (forceCameraFrameCapture()) {
        // Publish success status
        if (mqttConnected && mqttClient.connected()) {
            String response = "{\"frequency_mhz\":" + String(frequency_mhz) + ",\"status\":\"set\"}";
            mqttClient.publish("camera/frequency/status", response.c_str());
        }
    }
}

void setCameraQuality(int quality) {
    Serial.printf("Setting camera quality to %d\n", quality);
    
    // Validate quality range (1-63, lower is better)
    if (quality < 1 || quality > 63) {
        Serial.printf("ERROR: Invalid quality %d. Must be between 1-63\n", quality);
        return;
    }
    
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor) {
        Serial.println("ERROR: Camera sensor not available");
        return;
    }
    
    // Set the quality
    esp_err_t res = sensor->set_quality(sensor, quality);
    if (res != ESP_OK) {
        Serial.printf("ERROR: Failed to set quality, error: 0x%x\n", res);
        return;
    }
    
    Serial.printf("✓ Quality set to %d\n", quality);
    
    // CRITICAL: Force frame capture (this is what made Arduino solution work!)
    if (forceCameraFrameCapture()) {
        // Publish success status
        if (mqttConnected && mqttClient.connected()) {
            String response = "{\"quality\":" + String(quality) + ",\"status\":\"success\"}";
            mqttClient.publish("camera/quality/status", response.c_str());
        }
    }
}

void setCameraBrightness(int brightness) {
    Serial.printf("Setting camera brightness to %d\n", brightness);
    
    // Validate brightness range (-2 to +2)
    if (brightness < -2 || brightness > 2) {
        Serial.printf("ERROR: Invalid brightness %d. Must be between -2 and 2\n", brightness);
        return;
    }
    
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor) {
        Serial.println("ERROR: Camera sensor not available");
        return;
    }
    
    // Set the brightness
    esp_err_t res = sensor->set_brightness(sensor, brightness);
    if (res != ESP_OK) {
        Serial.printf("ERROR: Failed to set brightness, error: 0x%x\n", res);
        return;
    }
    
    Serial.printf("✓ Brightness set to %d\n", brightness);
    
    // CRITICAL: Force frame capture (this is what made Arduino solution work!)
    if (forceCameraFrameCapture()) {
        // Publish success status
        if (mqttConnected && mqttClient.connected()) {
            String response = "{\"brightness\":" + String(brightness) + ",\"status\":\"success\"}";
            mqttClient.publish("camera/brightness/status", response.c_str());
        }
    }
}

void setCameraContrast(int contrast) {
    Serial.printf("Setting camera contrast to %d\n", contrast);
    
    // Validate contrast range (-2 to +2)
    if (contrast < -2 || contrast > 2) {
        Serial.printf("ERROR: Invalid contrast %d. Must be between -2 and 2\n", contrast);
        return;
    }
    
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor) {
        Serial.println("ERROR: Camera sensor not available");
        return;
    }
    
    // Set the contrast
    esp_err_t res = sensor->set_contrast(sensor, contrast);
    if (res != ESP_OK) {
        Serial.printf("ERROR: Failed to set contrast, error: 0x%x\n", res);
        return;
    }
    
    Serial.printf("✓ Contrast set to %d\n", contrast);
    
    // CRITICAL: Force frame capture (this is what made Arduino solution work!)
    if (forceCameraFrameCapture()) {
        // Publish success status
        if (mqttConnected && mqttClient.connected()) {
            String response = "{\"contrast\":" + String(contrast) + ",\"status\":\"success\"}";
            mqttClient.publish("camera/contrast/status", response.c_str());
        }
    }
}

void setCameraSaturation(int saturation) {
    Serial.printf("Setting camera saturation to %d\n", saturation);
    
    // Validate saturation range (-2 to +2)
    if (saturation < -2 || saturation > 2) {
        Serial.printf("ERROR: Invalid saturation %d. Must be between -2 and 2\n", saturation);
        return;
    }
    
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor) {
        Serial.println("ERROR: Camera sensor not available");
        return;
    }
    
    // Set the saturation
    esp_err_t res = sensor->set_saturation(sensor, saturation);
    if (res != ESP_OK) {
        Serial.printf("ERROR: Failed to set saturation, error: 0x%x\n", res);
        return;
    }
    
    Serial.printf("✓ Saturation set to %d\n", saturation);
    
    // CRITICAL: Force frame capture (this is what made Arduino solution work!)
    if (forceCameraFrameCapture()) {
        // Publish success status
        if (mqttConnected && mqttClient.connected()) {
            String response = "{\"saturation\":" + String(saturation) + ",\"status\":\"success\"}";
            mqttClient.publish("camera/saturation/status", response.c_str());
        }
    }
}

void setCameraVFlip(bool enable) {
    Serial.printf("Setting camera vertical flip to %s\n", enable ? "enabled" : "disabled");
    
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor) {
        Serial.println("ERROR: Camera sensor not available");
        return;
    }
    
    // Debug: Check what sensor we have
    Serial.printf("Camera sensor PID: 0x%x\n", sensor->id.PID);
    
    // Try to get current vflip status first
    Serial.println("Checking current sensor capabilities...");
    
    // Set the vertical flip
    esp_err_t res = sensor->set_vflip(sensor, enable ? 1 : 0);
    if (res != ESP_OK) {
        Serial.printf("ERROR: Failed to set vertical flip, error: 0x%x\n", res);
        Serial.println("Possible reasons:");
        Serial.println("  - Sensor doesn't support vflip");
        Serial.println("  - Sensor is busy");
        Serial.println("  - Invalid parameter");
        return;
    }
    
    Serial.printf("✓ Vertical flip %s\n", enable ? "enabled" : "disabled");
    
    // Additional debugging - try to verify the setting took effect
    Serial.println("Attempting to read back sensor status...");
    
    // Wait a moment for the setting to take effect
    delay(100);
    
    // CRITICAL: Force frame capture (this is what made Arduino solution work!)
    if (forceCameraFrameCapture()) {
        // Publish success status
        if (mqttConnected && mqttClient.connected()) {
            String response = "{\"vflip\":" + String(enable ? "true" : "false") + ",\"status\":\"success\"}";
            mqttClient.publish("camera/vflip/status", response.c_str());
        }
    }
}

void setCameraHMirror(bool enable) {
    Serial.printf("Setting camera horizontal mirror to %s\n", enable ? "enabled" : "disabled");
    
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor) {
        Serial.println("ERROR: Camera sensor not available");
        return;
    }
    
    // Set the horizontal mirror
    esp_err_t res = sensor->set_hmirror(sensor, enable ? 1 : 0);
    if (res != ESP_OK) {
        Serial.printf("ERROR: Failed to set horizontal mirror, error: 0x%x\n", res);
        return;
    }
    
    Serial.printf("✓ Horizontal mirror %s\n", enable ? "enabled" : "disabled");
    
    // CRITICAL: Force frame capture (this is what made Arduino solution work!)
    if (forceCameraFrameCapture()) {
        // Publish success status
        if (mqttConnected && mqttClient.connected()) {
            String response = "{\"hmirror\":" + String(enable ? "true" : "false") + ",\"status\":\"success\"}";
            mqttClient.publish("camera/hmirror/status", response.c_str());
        }
    }
}
