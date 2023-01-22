
const int poten = 2;
int valor = 0;


void setup() {
  Serial.begin(115200);
  pinMode(2,INPUT);
  analogReadResolution(12);
  delay(1000);
}

void loop() {
  valor = analogRead(poten);
//  Serial.print("Potenciometro: ");
  Serial.println(valor);

  delay(1000);
}
