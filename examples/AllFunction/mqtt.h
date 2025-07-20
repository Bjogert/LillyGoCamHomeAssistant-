#ifndef MQTT_H
#define MQTT_H

#include <WiFiClient.h>
#include <PubSubClient.h>

// MQTT Topics - defined in mqtt.cpp
extern const char* mqtt_topic_frequency;
extern const char* mqtt_topic_quality;
extern const char* mqtt_topic_brightness;
extern const char* mqtt_topic_contrast;
extern const char* mqtt_topic_saturation;
extern const char* mqtt_topic_vflip;
extern const char* mqtt_topic_hmirror;
extern const char* mqtt_topic_screen_flip;  // Add screen flip topic
extern const char* mqtt_topic_status;

// Function declarations
bool setupMQTT();
void loopMQTT();
void publishCameraStatus();
void setCameraFrequency(int frequency_mhz);
void setCameraQuality(int quality);
void setCameraBrightness(int brightness);
void setCameraContrast(int contrast);
void setCameraSaturation(int saturation);
void setCameraVFlip(bool enable);
void setCameraHMirror(bool enable);

// CRITICAL: Force frame capture after setting change
bool forceCameraFrameCapture();

#endif
