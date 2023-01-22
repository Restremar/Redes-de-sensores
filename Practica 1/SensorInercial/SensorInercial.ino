#include <MPU9250_asukiaaa.h>
#include <Arduino.h>

MPU9250_asukiaaa mpu;
volatile float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection;
volatile uint16_t mX, mY, mZ;
String dataLabel1 = "accelX";
String dataLabel2 = "accelY";
String dataLabel3 = "accelZ";
String dataLabel4 = "accelSqrt";
String dataLabel5 = "gyroX";
String dataLabel6 = "gyroY";
String dataLabel7 = "gyroZ";
String dataLabel8 = "magX";
String dataLabel9 = "magY";
String dataLabel10 = "magZ";
String dataLabel11 = "horizontalDirection";

boolean label;

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

#define LED 5

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

  label = true;
}

void loop() {
  int result;

  result = mpu.accelUpdate();
  if (result == 0) {
    aX = mpu.accelX();
    aY = mpu.accelY();
    aZ = mpu.accelZ();
    aSqrt = mpu.accelSqrt();
  }

  result = mpu.gyroUpdate();
  if (result == 0) {
    gX = mpu.gyroX();
    gY = mpu.gyroY();
    gZ = mpu.gyroZ();
  }

  result = mpu.magUpdate();
  if (result == 0) {
    mX = mpu.magX();
    mY = mpu.magY();
    mZ = mpu.magZ();
    mDirection = mpu.magHorizDirection();
  }

  delay(100);

  enviarDatos();
}

void enviarDatos() {
  delay(800);

  while (label) {
    Serial.print(dataLabel1 + "," + dataLabel2 + ","
                 + dataLabel3 + "," + dataLabel4 + ","
                 + dataLabel5 + "," + dataLabel6 + ","
                 + dataLabel7 + "," + dataLabel8 + ","
                 + dataLabel9 + "," + dataLabel10 + "," + dataLabel11);

    label = false;
  }
  Serial.print(String(aX));
  Serial.print(",");
  Serial.print(String(aY));
  Serial.print(",");
  Serial.print(String(aZ));
  Serial.print(",");
  Serial.print(String(aSqrt));
  Serial.print(",");
  Serial.print(String(gX));
  Serial.print(",");
  Serial.print(String(gY));
  Serial.print(",");
  Serial.print(String(gZ));
  Serial.print(",");
  Serial.print(String(mX));
  Serial.print(",");
  Serial.print(String(mY));
  Serial.print(",");
  Serial.print(String(mZ));
  Serial.print(",");
  Serial.println(String(mDirection));

  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                // wait for 200 ms
  digitalWrite(LED, LOW);

}
