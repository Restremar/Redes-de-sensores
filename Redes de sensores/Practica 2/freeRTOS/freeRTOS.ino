#include <Arduino.h>
#include <MPU9250_asukiaaa.h>

MPU9250_asukiaaa mpu;
volatile float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection;
volatile uint16_t mX, mY, mZ;

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 22 //21
#define SCL_PIN 21 //22
#endif

#define LED 5

void Muestrear(void *parameter) {

  for (;;) {

    aX = mpu.accelX();
    aY = mpu.accelY();
    aZ = mpu.accelZ();
    aSqrt = mpu.accelSqrt();


    gX = mpu.gyroX();
    gY = mpu.gyroY();
    gZ = mpu.gyroZ();


    mX = mpu.magX();
    mY = mpu.magY();
    mZ = mpu.magZ();
    mDirection = mpu.magHorizDirection();

    vTaskDelay (100 / portTICK_PERIOD_MS); //Ejecuta esta tarea cada 100 milisegundos
  }

}

void Enviar(void *parameter) {

  for (;;) {
    mpu.accelUpdate();
    Serial.print("accelX: " + String(aX));
    Serial.print("\taccelY: " + String(aY));
    Serial.print("\taccelZ: " + String(aZ));
    Serial.print("\taccelSqrt: " + String(aSqrt));

    mpu.gyroUpdate();
    Serial.print("\tgyroX: " + String(gX));
    Serial.print("\tgyroY: " + String(gY));
    Serial.print("\tgyroZ: " + String(gZ));

    mpu.magUpdate();
    Serial.print("\tmagX: " + String(mX));
    Serial.print("\tmaxY: " + String(mY));
    Serial.print("\tmagZ: " + String(mZ));
    Serial.print("\thorizontalDirection: " + String(mDirection));

    Serial.println("");

    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                // wait for 200 ms
    digitalWrite(LED, LOW);

    vTaskDelay (1000 / portTICK_PERIOD_MS); //Ejecuta esta tarea cada segundo
  }

}

void setup() {

  Serial.begin(115200);
  while (!Serial);

  pinMode(5, OUTPUT);

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mpu.setWire(&Wire);
#endif

  mpu.beginAccel();
  mpu.beginGyro();
  mpu.beginMag();

  xTaskCreate(Muestrear, "Muestrear", 1024, NULL, 2, NULL);
  xTaskCreate(Enviar, "Enviar", 1024, NULL, 1, NULL);
}

void loop() {

}
