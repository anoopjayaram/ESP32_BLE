#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5;       // Duration of each scan in seconds
int intervalTime = 10;  // Interval between scans in seconds
int rssiThreshold = -70; // RSSI threshold for proximity alert (adjust based on range requirements)
BLEScan* pBLEScan;
unsigned long lastScanTime = 0; // To keep track of the last scan time
bool deviceInRange = false;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.haveServiceUUID()) {
          BLEUUID serviceUUID = advertisedDevice.getServiceUUID();
         // Serial.print("Advertised Service UUID: ");
         // Serial.println(serviceUUID.toString().c_str());
      } else {
         // Serial.println("No Service UUID found in advertising packet");
      }


      // Check for specific UUID
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().toString() == "0000180d-0000-1000-8000-00805f9b34fb") {
          int rssi = advertisedDevice.getRSSI();
          Serial.printf("Tracked Device Found with RSSI: %d\n", rssi);

          if (rssi >= rssiThreshold) { 
              // Device is in range
              deviceInRange = true;
              digitalWrite(LED_BUILTIN, HIGH); // LED ON when in range
          
          } else {
              // Device is out of range
              deviceInRange = false;
              digitalWrite(LED_BUILTIN, LOW);  // LED OFF when out of range
              Serial.println("Warning: Device is going out of range!");
          }
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Starting BLE anti-lost tag...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // Create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Active scan for faster results
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // Must be less than or equal to setInterval
}

void loop() {
  unsigned long currentTime = millis() / 1000; // Current time in seconds

  // Check if enough time has passed since the last scan
  if (currentTime - lastScanTime >= intervalTime) {
    Serial.println("Starting BLE scan...");
    pBLEScan->start(scanTime, false);

    if (!deviceInRange) {
      Serial.println("Device out of range! Triggering alert...");
      digitalWrite(LED_BUILTIN, HIGH); // LED ON when in range
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW); // LED ON when in range
      delay(1000);
      // You could add a buzzer or other alert here
    }

    // Update last scan time
    lastScanTime = currentTime;
    
    // Reset deviceInRange for the next scan
    deviceInRange = false;
    
    // Clear results to free memory
    pBLEScan->clearResults();
  }

  delay(1000); // Check every second if it's time for the next scan
}
