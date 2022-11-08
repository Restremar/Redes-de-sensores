#include <MPU9250_asukiaaa.h>

MPU9250_asukiaaa mpu;
volatile float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection;
volatile uint16_t mX, mY, mZ;
volatile int interruptCounter;
int totalInterruptCounter;
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

boolean label = true;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

#define LED 5

void IRAM_ATTR onTimer() {

  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);

  mpu.accelUpdate();
  aX = mpu.accelX();
  aY = mpu.accelY();
  aZ = mpu.accelZ();
  aSqrt = mpu.accelSqrt();

  mpu.gyroUpdate();
  gX = mpu.gyroX();
  gY = mpu.gyroY();
  gZ = mpu.gyroZ();

  mpu.magUpdate();
  mX = mpu.magX();
  mY = mpu.magY();
  mZ = mpu.magZ();
  mDirection = mpu.magHorizDirection();

}

void setup() {

  Serial.begin(115200);
  while (!Serial);

  pinMode(5, OUTPUT);
  
  timer = timerBegin(0, 80, true);  //Prescaler a 80 para que el contador se incremente cada microsegundo
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true); //Valor de 10000 para que haya interrupcion cada 100ms
  timerAlarmEnable(timer);

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mpu.setWire(&Wire);
#endif

  mpu.beginAccel();
  mpu.beginGyro();
  mpu.beginMag();

}

void loop() {

  if (interruptCounter > 0) {

    //    while(label){
    //      Serial.print(dataLabel1);
    //      Serial.print(",");
    //      Serial.print(dataLabel2);
    //      Serial.print(",");
    //      Serial.print(dataLabel3);
    //      Serial.print(",");
    //      Serial.print(dataLabel4);
    //      Serial.print(",");
    //      Serial.print(dataLabel5);
    //      Serial.print(",");
    //      Serial.print(dataLabel6);
    //      Serial.print(",");
    //      Serial.print(dataLabel7);
    //      Serial.print(",");
    //      Serial.print(dataLabel8);
    //      Serial.print(",");
    //      Serial.print(dataLabel9);
    //      Serial.print(",");
    //      Serial.print(dataLabel10);
    //      Serial.print(",");
    //      Serial.println(dataLabel11);
    //
    //      label = false;
    //    }

    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;

    if (totalInterruptCounter == 10) {

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

      totalInterruptCounter = 0;

    }

  }

}
