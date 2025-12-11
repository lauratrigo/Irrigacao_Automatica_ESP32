#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>
#include <DHT.h>

// wi-fi e MQTT
WiFiClientSecure espClient;     // cliente seguro
PubSubClient client(espClient); // cliente MQTT seguro

// configurações ThingsBoard
const char *THINGSBOARD_SERVER = "mqtt.thingsboard.cloud";
const int THINGSBOARD_PORT = 8883; // porta TLS
const char *ACCESS_TOKEN = "g140psgsm9403ih4hcpz";

// certificado CA do ThingsBoard (PEM)
const char *ISRG_ROOT_X1_PEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// configurações wi-fi
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// lcd
LiquidCrystal lcd(12, 14, 27, 26, 25, 33);

// pinos dos dispositivos
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

// variáveis
float umidadeSolo, luminosidade, temperatura, umidadeAr;
bool noite;

// funções 
void checkWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi caiu. Tentando reconectar...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    int tentativas = 0;
    while (WiFi.status() != WL_CONNECTED && tentativas < 10)
    {
      delay(1000);
      Serial.print(".");
      tentativas++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nWiFi reconectado!");
      Serial.print("Endereço IP: ");
      Serial.println(WiFi.localIP());
    }
    else
    {
      Serial.println("\nFalha ao reconectar WiFi.");
    }
  }
}

void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Conectando ao ThingsBoard...");
    if (client.connect("ESP32Wokwi", ACCESS_TOKEN, NULL))
    {
      Serial.println("Conectado via TLS!");
    }
    else
    {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  // conexão wi-fi
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20)
  {
    delay(500);
    Serial.print(".");
    tentativas++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nFalha ao conectar WiFi.");
  }

  // SSL/TLS 
  espClient.setCACert(ISRG_ROOT_X1_PEM);

  client.setServer(THINGSBOARD_SERVER, THINGSBOARD_PORT);

  // lds (diodos)
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // servo motor bomba d'água
  bomba.attach(SERVO_PIN);
  bomba.write(0);

  // display LCD
  lcd.begin(20, 4);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();

  // sensor DHT22
  dht.begin();

  randomSeed(esp_random());
}

void loop()
{
  // atualiza os sensores 
  int leituraSolo = analogRead(SENSOR_SOLO);
  umidadeSolo = map(leituraSolo, 0, 4095, 100, 0);

  int leituraLuz = analogRead(LDR_PIN);
  luminosidade = map(leituraLuz, 0, 4095, 0, 100);

  temperatura = dht.readTemperature();
  umidadeAr = dht.readHumidity();

  if (isnan(temperatura))
  {
    temperatura = 24 + random(-20, 20) * 0.1;
    umidadeAr = 40 + random(-50, 50) * 0.1;
  }

  noite = (luminosidade < 30);

  checkWiFi();

  if (!client.connected())
  {
    reconnectMQTT();
  }
  client.loop();

  // controle das condições de irrigação
  if (umidadeSolo < 20 || noite)
  {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(RELAY_PIN, HIGH);
    bomba.write(90);
  }
  else if (umidadeSolo >= 60)
  {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(RELAY_PIN, LOW);
    bomba.write(0);
  }
  else
  {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(RELAY_PIN, LOW);
    bomba.write(0);
  }

  // atualiza o display LCD
  lcd.setCursor(0, 0);
  lcd.print("Solo: ");
  lcd.print(umidadeSolo, 1);
  lcd.print("%      ");

  lcd.setCursor(0, 1);
  lcd.print("Luz: ");
  lcd.print(luminosidade, 1);
  lcd.print("%        ");

  lcd.setCursor(0, 2);
  lcd.print("Temp: ");
  lcd.print(temperatura, 1);
  lcd.print("C       ");

  lcd.setCursor(0, 3);
  lcd.print("UmAr: ");
  lcd.print(umidadeAr, 1);
  lcd.print("%");

  // envio dos dados criptografados via MQTT TLS para o ThingsBoard
  String payload = "{";
  payload += "\"umidadeSolo\":";
  payload += umidadeSolo;
  payload += ",\"luminosidade\":";
  payload += luminosidade;
  payload += ",\"temperatura\":";
  payload += temperatura;
  payload += ",\"umidadeAr\":";
  payload += umidadeAr;
  payload += "}";

  client.publish("v1/devices/me/telemetry", payload.c_str());
  Serial.println("Dados enviados ao ThingsBoard: " + payload);

  delay(2000);
}