

/**

 * DEPENDENCIES - Files used by this main file:
 * 
 * LOCAL HEADER FILES (in this folder):
 * - screen.h/screen.cpp           - OLED display management
 * - screen_custom.h/screen_custom.cpp - Custom screen layouts and UI
 * - camera.h/camera.cpp           - Camera initialization and control
 * - button.h/button.cpp           - Physical button handling
 * - power.h/power.cpp             - Power management (PMU/battery)
 * - network.h/network.cpp         - WiFi and network connectivity
 * - server.h/server.cpp           - HTTP server for camera streaming
 * - utilities.h                   - Pin definitions and hardware config
 * - app_httpd.cpp                 - Alternative HTTP server implementation
 * - camera_index.h                - HTML/CSS for camera web interface
 * - secrets.h                     - WiFi credentials (create from secrets.h.example)
 * 
 * EXTERNAL LIBRARIES (in lib/ folder):
 * - XPowersLib                    - AXP2101 power management IC
 * - U8g2                          - OLED display driver
 * - AceButton                     - Button debouncing and events
 * - ESP32QRCodeReader             - QR code detection (if used)
 * 
 * SYSTEM CONFIGURATION:
 * - platformio.ini                - Build configuration and compile flags
 * - partitions.csv                - ESP32 memory partitioning
 * 
 * FEATURES ENABLED:
 * - Camera streaming via HTTP server
 * - OLED display with status information
 * - PIR motion detection
 * - Physical button controls
 * - Power management and battery monitoring
 * - WiFi connectivity (AP or Station mode)
 * - Deep sleep functionality
 * - Watchdog timer protection
 */

#include "screen.h"
#include "camera.h"
#include "button.h"
#include "power.h"
#include "network.h"
#include "server.h"
#include "utilities.h"
#include "esp_task_wdt.h" // Add watchdog timer header
#include "esp_camera.h"

void startCameraServer();

void getWakeupReason();

static LilyGoTrigger status = LILYGO_TRIGGER_FROM_NONE;

void clearPheralsEvent()
{
    status = LILYGO_TRIGGER_FROM_NONE;
}

void pir_interrupt_event()
{
    // PIR motion detected - wake up screen
    resetScreenTimer();
    setScreenStatus(false);
}


void loopPeripherals(void *ptr)
{
    pinMode(PIR_INPUT_PIN, INPUT);
    //Each state change will trigger an interrupt,
    //if you only want to trigger when a human body is sensed, change this to RISING
    attachInterrupt(PIR_INPUT_PIN, pir_interrupt_event, CHANGE);

    // Initialize the external extension pins
    pinMode(EXTERN_PIN1, OUTPUT);
    pinMode(EXTERN_PIN2, OUTPUT);

    while (1) {
        loopScreen(status);
        loopPower();
        loopNetwork();
        loopButton();
        delay(8);
    }
}

void setup()
{
    bool ret = false;

    Serial.begin(115200);

    getWakeupReason();

    if (psramFound()) {
        Serial.println("psram is found !");
    } else {
        Serial.println("psram not found !");
    }

    // Initialize the board power parameters
    setupPower();

    // Initialize the camera
    ret = setupCamera();

    // Initialize the screen
    setupScreen(clearPheralsEvent, ret);

    while (!ret) {
        delay(1000);
    }

    // Start button trigger, stand-alone will set camera resolution, long press will set sleep
    setupButton();

    // Start the network, use AP hotspot mode by default
    setupNetwork(USING_AP_MODE);

    // Custom Transport Server
    setupServer();

    // Initialize task watchdog (30 second timeout)
    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);

    /*
    * espressif official example, using asynchronous streaming, unstable, not recommended
    * */
    // startCameraServer();

    xTaskCreate(loopPeripherals, "App/per", 4 * 1024, NULL, 8, NULL);

    // Enable screen timeout off display
    startScreenTimer();

}

void loop()
{
    // Reset watchdog timer
    esp_task_wdt_reset();
    
    loopServer();
    
    // Small delay to prevent tight loop
    delay(1);
}



void getWakeupReason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_UNDEFINED:
        //!< In case of deep sleep, reset was not caused by exit from deep sleep
        Serial.println("In case of deep sleep, reset was not caused by exit from deep sleep");
        break;
    case ESP_SLEEP_WAKEUP_ALL:
        //!< Not a wakeup cause: used to disable all wakeup sources with esp_sleep_disable_wakeup_source
        Serial.println("Not a wakeup cause: used to disable all wakeup sources with esp_sleep_disable_wakeup_source");
        break;
    case ESP_SLEEP_WAKEUP_EXT0:
        //!< Wakeup caused by external signal using RTC_IO
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        //!< Wakeup caused by external signal using RTC_CNTL
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        //!< Wakeup caused by timer
        Serial.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        //!< Wakeup caused by touchpad
        Serial.println("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        //!< Wakeup caused by ULP program
        Serial.println("Wakeup caused by ULP program");
        break;
    case  ESP_SLEEP_WAKEUP_GPIO:
        //!< Wakeup caused by GPIO (light sleep only)
        Serial.println("Wakeup caused by GPIO (light sleep only)");
        break;
    case ESP_SLEEP_WAKEUP_UART:
        //!< Wakeup caused by UART (light sleep only)
        Serial.println("Wakeup caused by UART (light sleep only)");
        break;
    case ESP_SLEEP_WAKEUP_WIFI:
        //!< Wakeup caused by WIFI (light sleep only)
        Serial.println("Wakeup caused by WIFI (light sleep only)");
        break;
    case ESP_SLEEP_WAKEUP_COCPU:
        //!< Wakeup caused by COCPU int
        Serial.println("Wakeup caused by COCPU int");
        break;
    case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG:
        //!< Wakeup caused by COCPU crash
        Serial.println("Wakeup caused by COCPU crash");
        break;
    case  ESP_SLEEP_WAKEUP_BT:
        //!< Wakeup caused by BT (light sleep only)
        Serial.println("Wakeup caused by BT (light sleep only)");
        break;
    default :
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;

    }
}
