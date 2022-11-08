String teclado;
const int poten = 2;
volatile int valor = 0;
int segundos;
int duty;
volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
  valor = analogRead(poten);
}

void ADC(int dato){
  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;
    
    if(totalInterruptCounter == dato){
      Serial.print("Potenciometro: ");
      Serial.println(valor);
      totalInterruptCounter = 0;
    }
  }
}

void PWM(int dato){

  switch (dato){
    case 0:
      analogWrite(5, 0);
      delay(1000);
      break;
    case 1:
      analogWrite(5, 25);
      delay(1000);
      break;
    case 2:
      analogWrite(5, 51);
      delay(1000);
      break;
    case 3:
      analogWrite(5, 76);
      delay(1000);
      break;
    case 4:
      analogWrite(5, 102);
      delay(1000);
      break;
    case 5:
      analogWrite(5, 127);
      delay(1000);
      break;
    case 6:
      analogWrite(5, 153);
      delay(1000);
      break;
    case 7:
      analogWrite(5, 178);
      delay(1000);
      break;
    case 8:
      analogWrite(5, 204);
      delay(1000);
      break;
    case 9:
      analogWrite(5, 229);
      delay(1000);
      break;
  }
  
}

void setup() {
  Serial.begin(115200);
  
  pinMode(2,INPUT);
  analogReadResolution(12);
  
  delay(1000);
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void loop() {
  
  if(Serial.available()>0){
    teclado = Serial.readString();
    Serial.println(teclado);
    delay(5);
  
      if(teclado.indexOf("ADC\n")>0){
      
      valor = analogRead(poten);
      Serial.print("Potenciometro: ");
      Serial.println(valor);

      delay(1000);
      
      }else if(teclado.indexOf("ADC(%u)")>0){
  
        segundos = teclado[5] - '0';
        ADC(segundos);
        
      }else if(teclado.indexOf("PWM(%u)")>0){
  
        duty = teclado[5] - '0';
        PWM(duty); 
        
      }else if(teclado.indexOf("ADC(0)")>0){
        
        Serial.println("Nothing to read");
        
      }else{
        Serial.println("Error");
      }

  }
  
}
  
