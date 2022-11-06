#define TRIGGER_PIN  5 // ESP32 pin GPIO5
#define ECHO_PIN 18 // ESP32 pin GPIO18
#define SOUND_SPEED 0.034 // calculo da distância para o obstáculo, valor em cm/uS.
#define CM_TO_INCH 0.393701
#define LED_PIN 23  // ESP32 pin GPIO23
#define BUZZER_PIN 22 // ESP32 pin GPIO22

long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); //Setando como saida
  pinMode(BUZZER_PIN, OUTPUT); //Setando como saida
  pinMode(TRIGGER_PIN, OUTPUT);//Setando como saida
  pinMode(ECHO_PIN, INPUT);//Setando como entrada
}

void loop() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  // Coloca o trigPin no estado HIGH por 10 micro segundos
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // retorna o tempo de viagem da onda sonora em microssegundos
  duration = pulseIn(ECHO_PIN, HIGH);

  // calcula a distancia, considerando a velocidade do som
  distanceCm = duration * SOUND_SPEED / 2;

  // Converte distancia em polegadas
  distanceInch = distanceCm * CM_TO_INCH;

  if (distanceCm < 50) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  // print dos resultados
  Serial.print("distancia (cm): ");
  Serial.println(distanceCm);
  delay(250);
}
