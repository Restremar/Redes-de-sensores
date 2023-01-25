#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" //UUID para el servicio
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" //UUID para la característica

BLECharacteristic *pCharacteristic;

int contador = 0;
char datos[3];

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  
  // Create the BLE Device
  BLEDevice::init("ESP32");

  //Set the BLE device as a server.
  BLEServer *pServer = BLEDevice::createServer();

  //Create a service for the BLE server with the UUID defined earlier.
  BLEService *pService = pServer->createService(SERVICE_UUID);

  //set the characteristic for that service. 
  //You also use the UUID defined earlier, and you need to pass 
  //as arguments the characteristic’s properties. In this case, it’s: READ and WRITE.
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  
}

void loop() {
  contador = contador +1;
  String(contador).toCharArray(datos,3);
  pCharacteristic->setValue(contador);
  pCharacteristic->notify();
  delay(1000); //delay de 1s
}
