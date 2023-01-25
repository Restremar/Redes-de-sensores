#include "sys/time.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"



#define GPIO_DEEP_SLEEP_DURATION     10  // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

BLEAdvertising *pAdvertising;   // BLE Advertisement type

struct timeval now; //The timeval is defined as structure to access the current time.

#define BEACON_UUID "87b99b2c-raul-11e9-bc42-526af7764f64" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)

//Now create a function which will contain iBeacon attributes such as UUID, Major, Minor, Etc. 
//In this function create an instance for BLE as iBeacon, and set a fake manufacturer ID, UUID, major and minor for ESP32 as iBeacon.
void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor((bootcount & 0xFFFF0000) >> 16);
  oBeacon.setMinor(bootcount & 0xFFFF);
  
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

  oAdvertisementData.setFlags(0x04); // Set the flag as 0x04 so that it will output BrEdrNotSupported in the scanner.

  std::string strServiceData = "Redes de sensores"; //Set advertising data for publishing.

  //Add the string one after other to advertise.
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);

  //Start the advertising by publishing data.
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);

}


void setup() {

  Serial.begin(115200);
  gettimeofday(&now, NULL);
  Serial.printf("start ESP32 %d\n", bootcount++);
  Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n", now.tv_sec, now.tv_sec - last);

  //Store the current time in the static memory.
  last = now.tv_sec;

  // Create the BLE Device and name it
  BLEDevice::init("ESP32 as iBeacon");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();

  //Set the ESP32 in iBeacon mode.
  setBeacon();

  // Start advertising
  pAdvertising->start();

  Serial.println("Advertizing started...");

  delay(100);

  pAdvertising->stop();

  Serial.printf("enter deep sleep\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
  Serial.printf("in deep sleep\n");

}


void loop() {

}
