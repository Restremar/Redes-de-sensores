#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32_FTPClient.h>
#include <octocat.h>
#include <ArduinoJson.h>

const char* ssid       = "vodafone0058";
const char* password   = "YMYQDZ5AD3YZM2";

const char* ntpServer = "pool.ntp.org"; //Servidor NTP
const long  gmtOffset_sec = 3600; //UTC +1.00 : 1 * 60 * 60 : 3600
const int   daylightOffset_sec = 3600; //Desplazamiento en segundos para el horario de verano

char ftp_server[] = "155.210.150.77";//Servidor FTP
char ftp_user[]   = "rsense";//Usuario a servidor FTP
char ftp_pass[]   = "rsense";//Contraseña a servidor FTP

//Variables necesarias para mandar datos al servidor
char hora[11];

//Crear cliente FTP para subir archivos a servidor FTP
ESP32_FTPClient ftp (ftp_server, ftp_user, ftp_pass, 5000, 2);

long temp ;
String nombre = "";

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
  Serial.begin(115200);

  //connect to WiFi
  Serial.printf("Conectando a %s ", ssid);
  Serial.println("");
  WiFi.begin(ssid, password); //Intento conectarme indicando el usuario y la contraseña
  while (WiFi.status() != WL_CONNECTED) { //Mientras se está conectando
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a la red"); //Conectado


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //Configuro la hora con los parametros definidos

  //Comenzar conexión con servidor FTP
  ftp.OpenConnection();
  ftp.ChangeWorkDir("/rsense/760245");//Cambiamos directorio en el servidor
}

void loop() {

  printLocalTime();//Consigo hora local
  temp = random(10, 40); //Genero temperatura de manera aleatoria
  DynamicJsonDocument doc(1024);//Creo archivo json
  doc["Temperatura"] = temp;//Campo de temperatura
  doc["Unidades"] = "ºC";//Campo de unidad de temperatura
  doc["Unidad temporal"] = hora;//Campo de hora local

  String datos;
  char datos2[250];
  //Serializamos los datos a un String
  serializeJson(doc, datos);
  Serial.println(datos);

  //Nombre para archivo a subir al servidor FTP
  nombre = "grupo90_";
  nombre.concat(hora);
  nombre.concat(".json");
  Serial.println("");
  Serial.println(nombre);

  //ftp.Write solo acepta array de char no String
  datos.toCharArray(datos2, 250);

  //Creamos archivo y subimos
  const char *path = nombre.c_str();
  ftp.InitFile("Type A");//Inicializamos tipo de archivo
  ftp.NewFile(path);//Creamos archivo
  ftp.Write(datos2);//Escribimos
  ftp.CloseFile();//Cerramos
  delay(10000);

}
