#include <MPU9250_asukiaaa.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32_FTPClient.h>
#include <octocat.h>
#include <ArduinoJson.h>
#include <Arduino.h>

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

//Parametros para el servidor TCP
const IPAddress serverIP(192, 168, 0, 22); // La dirección que desea visitar
uint16_t serverPort = 455;         // Número de puerto del servidor

WiFiClient client;

//Variables necesarias para mandar datos al servidor
char hora[11];
String nombre = "";
int contador = 0;
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
float aX, aY, aZ, aSqrt;

char sSqrt[10];

void printLocalTime()
{
  struct tm timeinfo; //Estructura que contiene los detalles sobre el tiempo (minutos, segundos, horas...)
  if (!getLocalTime(&timeinfo)) { //Obtengo datos de fecha y hora y los guardo en timeinfo
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(hora, 11, "%H:%M:%S", &timeinfo);
}

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

  //connect to WiFi
  //  Serial.printf("Conectando a %s ", ssid);
  //  Serial.println("");
  WiFi.begin(ssid, password); //Intento conectarme indicando el usuario y la contraseña
  //Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) { //Mientras se está conectando
    delay(1000);
    //Serial.print(".");
  }

  //Serial.println("\nConectado a la red"); //Conectado


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //Configuro la hora con los parametros definidos

  //Comenzar conexión con servidor FTP
  ftp.OpenConnection();
  ftp.ChangeWorkDir("/rsense/760245");//Cambiamos directorio en el servidor

}

void loop() {

  client.connect(serverIP, serverPort);

  DynamicJsonDocument doc(82000);//Creo archivo json

  printLocalTime();//Consigo hora local

  int len = datos.length() + 1;
  char datos2[len];
  char* cabecera = "ACELERACION PROMEDIO:\n";

  //Nombre para archivo a subir al servidor FTP
  nombre = "datosAcc_";
  nombre.concat(hora);
  nombre.concat(".json");
  //  Serial.println("");
  //  Serial.println(nombre);

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
  delay(5000);

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
      //Leo la aceleracion media
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();
      aSqrt = mySensor.accelSqrt();
    } else {
      Serial.println("Cannot read accel values " + String(result));
    }
  }
  vTaskDelete( NULL ); //No se puede salir de un Task mediante un return, sino hay que eliminar el Task.
}

void Envio( void * parameter ) {
  for ( ;; ) {

    //    Serial.println(aX);
    //    Serial.println(aY);
    //    Serial.println(aZ);
    Serial.println(aSqrt);
    
    if(client.connected()){
    sprintf(sSqrt, "%f", aSqrt);
    client.write(sSqrt);
    }
    
    datos.concat(aSqrt);//Concatenamos los datos en un array para almacenar varios
    datos.concat("\n"); //y subirlos al servidor ftp en un fichero json

    delay(40); //delay para pasar los datos muestreados cada 40ms
  }
  vTaskDelete( NULL ); //No se puede salir de un Task mediante un return, sino hay que eliminar el Task.
}
