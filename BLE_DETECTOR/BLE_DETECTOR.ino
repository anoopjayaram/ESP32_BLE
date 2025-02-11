#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define LED_PIN 2  // Set the GPIO pin for LED (e.g., GPIO 2)

String targetMAC = "8c:ce:fd:a7:07:40"; // The MAC address to scan for

// Define the callback class to handle BLE results
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    String deviceMac = advertisedDevice.getAddress().toString();
      Serial.println(deviceMac);

    // If the device MAC address matches the target, check the RSSI
    if (deviceMac == targetMAC) {
      Serial.print("Found device with MAC: ");
      Serial.println(deviceMac);
      
      int rssi = advertisedDevice.getRSSI();
      Serial.print("RSSI: ");
      Serial.println(rssi);

      // Adjust LED blinking rate based on RSSI
      if (rssi > -60) {
        // Strong signal: Fast blinking
        Serial.println("RSSI is stronger than -60, Fast Blink");
        blinkLED(100);  // Fast blink with 100ms delay
      } else if (rssi > -70) {
        // Medium signal: Moderate blinking
        Serial.println("RSSI is between -60 and -70, Moderate Blink");
        blinkLED(300);  // Moderate blink with 300ms delay
      } else {
        // Weak signal: Slow blinking
        Serial.println("RSSI is weaker than -70, Slow Blink");
        blinkLED(600);  // Slow blink with 600ms delay
      }
    } else {
      // If the target device is not found, ensure the LED is off
      digitalWrite(LED_PIN, LOW);
      Serial.println("Device not found, LED OFF");
    }
  }

  // Function to blink LED with specified delay
  void blinkLED(int delayTime) {
    digitalWrite(LED_PIN, HIGH);  // Turn on the LED
    delay(delayTime);              // Wait for the specified time
    digitalWrite(LED_PIN, LOW);   // Turn off the LED
    delay(delayTime);              // Wait for the specified time
  }
};

BLEScan* pBLEScan;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT); // Set the LED pin as an output
  
  BLEDevice::init("");  // Initialize BLE
  pBLEScan = BLEDevice::getScan(); // Create a BLE scan object
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  
  // Start scanning for 5 seconds initially
  pBLEScan->start(5, false);  // Scan for 5 seconds
}

void loop() {
  pBLEScan->start(1, false);  // Continue scanning for 1 second per iteration
}
