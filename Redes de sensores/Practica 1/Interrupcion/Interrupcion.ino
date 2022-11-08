const int poten = 2;
volatile int valor;
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

void setup() {

  Serial.begin(115200);

  pinMode(2,INPUT);
  analogReadResolution(12);
 
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void loop() {

  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;
    
    if(totalInterruptCounter == 10){
      Serial.print("Potenciometro: ");
      Serial.println(valor);
      totalInterruptCounter = 0;
    }
  }
  
}
