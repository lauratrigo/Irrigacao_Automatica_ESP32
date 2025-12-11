#include <WiFi.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>
#include <DHT.h>

// --- CONFIGURAÇÕES WIFI ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";  // senha, se houver

// --- LCD ---
LiquidCrystal lcd(12, 14, 27, 26, 25, 33);

// --- PINOS ---
#define SENSOR_SOLO 34
#define LDR_PIN 35
#define LED_VERMELHO 16
#define LED_AMARELO 17
#define LED_VERDE 5
#define RELAY_PIN 13
#define SERVO_PIN 18
#define DHT_PIN 4

#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

Servo bomba;

// --- VARIÁVEIS ---
float umidadeSolo, luminosidade, temperatura, umidadeAr;
bool noite;

void setup() {
  Serial.begin(115200);

  // --- LEDs ---
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // --- Servo ---
  bomba.attach(SERVO_PIN);
  bomba.write(0);

  // --- LCD ---
  lcd.begin(20, 4);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();

  // --- Sensor DHT ---
  dht.begin();

  // --- Conexão WiFi ---
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 10) {
    delay(1000);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar ao WiFi, usando IP fictício");
    Serial.println("Endereço IP: 192.168.0.10");
  }

  randomSeed(analogRead(0));  // inicializa random
}

void loop() {
  // --- Leitura sensores ---
  int leituraSolo = analogRead(SENSOR_SOLO);
  umidadeSolo = map(leituraSolo, 0, 4095, 100, 0);

  int leituraLuz = analogRead(LDR_PIN);
  luminosidade = map(leituraLuz, 0, 4095, 0, 100);

  // --- Leitura DHT ---
  temperatura = dht.readTemperature();
  umidadeAr = dht.readHumidity();

  // --- Simulação de temperatura (para teste ou fallback) ---
  if (isnan(temperatura)) {
    temperatura = 25.3 + random(-5, 5) * 0.1;
  }

  noite = (luminosidade < 30);

  // --- Monitor serial ---
  Serial.print("Solo: "); Serial.print(umidadeSolo, 1); Serial.print("%  ");
  Serial.print("Luz: "); Serial.print(luminosidade, 1); Serial.print("%  ");
  Serial.print("Temp: "); Serial.print(temperatura, 1); Serial.print("C  ");
  Serial.print("UmAr: "); Serial.print(umidadeAr, 1); Serial.print("%  ");
  Serial.print("Noite? "); Serial.println(noite ? "Sim" : "Nao");

  // --- Controle irrigação ---
  if (umidadeSolo < 20 || noite) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(RELAY_PIN, HIGH);
    bomba.write(90);
  } else if (umidadeSolo >= 60) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(RELAY_PIN, LOW);
    bomba.write(0);
  } else {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(RELAY_PIN, LOW);
    bomba.write(0);
  }

  // --- Atualiza LCD ---
  lcd.setCursor(0, 0);
  lcd.print("Solo: "); lcd.print(umidadeSolo, 1); lcd.print("%      ");

  lcd.setCursor(0, 1);
  lcd.print("Luz: "); lcd.print(luminosidade, 1); lcd.print("%        ");

  lcd.setCursor(0, 2);
  lcd.print("Temp: "); lcd.print(temperatura, 1); lcd.print("C       ");

  lcd.setCursor(0, 3);
  lcd.print("UmAr: "); lcd.print(umidadeAr, 1); lcd.print("%");

  delay(2000);
}
