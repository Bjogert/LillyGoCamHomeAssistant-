/*
 * ESP32-S3 CAM with CameraWebServer + MQTT Home Assistant Integration
 * 
 * Based on Arduino CameraWebServer example with added MQTT functionality
 * for Home Assistant camera parameter control.
 * 
 * Features:
 * - Web-based camera streaming (robust Arduino implementation)
 * - MQTT integration for Home Assistant
 * - Camera parameter control via MQTT
 * - WiFi connectivity
 * - Optimized for ESP32-S3 with OV2640 camera
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp32_s3_cam_pins.h"

// Include simplified web server
extern void startCameraServer();

// ===========================
// WiFi Credentials
// ===========================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ===========================
// MQTT Configuration
// ===========================
const char* mqtt_server = "192.168.1.100";  // Your MQTT broker IP
const int mqtt_port = 1883;
const char* mqtt_user = "homeassistant";     // MQTT username
const char* mqtt_password = "mqtt_password"; // MQTT password
const char* mqtt_client_id = "esp32s3cam";   // Unique client ID

// MQTT Topics for Home Assistant
const char* mqtt_topic_availability = "homeassistant/camera/esp32s3cam/availability";
const char* mqtt_topic_config = "homeassistant/camera/esp32s3cam/config";
const char* mqtt_topic_command = "homeassistant/camera/esp32s3cam/set";
const char* mqtt_topic_state = "homeassistant/camera/esp32s3cam/state";

// Camera control topics
const char* mqtt_topic_quality = "homeassistant/camera/esp32s3cam/quality/set";
const char* mqtt_topic_brightness = "homeassistant/camera/esp32s3cam/brightness/set";
const char* mqtt_topic_contrast = "homeassistant/camera/esp32s3cam/contrast/set";
const char* mqtt_topic_saturation = "homeassistant/camera/esp32s3cam/saturation/set";
const char* mqtt_topic_framesize = "homeassistant/camera/esp32s3cam/framesize/set";

// Global objects
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Function declarations
void startCameraServer();
void setupCamera();
void setupMQTT();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishHomeAssistantDiscovery();
void setCameraParameter(const char* parameter, int value);

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println("ESP32-S3 CAM with CameraWebServer + MQTT");

    // Initialize camera
    setupCamera();

    // Connect to WiFi
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);

    Serial.print("WiFi connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");

    // Setup MQTT
    setupMQTT();

    // Start camera web server (Arduino's robust implementation)
    startCameraServer();

    Serial.println("System ready!");
}

void loop() {
    // Handle MQTT connection and messages
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }
    mqttClient.loop();
    
    // Small delay to prevent watchdog issues
    delay(10);
}

void setupCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG;  // for streaming
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    if (config.pixel_format == PIXFORMAT_JPEG) {
        if (psramFound()) {
            config.jpeg_quality = 10;
            config.fb_count = 2;
            config.grab_mode = CAMERA_GRAB_LATEST;
        } else {
            // Limit the frame size when PSRAM is not available
            config.frame_size = FRAMESIZE_SVGA;
            config.fb_location = CAMERA_FB_IN_DRAM;
        }
    }

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t* s = esp_camera_sensor_get();
    // Initial sensor setup - adjust for your camera orientation
    if (s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1);        // flip it back
        s->set_brightness(s, 1);   // up the brightness just a bit
        s->set_saturation(s, -2);  // lower the saturation
    }
    
    // Drop down frame size for higher initial frame rate
    if (config.pixel_format == PIXFORMAT_JPEG) {
        s->set_framesize(s, FRAMESIZE_QVGA);
    }

    Serial.println("Camera initialized successfully");
}

void setupMQTT() {
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(mqttCallback);
    
    // Connect to MQTT
    reconnectMQTT();
    
    // Publish Home Assistant discovery
    publishHomeAssistantDiscovery();
}

void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        
        if (mqttClient.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
            Serial.println(" connected");
            
            // Subscribe to command topics
            mqttClient.subscribe(mqtt_topic_command);
            mqttClient.subscribe(mqtt_topic_quality);
            mqttClient.subscribe(mqtt_topic_brightness);
            mqttClient.subscribe(mqtt_topic_contrast);
            mqttClient.subscribe(mqtt_topic_saturation);
            mqttClient.subscribe(mqtt_topic_framesize);
            
            // Publish availability
            mqttClient.publish(mqtt_topic_availability, "online", true);
            
            Serial.println("MQTT subscriptions active");
        } else {
            Serial.print(" failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Convert payload to string
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.printf("MQTT received [%s]: %s\n", topic, message.c_str());
    
    // Parse command and apply to camera
    if (strcmp(topic, mqtt_topic_quality) == 0) {
        setCameraParameter("quality", message.toInt());
    } else if (strcmp(topic, mqtt_topic_brightness) == 0) {
        setCameraParameter("brightness", message.toInt());
    } else if (strcmp(topic, mqtt_topic_contrast) == 0) {
        setCameraParameter("contrast", message.toInt());
    } else if (strcmp(topic, mqtt_topic_saturation) == 0) {
        setCameraParameter("saturation", message.toInt());
    } else if (strcmp(topic, mqtt_topic_framesize) == 0) {
        setCameraParameter("framesize", message.toInt());
    }
}

void setCameraParameter(const char* parameter, int value) {
    sensor_t* s = esp_camera_sensor_get();
    if (!s) {
        Serial.println("Failed to get camera sensor");
        return;
    }
    
    if (strcmp(parameter, "quality") == 0) {
        // JPEG quality (0-63, lower is better quality)
        s->set_quality(s, constrain(value, 0, 63));
        Serial.printf("Set quality to %d\n", value);
    } else if (strcmp(parameter, "brightness") == 0) {
        // Brightness (-2 to +2)
        s->set_brightness(s, constrain(value, -2, 2));
        Serial.printf("Set brightness to %d\n", value);
    } else if (strcmp(parameter, "contrast") == 0) {
        // Contrast (-2 to +2)
        s->set_contrast(s, constrain(value, -2, 2));
        Serial.printf("Set contrast to %d\n", value);
    } else if (strcmp(parameter, "saturation") == 0) {
        // Saturation (-2 to +2)
        s->set_saturation(s, constrain(value, -2, 2));
        Serial.printf("Set saturation to %d\n", value);
    } else if (strcmp(parameter, "framesize") == 0) {
        // Frame size (see framesize_t enum)
        s->set_framesize(s, (framesize_t)constrain(value, 0, FRAMESIZE_INVALID));
        Serial.printf("Set framesize to %d\n", value);
    }
}

void publishHomeAssistantDiscovery() {
    String config_topic = "homeassistant/camera/esp32s3cam/config";
    String config_payload = "{"
                           "\"name\":\"ESP32-S3 Camera\","
                           "\"stream_url\":\"http://" + WiFi.localIP().toString() + "/stream\","
                           "\"still_image_url\":\"http://" + WiFi.localIP().toString() + "/capture\","
                           "\"availability_topic\":\"" + String(mqtt_topic_availability) + "\","
                           "\"unique_id\":\"esp32s3cam_camera\","
                           "\"device\":{"
                           "\"identifiers\":[\"esp32s3cam\"],"
                           "\"name\":\"ESP32-S3 Camera\","
                           "\"model\":\"SKDJBJG ESP32-S3 CAM\","
                           "\"manufacturer\":\"Custom\""
                           "}"
                           "}";
    
    mqttClient.publish(config_topic.c_str(), config_payload.c_str(), true);
    Serial.println("Home Assistant discovery published");
}
