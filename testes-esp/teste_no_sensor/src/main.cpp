/*
 * =====================================================================================
 *
 *       Filename:  main.ino
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/02/2023 14:13:20
 *       Revision:  none
 *       Compiler:  -
 *
 *         Author:  Isaac Vinicius, isaacvinicius2121@alu.ufc.br
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "DHT.h"
#include "ccs811.h"
#include <HX711.h>
#include <SPI.h>
#include <SD.h>
#define DHTTYPE DHT22             /* DHT 22 (AM2302) */
#define PIN_HUMIDITY_SENSOR 14    /* ESP32 pin GPIO14 */
#define SENSOR_GATILHO 32         /* GPIO_RTC */
#define PIN_LIGHT_SENSOR 4        /* ESP32 pin GPIO4 */
#define PIN_VIBRATE_SENSOR_D32 32 /* ESP32 pin GPI32 */
#define PIN_VIBRATE_SENSOR_A33 33 /* ESP32 pin GPIO33 (ADC1_5)*/
#define pinDT_PESO 25             /* ESP32 pin GPIO25 */
#define pinSCK_PESO 26            /* ESP32 pin GPIO26 */

/*=======================SLEEP CONFIG========================================================*/
#define uS_TO_S_FACTOR 1000000ULL /* Fator de conversão de micro segundos para segundos */
#define TIME_TO_SLEEP 10          /* Tempo que o ESP32 vai dormir: 10  (em segundos) */

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Despertar causado por sinal externo usando RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Despertar causado por sinal externo usando RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Despertar causado pelo temporizador");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Despertar causado pelo touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Despertar causado pelo programa ULP");
    break;
  default:
    Serial.printf("O despertar não foi causado pelo sono profundo: %d\n", wakeup_reason);
    break;
  }
}
/* =================================HX711 Config================================================*/
/* Modulo de peso */
HX711 scale;
float medida = 0;

/* =================================CMJCU Config================================================*/
/* Modulo de CO2 */
CCS811 ccs811;

/* =================================DHT Config================================================*/
/* Modulo de temperatura e umidade */
DHT dht(PIN_HUMIDITY_SENSOR, DHTTYPE);
float Temperatura;
float Umidade;

/* =================================PTHREAD Config============================================*/
#include "pthread.h"
pthread_t task1;    /*identificador da thread 1*/
pthread_t task2;    /*identificador da thread 2*/
pthread_t task3;    /*identificador da thread 3*/
pthread_t task4;    /*identificador da thread 4*/
pthread_t task5;    /*identificador da thread 5*/
pthread_t task6;    /*identificador da thread 6*/

/*flag para verificacao do nivel logico do pino gatilho*/
int flag_sensor;
//unsigned long temp = 0;

/*funcao a ser chamada quando houver interrupcao ativo pelo sensor de vibracao*/
void IRAM_ATTR alertaFurto(){
  Serial.println("ROUBARAM");
  Serial.println("task Alerta indo dormir agora");
  //esp_deep_sleep_start();
}

/*funcao que aguarda a ativacao do sensor*/
void *tarefa1(void *ptr){
  Serial.println("INICIANDO TASK ALERTA FURTO");
  /*sensor como entrada*/
  pinMode(SENSOR_GATILHO, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 0);
  /*imprime o motivo de ativação */
  print_wakeup_reason();
  Serial.println("");
  while (1){
    flag_sensor = digitalRead(SENSOR_GATILHO);
    if (flag_sensor == LOW){
      attachInterrupt(SENSOR_GATILHO, alertaFurto, RISING);
    }
  }
}

/*funcao que faz a leitura periodica da temperatura*/
void *tarefa2(void *ptr){
  Serial.println("INICIANDO TASK TEMPERATURA POR TIMER");
  while (1){
    dht.begin();
    /*imprime o motivo de ativação */

    // print_wakeup_reason();
    // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    // Serial.println("Configurando a ESP32 para despertar a cada " + String(TIME_TO_SLEEP) + " segundos");
    Temperatura = dht.readTemperature();
    Umidade = dht.readHumidity();
    Serial.println("Temp:");
    Serial.println(Temperatura);
    Serial.println("Umidade:");
    Serial.println(Umidade);
    // Serial.println("task Temperatura indo dormir agora");
    // Serial.println("");
    delay(3000);
  }
  // esp_deep_sleep_start();
  // }
}

/*task para teste do modulo de luminosidade*/
void *tarefa3(void *ptr){
  Serial.println("INICIANDO TASK LUMINOSIDADE");
  pinMode(PIN_LIGHT_SENSOR, INPUT);
  while (1)
  {
    int dcValue = digitalRead(PIN_LIGHT_SENSOR);
    Serial.print("digital luminosidade: ");
    Serial.println(dcValue);
    delay(1000);
  }
}
/*task para teste do modulo de vibracao*/
void *tarefa4(void *ptr){
  Serial.println("INICIANDO TASK VIBRACAO");
  int leitura_porta_analogica = 0;
  int leitura_porta_digital = 0;
  pinMode(PIN_VIBRATE_SENSOR_D32, INPUT);
  pinMode(PIN_VIBRATE_SENSOR_A33, INPUT);

  while (1){
    leitura_porta_digital = digitalRead(PIN_VIBRATE_SENSOR_D32);
    leitura_porta_analogica = analogRead(PIN_VIBRATE_SENSOR_A33);

    // Testa se o sensor foi acionado
    if (leitura_porta_digital != 1){
      Serial.println("ACIONADO");
    }

    Serial.print("digital vibracao: ");
    Serial.println(leitura_porta_digital);
    delay(1000);
  }
}
/*task para teste do modulo de peso*/
void *tarefa5(void *ptr){
  scale.begin(pinDT_PESO, pinSCK_PESO); // CONFIGURANDO OS PINOS DA BALANÇA
  scale.set_scale();          // LIMPANDO O VALOR DA ESCALA coloca valor aqui

  delay(2000);
  scale.tare(); // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA

  Serial.println("Balança Zerada");

  while (1){
    medida = scale.get_units(5); // SALVANDO NA VARIAVEL O VALOR DA MÉDIA DE 5 MEDIDAS
    Serial.println(medida, 3);   // ENVIANDO PARA MONITOR SERIAL A MEDIDA COM 3 CASAS DECIMAIS

    scale.power_down(); // DESLIGANDO O SENSOR
    delay(1000);        // AGUARDA 5 SEGUNDOS
    scale.power_up();   // LIGANDO O SENSOR
  }
}

/*task para teste do modulo de CO2*/
void *tarefa6(void *ptr){
  // Enable I2C
  Wire.begin();
  bool ok = ccs811.begin();
  if (!ok)
    Serial.println("setup: CCS811 begin FAILED");

  // Print CCS811 versions
  Serial.print("setup: hardware    version: ");
  Serial.println(ccs811.hardware_version(), HEX);
  Serial.print("setup: bootloader  version: ");
  Serial.println(ccs811.bootloader_version(), HEX);
  Serial.print("setup: application version: ");
  Serial.println(ccs811.application_version(), HEX);

  // Start measuring
  ok = ccs811.start(CCS811_MODE_1SEC);
  if (!ok)
    Serial.println("setup: CCS811 start FAILED");
  while (1){
    // Read
    uint16_t eco2, etvoc, errstat, raw;
    ccs811.read(&eco2, &etvoc, &errstat, &raw);

    // Print measurement results based on status
    if (errstat == CCS811_ERRSTAT_OK){
      Serial.print("CCS811: ");
      Serial.print("eco2=");
      Serial.print(eco2);
      Serial.print(" ppm  ");
      Serial.print("etvoc=");
      Serial.print(etvoc);
      Serial.print(" ppb  ");
      // Serial.print("raw6=");  Serial.print(raw/1024); Serial.print(" uA  ");
      // Serial.print("raw10="); Serial.print(raw%1024); Serial.print(" ADC  ");
      // Serial.print("R="); Serial.print((1650*1000L/1023)*(raw%1024)/(raw/1024)); Serial.print(" ohm");
      Serial.println();
    }
    else if (errstat == CCS811_ERRSTAT_OK_NODATA){
      Serial.println("CCS811: waiting for (new) data");
    }else if (errstat & CCS811_ERRSTAT_I2CFAIL){
      Serial.println("CCS811: I2C error");
    }
    else{
      Serial.print("CCS811: errstat=");
      Serial.print(errstat, HEX);
      Serial.print("=");
      Serial.println(ccs811.errstat_str(errstat));
    }
    // Wait
    delay(1000);
  }
}

void setup(){
  Serial.begin(115200);

  // pthread_create(&task1, NULL, (void *(*)(void *))tarefa1, NULL);
  pthread_create(&task2, NULL, (void *(*)(void *))tarefa2, NULL);
  pthread_create(&task3, NULL, (void *(*)(void *))tarefa3, NULL);
  pthread_create(&task4, NULL, (void *(*)(void *))tarefa4, NULL);
  pthread_create(&task5, NULL, (void *(*)(void *))tarefa5, NULL);
  pthread_create(&task6, NULL, (void *(*)(void *))tarefa6, NULL);
  // pthread_join(task1, NULL);
  pthread_join(task2, NULL);
  pthread_join(task3, NULL);
  pthread_join(task4, NULL);
  pthread_join(task5, NULL);
  pthread_join(task6, NULL);
}
void loop()
{
}
