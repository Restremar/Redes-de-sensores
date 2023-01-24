#include <Average.h>
#include <MPU9250_asukiaaa.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32_FTPClient.h>
#include <octocat.h>
#include <ArduinoJson.h>
#include <Arduino.h>

//Defino boton para inicio
#define BUTTON_PIN 36

//Variables will change:
int lastState = HIGH; // the previous state from the input pin
int currentState;     // the current reading from the input pin
int i;

//Definimos parametros para el wifi
const char* ssid       = "vodafone0058";
const char* password   = "YMYQDZ5AD3YZM2";

//Parametros para el servidor NTP donde conseguimos la hora exacta
const char* ntpServer = "pool.ntp.org"; //Servidor NTP
const long  gmtOffset_sec = 3600; //UTC +1.00 : 1 * 60 * 60 : 3600
const int   daylightOffset_sec = 3600; //Desplazamiento en segundos para el horario de verano

//Parametros para el servidor FTP
char ftp_server[] = "155.210.150.77";//Servidor FTP
char ftp_user[]   = "rsense";//Usuario a servidor FTP
char ftp_pass[]   = "rsense";//Contraseña a servidor FTP

//Variables necesarias para mandar datos al servidor
char hora[11];
String nombre = "";
String datos;

//Crear cliente FTP para subir archivos a servidor FTP
ESP32_FTPClient ftp (ftp_server, ftp_user, ftp_pass, 5000, 2);

// Definimos los pines del I2C
#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

//Variables para las aceleraciones de los 3 ejes y una de tipo sensor
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, media;

float meanSampleAcc, meanSampleGX, meanSampleGY, meanSampleGZ;

float SumAcc, SumGX, SumGY, SumGZ;

float stDevAcc, stDevGX, stDevGY, stDevGZ;

float DevSumAcc, DevSumGX, DevSumGY, DevSumGZ;

float vzAcc, vzGX, vzGY, vzGZ;

float Acc[250];
float AccX[250];
float AccY[250];
float AccZ[250];

int pos = 0;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

#ifdef _ESP32_HAL_I2C_H_
  Wire.begin(SDA_PIN, SCL_PIN); //Inicializar la comunicación en los puertos
  mySensor.setWire(&Wire); //Asociamos el sensor a I2C
#endif
  mySensor.beginGyro();//Arrancamos las medidas de aceleración y posición
  mySensor.beginAccel();

  xTaskCreate(Muestreo, "Muestreo", 10000, NULL, 1, NULL);
  xTaskCreate(Envio, "Envio", 10000, NULL, 1, NULL);

  //connect to WiFi
  Serial.printf("Conectando a %s ", ssid);
  Serial.println("");
  WiFi.begin(ssid, password); //Intento conectarme indicando el usuario y la contraseña
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) { //Mientras se está conectando
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConectado a la red"); //Conectado


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //Configuro la hora con los parametros definidos

  //Comenzar conexión con servidor FTP
  ftp.OpenConnection();
  ftp.ChangeWorkDir("/rsense/760245");//Cambiamos directorio en el servidor

}

void loop() {

  // put your main code here, to run repeatedly:
  currentState = digitalRead(BUTTON_PIN);

  if (lastState == LOW && currentState == HIGH) {
    Serial.println("The state changed from LOW to HIGH");

    memset(Acc, 0.0, sizeof(Acc));
    memset(AccX, 0.0, sizeof(AccX));
    memset(AccY, 0.0, sizeof(AccY));
    memset(AccZ, 0.0, sizeof(AccZ));

    pos = 0;

    delay(10000);

    Calculo();

    crearMensaje();

  }

  //save the last state

  lastState = currentState;

}

void Muestreo( void * parameter ) {
  for ( ;; ) {
    uint8_t sensorId;
    int result;
    result = mySensor.readId(&sensorId);//Compruebo que puedo leer del sensor
    if (result != 0) {
      Serial.println("Cannot read sensorId " + String(result));
    }
    result = mySensor.accelUpdate();
    if (result == 0) {
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();
      aSqrt = mySensor.accelSqrt();
    } else {
      Serial.println("Cannot read accel values " + String(result));
    }
  }
  vTaskDelete( NULL );
}


void Envio( void * parameter ) {
  for ( ;; ) {

//    Serial.print(aX);
//    Serial.print(",");
//    Serial.print(aY);
//    Serial.print(",");
//    Serial.print(aZ);
//    Serial.print(",");
//    Serial.print(aSqrt);
//    Serial.print(",");
//
//    float media = (aX + aY + aZ) / float(3);
//    Serial.println(media);

    if ( pos <= 249 ) {
      Acc[pos] = aSqrt;
      AccX[pos] = aX;
      AccY[pos] = aY;
      AccZ[pos] = aZ;

      pos++;
    }

    delay(40); //delay para pasar los datos muestreados cada 40ms
  }
  vTaskDelete( NULL );
}

//Funcion para conseguir el tiempo actual a partir del servidor NTP
void printLocalTime()
{
  struct tm timeinfo; //Estructura que contiene los detalles sobre el tiempo (minutos, segundos, horas...)
  if (!getLocalTime(&timeinfo)) { //Obtengo datos de fecha y hora y los guardo en timeinfo
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(hora, 11, "%H:%M:%S", &timeinfo);
}

//Funcion que calcula la desviacion tipica, la varianza y la media de la
//aceleracion sqrt y el giroscopio x,y,z
void Calculo() {
  for (int i = 0; i < 250; i++) {
    SumAcc += Acc[i];
    SumGX += AccX[i];
    SumGY += AccY[i];
    SumGZ += AccZ[i];
  }

  //Obtenemos la media de cada parametro
  meanSampleAcc = SumAcc / float(250);
  meanSampleGX = SumGX / float(250);
  meanSampleGY = SumGY / float(250);
  meanSampleGZ = SumGZ / float(250);

  //Se calcula la desviacion tipica de cada parametro
  for (int i = 0; i < 250; i++) {
    DevSumAcc += pow((meanSampleAcc - float(Acc[i])), 2);
    DevSumGX += pow((meanSampleGX - float(AccX[i])), 2);
    DevSumGY += pow((meanSampleGY - float(AccY[i])), 2);
    DevSumGZ += pow((meanSampleGZ - float(AccZ[i])), 2);
  }

  stDevAcc = sqrt(DevSumAcc / float(250));
  stDevGX = sqrt(DevSumGX / float(250));
  stDevGY = sqrt(DevSumGY / float(250));
  stDevGZ = sqrt(DevSumGZ / float(250));

  //Se calcula la varianza a partir de la desviación tipica
  vzAcc = stDevAcc * stDevAcc;
  vzGX = stDevGX * stDevGX;
  vzGY = stDevGY * stDevGY;
  vzGZ = stDevGZ * stDevGZ;

    Serial.println("Media: ");
    Serial.println(meanSampleAcc);
    Serial.println(meanSampleGX);
    Serial.println(meanSampleGY);
    Serial.println(meanSampleGZ);
  
    Serial.println("Desviacion tipica: ");
    Serial.println(stDevAcc);
    Serial.println(stDevGX);
    Serial.println(stDevGY);
    Serial.println(stDevGZ);
  
    Serial.println("Varianza: ");
    Serial.println(vzAcc);
    Serial.println(vzGX);
    Serial.println(vzGY);
    Serial.println(vzGZ);

  if ((meanSampleAcc > 1.2 && meanSampleAcc < 1.4) && (meanSampleGX < 0.1 && meanSampleGX > -0.17) && (meanSampleGY > 0.35 && meanSampleGY < 0.7) && (meanSampleGZ > 1.0 && meanSampleGZ < 1.2)
      && (stDevAcc > 0.2 && stDevAcc < 0.35) && (stDevGX > 0.1 && stDevGX < 0.35) && (stDevGY > 0.05 && stDevGY < 0.35) && (stDevGZ > 0.25 && stDevGZ < 0.5) &&
      (vzAcc > 0.01 && vzAcc < 0.1) && (vzGX > 0.01 && vzGX < 0.1) && (vzGY > 0.0 && vzGY < 0.06) && (vzGZ > 0.05 && vzGZ < 0.25)) {

    Serial.println("Se ha reconocido el movimiento de hombro lateral");

  } else if ((meanSampleAcc > 1.4 && meanSampleAcc < 1.6) && (meanSampleGX > -0.6 && meanSampleGX < -0.3) && (meanSampleGY > 1.3 && meanSampleGY < 1.55) && (meanSampleGZ > 0.0 && meanSampleGZ < 0.25)
             && (stDevAcc > 0.05 && stDevAcc < 0.3) && (stDevGX > 0.0 && stDevGX < 0.2) && (stDevGY > 0.1 && stDevGY < 0.4) && (stDevGZ > 0.05 && stDevGZ < 0.15) &&
             (vzAcc > 0.01 && vzAcc < 0.1) && (vzGX > 0.0 && vzGX < 0.04) && (vzGY > 0.0 && vzGY < 0.1) && (vzGZ > 0.0 && vzGZ < 0.05)) {

    Serial.println("Se ha reconocido el movimiento de press militar");

  } else {

    Serial.println("No se ha reconocido ningun movimiento");

  }

  vzAcc = 0;
  vzGX = 0;
  vzGY = 0;
  vzGZ = 0;
  stDevAcc = 0;
  stDevGX = 0;
  stDevGY = 0;
  stDevGZ = 0;
  meanSampleAcc = 0;
  meanSampleGX = 0;
  meanSampleGY = 0;
  meanSampleGZ = 0;
  DevSumAcc = 0;
  DevSumGX = 0;
  DevSumGY = 0;
  DevSumGZ = 0;
  SumAcc = 0;
  SumGX = 0;
  SumGY = 0;
  SumGZ = 0;
}

//Funcion que crea el String de datos para mandarlo al Json
//y subirlo al servidor ftp
void crearMensaje() {

  datos.concat(meanSampleAcc);
  datos.concat(" | ");
  datos.concat(meanSampleGX);
  datos.concat(" | ");
  datos.concat(meanSampleGY);
  datos.concat(" | ");
  datos.concat(meanSampleGZ);
  datos.concat(" | ");
  datos.concat(stDevAcc);
  datos.concat(" | ");
  datos.concat(stDevGX);
  datos.concat(" | ");
  datos.concat(stDevGY);
  datos.concat(" | ");
  datos.concat(stDevGZ);
  datos.concat(" | ");
  datos.concat(vzAcc);
  datos.concat(" | ");
  datos.concat(vzGX);
  datos.concat(" | ");
  datos.concat(vzGY);
  datos.concat(" | ");
  datos.concat(vzGZ);
  datos.concat("\n");

  DynamicJsonDocument doc(82000);//Creo archivo json

  printLocalTime();//Consigo hora local

  int len = datos.length() + 1;
  char datos2[len];
  char* cabecera = "ACELERACION:\n";

  //Nombre para archivo a subir al servidor FTP
  nombre = "datosAcc_";
  nombre.concat(hora);
  nombre.concat(".json");

  //ftp.Write solo acepta array de char no String
  datos.toCharArray(datos2, len);

  //Creamos archivo y subimos
  const char *path = nombre.c_str();
  ftp.InitFile("Type A");//Inicializamos tipo de archivo
  ftp.NewFile(path);//Creamos archivo
  ftp.Write(cabecera);
  ftp.Write(datos2);//Escribimos
  ftp.CloseFile();//Cerramos

  datos.clear();//Limpiamos el String de datos
}
