/*=================================================================================*/
/*Para obter a máscara de bits GPIOs, siga as próximas etapas:
  1- Calcule 2^(GPIO_NUMBER). Salve o resultado em decimal;
  2- Substitua o número hexadecimal que você obteve em BUTTON_PIN_BITMASK .
*/

/*=================================================================================*/
/*Máscara para vários GPIOs:
  Se você quiser usar GPIO2 e GPIO15, como fonte de ativação,
  você deve fazer o seguinte:
  1- Calcule 2^2 + 2^15. Você deve obter 32772
  2- converta esse número para hexadecimal. Você deve obter: 8004
  3- Substitua esse número em BUTTON_PIN_BITMASK
*/
/*=================================================================================*/
#define BUTTON_PIN_BITMASK 0x80000 // 2^2 in hex
#define LED_PIN 22 /*GPIO22*/
#define SENSOR_GATILHO 13 /*GPIO_RTC*/

/* Modulo de temperatura e humidade */
#include "DHT.h"
#define DHTTYPE DHT22   /*DHT 22 (AM2302)*/
#define PIN_HUMIDITY_SENSOR  4  /* ESP32 pin GPIO4*/

#define uS_TO_S_FACTOR 1000000ULL  /* Fator de conversão de micro segundos para segundos */
#define TIME_TO_SLEEP  10        /* Tempo que o ESP32 vai dormir: 60 min (em segundos) */

/*=======================SLEEP CONFIG========================================================*/
RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0 :
      Serial.println("Despertar causado por sinal externo usando RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1 :
      Serial.println("Despertar causado por sinal externo usando RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER :
      Serial.println("Despertar causado pelo temporizador");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD :
      Serial.println("Despertar causado pelo touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP :
      Serial.println("Despertar causado pelo programa ULP");
      break;
    default :
      Serial.printf("O despertar não foi causado pelo sono profundo: %d\n", wakeup_reason);
      break;
  }
}
/* =================================DHT Config================================================*/
/*Initialize DHT sensor.*/
DHT dht(PIN_HUMIDITY_SENSOR, DHTTYPE);
float Temperatura;
float Umidade;

/* =================================PTHREAD Config============================================*/
pthread_t task1;
pthread_t task2;
int flag_sensor;
unsigned long tempoAnterior = 0;
unsigned long agora = 0;
/*funcao a ser chamada quando houver interrupcao ativo pelo sensor de vibracao*/
void IRAM_ATTR alertaFurto() {
  pinMode(LED_PIN, OUTPUT);
  //  digitalWrite(LED_PIN, HIGH);
  while (1) {
    while (millis() < agora + 4000) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ACESO");
      Serial.println("task Alerta indo dormir agora");
      Serial.println("");
      esp_deep_sleep_start();
    }


  }
}

/*funcao que aguarda a ativacao do sensor*/
void *tarefa1(void *ptr) {
  Serial.println("");
  Serial.println("INICIANDO TASK ALERTA FURTO");
  /*sensor como entrada*/
  pinMode(SENSOR_GATILHO, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 1);
  /*imprime o motivo de ativação */
  print_wakeup_reason();
  Serial.println("");
  while (1) {
    flag_sensor = digitalRead(SENSOR_GATILHO);
    if (flag_sensor == HIGH) {
      attachInterrupt(SENSOR_GATILHO, alertaFurto, RISING);
    }
  }
}
/*funcao que faz a leitura periodica da temperatura*/
void *tarefa2(void *ptr) {
  Serial.println("");
  Serial.println("INICIANDO TASK TEMPERATURA POR TIMER");
  while (1) {
    dht.begin();
    /*imprime o motivo de ativação */
    print_wakeup_reason();
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Configurando a ESP32 para despertar a cada " + String(TIME_TO_SLEEP) + " segundos");

    Temperatura = dht.readTemperature();
    Umidade = dht.readHumidity();
    Serial.println("Temp:");
    Serial.println(Temperatura);
    Serial.println("Umidade:");
    Serial.println(Umidade);
    Serial.println("task Temperatura indo dormir agora");
    Serial.println("");
    delay(3000);
    esp_deep_sleep_start();

  }
}
/* =====================================================================================*/

void setup() {
  Serial.begin(115200);

  pthread_create(&task1, NULL, (void*(*)(void*))tarefa1, NULL);
  pthread_create(&task2, NULL, (void*(*)(void*))tarefa2, NULL);
  pthread_join(task1, NULL);
  pthread_join(task2, NULL);
}

void loop() {
}
