
#define PIN_LIGHT_SENSOR  2  /* ESP32 pin GPIO2 (ADC2_2)*/
#define LED_PIN           22  /* ESP32 pin GPIO22*/
#define uS_TO_S_FACTOR 1000000ULL  /* Fator de conversão de micro segundos para segundos */
#define TIME_TO_SLEEP  5        /* Tempo que o ESP32 vai dormir: 5 min (em segundos) */

/* leitura e escrita em cartões SD */
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SD_MMC.h"

#define MISO_PIN 19   /*ESP32 PIN GPIO19 */
#define MOSI_PIN 23   /*ESP32 PIN GPIO23 */
#define SCK_PIN 18    /*ESP32 PIN GPIO18 */
#define SDCS_PIN 5    /*ESP32 PIN GPIO5 */

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
/*=======================SDCard CONFIG========================================================*/
void createDir(fs::FS &fs, const char * path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Diretorio criado");
  } else {
    Serial.println("Falha ao criar o diretorio");
  }
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Falha ao abrir o arquivo para gravacao");
    return;
  }
  if (file.print(message)) {
    Serial.println("Arquivo escrito");
  } else {
    Serial.println("Falha na gravacao");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Anexando ao arquivo: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Falha ao abrir o arquivo para anexar");
    return;
  }
  if (file.print(message)) {
    Serial.println("Mensagem anexada");
  } else {
    Serial.println("Falha ao anexar");
  }
  file.close();
}

String dataMessage;
void logSDcard(int value){
  dataMessage = String(value) + "\r\n";
    Serial.print("Dados salvos: ");
    Serial.println(dataMessage);

    appendFile(SD, "/Dados/data.txt", dataMessage.c_str()); /*Anexando dados lidos pelo sensor*/
}
/* =====================================================================================*/

void setup() {
  Serial.begin(115200);
  Serial.print("Inicializando cartão SD...");
  if (!SD.begin(SDCS_PIN)) {
    Serial.println("Falha na inicialização do SD Card.");
    return;
  }
  Serial.println("\nSD Card pronto para uso.");
  
  createDir(SD, "/Dados");  /*Criando um novo diretorio no micro SD*/

  File file = SD.open("/Dados/data.txt"); /*Verificacao do arquivo de texto que ira armazenar os dados*/
  if (!file) {
    Serial.println("O arquivo nao existe!");
    Serial.println("Criando arquivo...");
    writeFile(SD, "/Dados/data.txt", "Taxa de Luminosidade: \r\n");
  } else {
    Serial.println("O arquivo ja existe!");
  }
  file.close();
  
  pinMode(LED_PIN, OUTPUT); /*Setando como saida*/
  pinMode(PIN_LIGHT_SENSOR, INPUT); /*Setando como entrada*/

  ++bootCount;
  Serial.println("Boot numero: " + String(bootCount)); /*Incrementa boot e printa a quantidade de reboot*/

  print_wakeup_reason(); /*imprime o motivo de ativação */

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Configure o ESP32 para dormir a cada " + String(TIME_TO_SLEEP) + " Seconds");
  delay(1000);
  int analogValue = analogRead(PIN_LIGHT_SENSOR); // Leitura analogica
  if(analogValue < 600){
    logSDcard(analogValue); /*Registrando dados no micro SD*/
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
