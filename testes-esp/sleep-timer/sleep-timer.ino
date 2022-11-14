
#define PIN_LIGHT_SENSOR  2  /* ESP32 pin GPIO2 (ADC2_2)*/
#define LED_PIN           23  /* ESP32 pin GPIO23*/

#define uS_TO_S_FACTOR 1000000ULL  /* Fator de conversão de micro segundos para segundos */
#define TIME_TO_SLEEP  10        /* Tempo que o ESP32 vai dormir (em segundos) */

/*=================================================================================*/
/*Para obter a máscara de bits GPIOs, siga as próximas etapas:
  1- Calcule 2^(GPIO_NUMBER). Salve o resultado em decimal;
  2- Substitua o número hexadecimal que você obteve em BUTTON_PIN_BITMASK .
*/

/*=================================================================================*/
/*Máscara para vários GPIOs:
  Se você quiser usar GPIO2 eGPIO15, como fonte de ativação,
  você deve fazer o seguinte:
  1- Calcule 2^2 + 2^15. Você deve obter 32772
  2- converta esse número para hexadecimal. Você deve obter: 8004
  3- Substitua esse número em BUTTON_PIN_BITMASK
*/
/*=================================================================================*/

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

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);  /*Setando como saida*/
  pinMode(PIN_LIGHT_SENSOR, INPUT); /*Setando como entrada*/

  //Incrementa boot e printa a quantidade de reboot
  ++bootCount;
  Serial.println("Boot numero: " + String(bootCount));

  /*imprime o motivo de ativação */
  print_wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Configrando a ESP32 para dormir a cada " + String(TIME_TO_SLEEP) + " Seconds");
  
  delay(1000);
  int analogValue = analogRead(PIN_LIGHT_SENSOR); /*Leitura analogica*/
  if(analogValue < 600){
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
  }else{
    digitalWrite(LED_PIN, LOW);
    delay(1000);
  }
  Serial.print(analogValue);
  Serial.print("\n");
  delay(1000);

  Serial.println("Indo dormir agora");
  esp_deep_sleep_start();
  Serial.println("Isso nunca será impresso");
}


void loop() {
}
