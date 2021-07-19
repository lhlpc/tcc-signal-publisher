#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//const char* SSID = ""; // SSID / nome da rede WI-FI que deseja se conectar
//const char* PASSWORD = ""; // Senha da rede WI-FI que deseja se conectar


// JSON to send
const int capacity = JSON_ARRAY_SIZE(2) + 4 * JSON_OBJECT_SIZE(2);
StaticJsonDocument<capacity> doc;
char jsonBuffer[256];
JsonArray signalSamples = doc.createNestedArray("signalSamples");


/* Definições do LED */
#define PIN_LED     25

/* Defines do MQTT */
#define EMG_PUBLISH_TOPIC "lpctcc/emg"

#define MQTT_CLIENT_ID  "clientId-pvAkhZbJdW"     //id mqtt (para identificação de sessão)
//IMPORTANTE: este deve ser único no broker (ou seja,
//            se um client MQTT tentar entrar com o mesmo
//            id de outro já conectado ao broker, o broker
//            irá fechar a conexão de um deles).
#define MQTT_USERNAME "useraskjas"
#define MQTT_PASSWORD "ajshdajsdhv"

#define EMG_PIN 35 // EMG mesmo

/* Variaveis, constantes e objetos globais */
//DHT dht(DHTPIN, DHTTYPE);

const char* BROKER_MQTT = "broker.hivemq.com"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient


void initWiFi(void);
void initMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);

void initWiFi(void)
{
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconnectWiFi();
}


void initMQTT(void)
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
}

void reconnectMQTT(void)
{
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    Serial.println(MQTT.state());
    Serial.println(MQTT_CLIENT_ID);
    Serial.println(MQTT_USERNAME);
    Serial.println(MQTT_PASSWORD);
    Serial.println(BROKER_MQTT);
    Serial.println(BROKER_PORT);
    if (MQTT.connect(MQTT_CLIENT_ID))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentatica de conexao em 2s");
      delay(2000);
    }
  }
}

/* Função: verifica o estado das conexões WiFI e ao broker MQTT.
           Em caso de desconexão (qualquer uma das duas), a conexão
           é refeita.
   Parâmetros: nenhum
   Retorno: nenhum
*/
void VerificaConexoesWiFIEMQTT(void)
{
  if (!MQTT.connected()) {
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
  }

  reconnectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

/* Função: reconecta-se ao WiFi
   Parâmetros: nenhum
   Retorno: nenhum
*/
void reconnectWiFi(void)
{
  //se já está conectado a rede WI-FI, nada é feito.
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

/* Função de setup inicial do software */
void setup()
{
  Serial.begin(115200);

  doc["idUser"] = 10;

  /* Inicializa a conexao WiFi */
  initWiFi();

  /* Inicializa a conexao ao broker MQTT */
  initMQTT();
}

/* Loop principal */
void loop()
{

  signalSamples[0] = analogRead(EMG_PIN);
  serializeJson(doc, jsonBuffer);

  Serial.println(jsonBuffer);

  VerificaConexoesWiFIEMQTT();

  // Envia sinal captado para conexão com broket MQTT
  MQTT.publish(EMG_PUBLISH_TOPIC, jsonBuffer);

  // Manter conexão com broker MQTT ativa
  MQTT.loop();
  delay(300);
}
