#include <WiFi.h>
#include "time.h"

const char* ssid       = "vodafone0058";
const char* password   = "YMYQDZ5AD3YZM2";

char date[50];

const IPAddress serverIP(192, 168, 0, 22); // La dirección que desea visitar
uint16_t serverPort = 455;         // Número de puerto del servidor

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 0;

String control; //Para implementar la capa de control
String control2;

WiFiClient client;

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  strftime(date, 42, "%A, %B %d %Y %H:%M:%S\n", &timeinfo);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

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

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  client.connect(serverIP, serverPort);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (client.available()) { //Compruebo que el cliente tiene datos para leer
    char dato = client.read(); //Leo char a char
    control += dato; //Concateno char en un String
  }
  control.toUpperCase(); //Paso todo a mayusculas
  if (control.equals("START")) { //Compruebo que ha mandado un START
    while (client.connected()) { //Mientras el cliente esté conectado
      if (client.available()) { //Compruebo que el cliente tiene datos para leer
        char dato = client.read();//Leo char a char
        control2 += dato;//Concateno char en un String
      }
      control2.toUpperCase(); //Paso a mayusculas
      if (control2.equals("STOP")) { //Si datos = STOP paro el programa
        return;
      }
      printLocalTime();//Con un delay de un segundo printeo la hora local
      client.write(date);
      delay(1000);
    }
  }
}
