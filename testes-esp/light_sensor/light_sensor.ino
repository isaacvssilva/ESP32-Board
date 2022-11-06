
#define PIN_LIGHT_SENSOR  2  // ESP32 pin GPIO2 (ADC2_2)
#define LED_PIN           23  // ESP32 pin GPIO23

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT); //Setando como saida
  pinMode(PIN_LIGHT_SENSOR, INPUT); //Setando como entrada
}

void loop() {
  int analogValue = analogRead(PIN_LIGHT_SENSOR); // Leitura analogica
  if(analogValue < 600){
    digitalWrite(LED_PIN, HIGH);
  }else{
    digitalWrite(LED_PIN, LOW);
  }
  Serial.print(analogValue); 
  Serial.print("\n"); 
  delay(1000);
}
