char *teclado;
char *aux;
char *segundos;
const int poten = 2;
volatile int valor = 0;
int duty;
uint64_t secTimer;

hw_timer_t * timer = NULL;

void IRAM_ATTR onTimer() {

  valor = analogRead(poten);
  Serial.println("Potenciometro: " + valor);
  
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

uint64_t convertStringToU64(String str)  
{

  uint64_t val = 0;
  for (int i = 0; i < str.length(); i++)
  {
    val = val * 16;
    val = val + str[i] - '0'; 
  }
  return val;
}

void setup() {
  Serial.begin(115200);
  
  pinMode(2,INPUT);
  analogReadResolution(12);
  
  delay(1000);
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);

}

void loop() {

  if(Serial.available()>0){
    teclado = (char*)Serial.read();
    delay(5);

    if(strcmp(teclado,"ADC")){
      
      valor = analogRead(poten);
      Serial.println("Potenciometro: " + valor);

      delay(1000);
      
    }else if(strcmp(teclado,"ADC(%u)")){
      
      aux = strstr(teclado, "(");
      segundos = strtok(aux, ")");
      strcat(segundos, "000000");
      secTimer = convertStringToU64(segundos);
      timerAlarmWrite(timer, secTimer, true);
      timerAlarmEnable(timer);
          
    }else if(strcmp(teclado,"PWM(%u)")){

      duty = *(teclado + 5) - '0';
      PWM(duty); 
      
    }else if(strcmp(teclado,"ADC(0)")){
      timerAlarmDisable(timer);
    }
       
  }
  
}
