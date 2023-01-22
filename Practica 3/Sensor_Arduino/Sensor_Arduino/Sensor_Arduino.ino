#include <MPU9250_asukiaaa.h>
#include <Wire.h>

// Definimos los pines del I2C
#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

//Variables para las aceleraciones de los 3 ejes y una de tipo sensor
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
#ifdef _ESP32_HAL_I2C_H_
  Wire.begin(SDA_PIN, SCL_PIN); //Inicializar la comunicación en los puertos
  mySensor.setWire(&Wire); //Asociamos el sensor a I2C
#endif
  mySensor.beginAccel();//Arrancamos las medidas de aceleración
  
  xTaskCreate(Muestreo, "Muestreo", 10000, NULL, 1, NULL);
  xTaskCreate(Envio, "Envio", 10000, NULL, 1, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void Muestreo( void * parameter ){
  for( ;; ){                  //Bucle infinito
    uint8_t sensorId;
    int result;
    result = mySensor.readId(&sensorId);//Compruebo que puedo leer del sensor
    if (result != 0) {   
      Serial.println("Cannot read sensorId " + String(result));
    }
    result = mySensor.accelUpdate();
    if (result == 0) {
      //Leo las aceleraciones de los 3 ejes del sensor y la media
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();
      aSqrt = mySensor.accelSqrt();
        } else {
      Serial.println("Cannod read accel values " + String(result));
    }
  }
  vTaskDelete( NULL ); //No se puede salir de un Task mediante un return, sino hay que eliminar el Task.
}



void Envio( void * parameter ){
  for( ;; ){                  //Bucle infinito
    //Mando por UART cada segundo los valores
    Serial.print(aX);
    Serial.print(",");
    Serial.print(aY);
    Serial.print(",");
    Serial.print(aZ);
    Serial.print(",");
    Serial.print(aSqrt);
    Serial.print(",");

    mean = (aX+aY+aZ)/3;
    Serial.println(mean);

    delay(40);
  }
  vTaskDelete( NULL ); //No se puede salir de un Task mediante un return, sino hay que eliminar el Task.
}
