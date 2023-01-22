// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char* ntpServer = "pool.ntp.org"; //Servidor NTP
const long  gmtOffset_sec = 3600; //UTC +1.00 : 1 * 60 * 60 : 3600
const int   daylightOffset_sec = 3600; //Desplazamiento en segundos para el horario de verano

const char* ssid       = "vodafone0058";
const char* password   = "YMYQDZ5AD3YZM2";

//Para escribir la hora en el servidor
String hora;
char hour[10];

// Objeto AsyncWebServer para crear el servidor
AsyncWebServer server(80);

void printLocalTime()
{
  struct tm timeinfo; //Estructura que contiene los detalles sobre el tiempo (minutos, segundos, horas...)
  if(!getLocalTime(&timeinfo)){ //Obtengo datos de fecha y hora y los guardo en timeinfo
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(hour,10,"%H:%M",&timeinfo);
} 

// Reemplaza el marcador del valor de la hora por la actual 
String processor(const String& var){
  Serial.println(var);
  if(var == "HOUR"){
    printLocalTime(); //Obtenemos la hora actual del servidor ntp
    hora = String(hour); 
    Serial.print(hora);
    return hora; //Reemplazamos por este valor
  }
  return String();
}

// Reemplaza el marcador del valor de la hora por 00:00 
String processor2(const String& var){
  if(var == "HOUR"){ //Comprueba que eciste el marcador de posicion
    hora = "00:00";
    Serial.print(hora);
    return hora; //Reemplazamos por este valor
  }
  return String();
}

void setup() {
  // Serial port
  Serial.begin(115200);

  // Inicializo SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // Desactiva la suspensión de wifi en modo STA para mejorar la velocidad de respuesta
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //Configuro la hora con los parametros definidos

  // Ruta para la pagina web
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Ruta para cargar el archivo style.css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  
  // Ruta para resetear la hora
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor2);
  });
  
  // Ruta para actualizar la hora
  server.on("/actual", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Arrancamos servidor para que escuche a los clientes
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

}
