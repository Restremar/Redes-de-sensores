#include <HardwareSerial.h>

// Potenciometro conectado a GPIO 34 (Analog ADC1_CH6)
const int potPin = 34;

//Pin del LED para comprobar el correcto funcionamiento del PWM
const int LED = 5;

//Contador para la interrupción. Se define volatile para evitar la eliminación por parte del compilador
//al compartirse por dos ciclos
volatile int counter;

// variable para almacenar valor de potenciometro
float potVal = 0;
float potValor = 0;

//variable para almacenar el retraso para mostrar el valor del ADC
int retraso = 0;
int i = 0;

//Variable para almacenar dato de UART
String dato;
String datov;
boolean TransmisionCompleta = false;
char answer[10];

//variable para almacenar el duty del PWM
int duty = 0;

//Propiedades del PWM
const int frecuencia = 5000;
const int canal = 0; //Canales disponibles del 0 al 15
const int resolucion = 8; //de 8 bits de 0 a 255

//Para configurar el temporizador necesario un puntero a este tipo de variable
hw_timer_t * tempo = NULL;

//Para la sincronización entre el bucle principal y el de ISR(interrupt service routine)
portMUX_TYPE tempoMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR ftemp() {
  // debe tener el atributo IRAM_ATTR para que el compilador coloque el código en la RAM interna
  //Como variable counter compartida con el bucle principal el aumento se hará dentro de una sección crítica
  portENTER_CRITICAL_ISR(&tempoMux);
  counter++; //Indica al bucle principal que ha ocurrido una interrupción
  portEXIT_CRITICAL_ISR(&tempoMux);
}

void setup() {
  Serial.begin(115200); //Inicializamos la UART
  analogSetPinAttenuation(potPin, ADC_11db); // Es la atenuación correspondiente para una entrada de 3.3V

  //Correspondiente a la interrupción
  tempo = timerBegin(0, 80, true); //inicialización del temporizador.
  //Primero se mete el número del temporizador a utilizar
  //el valor del preescalado. La frecuencia de ESP32 es 80 MHz si ponemos preescaler de 80
  //el contador del temporizador incrementará 1 000 000 veces por segundo. Inviertiendo
  //se sabe que el contador se incrementará a cada microsegundo.
  //el flag que indica si contar hacia arriba o abajo, true hacia arriba y false hacia abajo
  timerAttachInterrupt(tempo, &ftemp, true);//vinculación del temporizador a una función de manejo
  //primero se mete la varibale global
  //la dirección de la función de manejo
  //true(interrución edge triggered) o false (interrupción de level triggered)
  //Edge triggered: El circuito se activa en el flanco negativo o positivo de la señal del reloj.
  //Level triggered: El circuito se activará cuando el impulso de reloj o activación esté en un nivel particular.


  //Correspondiente al PWM
  ledcSetup(canal, frecuencia, resolucion); //Se configura la funcionalidad PWM
  ledcAttachPin(LED, canal); //Asociamos el canal al puerto del LED
}

void serialEvent() {
  while (Serial.available()) {
    char CharEntrada = Serial.read(); //Leer un byte del puerto serial
    dato += CharEntrada;  //Agregar el char anterior al string
    if (CharEntrada == '\n') {  //Si se detecta un fin de linea
      dato.trim(); //Quito saltos de linea al final
      datov = dato; //Me guardo el dato actual en otra variable para solo cambiar de ejecución si cambia dato
      dato = "";// Reseteo dato para poder volver a rellenarlo
      TransmisionCompleta = true;  //Se indica al programa que el usuario termino de ingresar la informacion
    }
  }
}


void loop() {
  do {
    Serial.print("Answer: ");
    leerTeclado(answer);
  } while (validAnswer(answer) != true);

  if (answer == "ADC") {
    
    potVal = analogRead(potPin); //Leo el puerto analógico del potenciómetro
    potValor = potVal * (3.3 / 4095); // Conversión referente al ADC (4095 porque cogemos todo el rango al tener la attenuación de 11dB)
    Serial.print("El valor leido del ADC es: ");
    Serial.println(potValor);//Muestro el valor por el monitor serie
    TransmisionCompleta = false;  //Limpiar la bandera
    
  } else if (answer == "ADC(%i)") {
    
    char* dato = strstr(answer, "(");
    dato = strtok(dato, ")");
    retraso = atoi(dato);
    
    if (retraso != 0) {
      timerAlarmWrite(tempo, retraso * 1000000, true); //Para indicar la frecuencia de interrupciones

      timerAlarmEnable(tempo);//habilitamos el temporizador pasando la variable como entrada
      if (counter > 0) {
        //Como variable counter compartida con el ISR el decrementar se hará dentro de una sección crítica
        portENTER_CRITICAL(&tempoMux);
        counter--; //Decrementamos para indicar que la interrupción ha sido reconocida y se va a manejar
        portEXIT_CRITICAL(&tempoMux);

        //Acciones a realizar cada vez que ocurra la interrupción
        potVal = analogRead(potPin); //Leo el puerto analógico del potenciómetro
        potValor = potVal * (3.3 / 4095); // Conversión referente al ADC (4095 porque cogemos todo el rango al tener la attenuación de 11dB)
        Serial.print("El valor leido del ADC es: ");
        Serial.println(potValor);//Muestro el valor por el monitor serie
        
      }
      
    }
    
  } else if (answer == "PWM(%i)") {
    
    char* dato = strstr(answer, "(");
    dato = strtok(dato, ")");
    duty = atoi(dato);
    duty = map(duty, 0, 9, 255, 0);
    ledcWrite(canal, duty);
    
  }
  
}


void leerTeclado(char* dato)
{
  while (!Serial.available());

  while (Serial.available() > 0)
  {
    dato[i] = Serial.read();

    if ( (dato[i] == '\r') && (dato[i] == '\n') )
    {
      dato[i] = '\0';
    }
    else
    {
      i++;
    }
  }
}


boolean validAnswer(char* dato)
{
  if ((strcmp(dato, "ADC")) || (strcmp(dato, "adc")) || (strcmp(dato, "ADC(%i)")) || (strcmp(dato, "adc(%i)")) || (strcmp(dato, "PWM(%i)")) || (strcmp(dato, "pwm(%i)")))
  {
    Serial.println(dato);
    return true;
  }
  else
  {
    Serial.println("Not valid answer");
    return false;
  }
}
