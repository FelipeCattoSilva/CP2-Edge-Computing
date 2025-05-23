#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>

// Definições dos pinos
#define DHTPIN 2
#define DHTTYPE DHT22
#define LDR_PIN A0
#define BUZZER_PIN 3
#define LED_RED 4
#define LED_YELLOW 5
#define LED_GREEN 6


// Limites para alertas
#define TEMP_MIN 6
#define TEMP_MAX 20
#define HUMID_MIN 60
#define HUMID_MAX 80
#define LIGHT_MIN 0.1
#define LIGHT_MAX 350

// Inicialização dos componentes
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;

// Variáveis globais
float temperature, humidity;
int lightValue, mappedLight;
unsigned long previousMillis = 0;
const long interval = 10000; // Intervalo de 10s para média
int lightReadings[10];
int readingIndex = 0;
int total = 0;
int average = 0;

// Estrutura para logs
struct LogEntry {
  DateTime time;
  float temp;
  float humid;
  int light;
};
int logIndex = 0;
unsigned long lastLogMillis = 0; // Para controle do log a cada minuto

// Array tridimensional para armazenar os 3 frames da animação do cavalo
byte cavalo[3][8][8] = {
  {
    {0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0E},
    {0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x00, 0x03, 0x07, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x05, 0x1F, 0x1D, 0x1F, 0x16, 0x06},
    {0x0C, 0x18, 0x10, 0x00, 0x01, 0x01, 0x01, 0x00},
    {0x1F, 0x1F, 0x1E, 0x17, 0x00, 0x00, 0x10, 0x00},
    {0x1F, 0x1F, 0x03, 0x02, 0x14, 0x04, 0x02, 0x00},
    {0x1C, 0x1C, 0x04, 0x04, 0x08, 0x00, 0x00, 0x00}
  },
  {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1E},
    {0x00, 0x00, 0x00, 0x00, 0x0E, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x00, 0x01, 0x07, 0x1F, 0x1F, 0x1F},
    {0x00, 0x05, 0x1F, 0x1D, 0x1F, 0x1B, 0x13, 0x10},
    {0x13, 0x03, 0x06, 0x0C, 0x10, 0x10, 0x00, 0x00},
    {0x1F, 0x17, 0x06, 0x0C, 0x10, 0x10, 0x00, 0x00},
    {0x1F, 0x1F, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00},
    {0x10, 0x18, 0x1C, 0x08, 0x08, 0x00, 0x10, 0x00}
  },
  {
    {0x00, 0x00, 0x00, 0x07, 0x0F, 0x0E, 0x1C, 0x18},
    {0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x00, 0x00, 0x01, 0x03, 0x1F, 0x1F, 0x1F, 0x1F},
    {0x14, 0x1C, 0x1A, 0x1E, 0x1F, 0x13, 0x10, 0x10},
    {0x13, 0x13, 0x02, 0x02, 0x04, 0x00, 0x00, 0x00},
    {0x1F, 0x07, 0x0E, 0x06, 0x01, 0x00, 0x00, 0x00},
    {0x0F, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00},
    {0x10, 0x18, 0x0C, 0x02, 0x02, 0x11, 0x00, 0x00}
  }
};

// Função utilitária para imprimir dois dígitos
void printDigits(int digits) {
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

// Função que exibe uma animação de carregamento com o cavalo se movendo
void animacaoCarregamento() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Carregando...");

  int posicaoCavalo = 0; // Posição inicial do cavalo

  for (int progresso = 0; progresso <= 100; progresso += 10) {
    lcd.setCursor(0, 1);
    lcd.print(progresso);
    lcd.print("%");

    for (int frame = 0; frame < 3; frame++) {
      // Cria os 8 caracteres personalizados para o frame atual
      for (int i = 0; i < 8; i++) {
        lcd.createChar(i, cavalo[frame][i]);
      }
      // Limpa a linha 1 para evitar sobreposição
      lcd.setCursor(0, 1);
      lcd.print("                "); // 16 espaços

      // Escreve os 4 caracteres do cavalo na posição atual
      lcd.setCursor(posicaoCavalo, 1);
      for (int col = 0; col < 4; col++) {
        if (posicaoCavalo + col < 16) {
          lcd.write(byte(col)); // Escreve os caracteres 0 a 3
        }
      }
      delay(150); // Atraso para suavizar a animação

      // Move o cavalo uma coluna para a direita
      posicaoCavalo = (posicaoCavalo + 1) % 13; // De 0 a 12
    }
    delay(50); // Atraso para atualizar a porcentagem
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UnlimitTech");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Solution");
  delay(1000);

  for (int i = 0; i < 3; i++) {
    lcd.setCursor(13, 1);
    lcd.print(" ");
    delay(500);
    lcd.setCursor(13, 1);
    lcd.print("<>");
    delay(500);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bem-Vindo!");
  delay(2000);
}

void showLogoAnimation() {
  animacaoCarregamento();
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();
  Wire.begin();
  rtc.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  if (!rtc.begin()) {
    Serial.println("Não achei o RTC");
    Serial.flush();
    abort();
  }

  // Mostra animação inicial
  showLogoAnimation();

  // Inicializa array de leituras de luz
  for (int i = 0; i < 10; i++) {
    lightReadings[i] = 0;
  }
}

void displaySensorData() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Umid: ");
  lcd.print(humidity);
  lcd.print("%");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Luz: ");
  lcd.print(average);
  lcd.print("%");

  // Mostra data no formato DD/MM/YYYY na segunda linha
  DateTime now = rtc.now();
  lcd.setCursor(0, 1);
  lcd.print(now.day(), DEC);
  lcd.print("/");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.year(), DEC);
  lcd.print("-");
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute());
  delay(1000);
}

void checkAlerts() {
  // Desliga todos os LEDs e buzzer inicialmente
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
  noTone(BUZZER_PIN);

  // Verifica temperatura
  if (temperature < TEMP_MIN || temperature > TEMP_MAX) {
    digitalWrite(LED_RED, HIGH);
    tone(BUZZER_PIN, 1000, 500);
  }

  // Verifica umidade
  if (humidity < HUMID_MIN || humidity > HUMID_MAX) {
    digitalWrite(LED_YELLOW, HIGH);
    tone(BUZZER_PIN, 800, 500);
  }

  // Verifica luminosidade
  int ldr_value = analogRead(LDR_PIN);
  if (ldr_value < LIGHT_MIN) {
    digitalWrite(LED_YELLOW, HIGH);
    tone(BUZZER_PIN, 600, 500);
  } else if (ldr_value > LIGHT_MAX) {
    digitalWrite(LED_RED, HIGH);
    tone(BUZZER_PIN, 1200, 500);
  } else {
    digitalWrite(LED_GREEN, HIGH);
  }
}

void storeLog() {
  if (logIndex >= EEPROM.length() / sizeof(LogEntry)) {
    logIndex = 0; // Volta ao início se a EEPROM estiver cheia
  }

  LogEntry entry;
  entry.time = rtc.now();
  entry.temp = temperature;
  entry.humid = humidity;
  entry.light = average;

  EEPROM.put(logIndex * sizeof(LogEntry), entry);
  logIndex++;
}

void loop() {
  unsigned long currentMillis = millis();

  // Leitura dos sensores a cada 2 segundos
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;

    // Lê temperatura e umidade
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    // Lê luminosidade e faz a média
    lightValue = analogRead(LDR_PIN);
    mappedLight = map(lightValue, 0, 1023, 0, 100); // Calibração com map()
    mappedLight = constrain(mappedLight, 0, 100);

    // Atualiza média móvel
    total = total - lightReadings[readingIndex];
    lightReadings[readingIndex] = mappedLight;
    total = total + lightReadings[readingIndex];
    readingIndex = (readingIndex + 1) % 10;
    average = total / 10;

    // Mostra valores no LCD
    displaySensorData();

    // Verifica alertas
    checkAlerts();

    // Armazena log a cada minuto
    if (currentMillis - lastLogMillis >= 60000) {
      storeLog();
      lastLogMillis = currentMillis;
    }
  }
}
