#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup()
{
  Serial.begin(115200);

  SerialBT.begin("ESP32test"); //Bluetooth device name that will appear
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop()
{
    if (Serial.available())
    {
      SerialBT.write(Serial.read()); //Write and send it from the serial with bluetooth
    }
    if (SerialBT.available())
    {
      Serial.write(SerialBT.read()); //Read from bluetooth, receive in the Serial
    }
    delay(20);
  
}
