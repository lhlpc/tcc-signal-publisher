// https://www.filipeflop.com/blog/esp32-e-mqtt-dashboard-android/
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


// JSON to send
const int capacity = JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> doc;
char jsonBuffer[256];

 
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
 
/* Variaveis, constantes e objetos globais */
//DHT dht(DHTPIN, DHTTYPE);
 
const char* SSID = "LHL"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "henrique94"; // Senha da rede WI-FI que deseja se conectar
 
const char* BROKER_MQTT = "broker.hivemq.com"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT
   
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
 
/* Prototypes */

void initWiFi(void);
void initMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);
 
/*
 * Implementações
 */
 
/* Função: inicializa e conecta-se na rede WI-FI desejada
*  Parâmetros: nenhum
*  Retorno: nenhum
*/
void initWiFi(void) 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
      
    reconnectWiFi();
}
 
/* Função: inicializa parâmetros de conexão MQTT(endereço do 
 *         broker, porta e seta função de callback)
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void initMQTT(void) 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
//    MQTT.setCallback(mqtt_callback);         //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
 
/* Função: função de callback 
 *         esta função é chamada toda vez que uma informação de 
 *         um dos tópicos subescritos chega)
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
//void mqtt_callback(char* topic, byte* payload, unsigned int length) 
//{
//    String msg;
//  
//    /* obtem a string do payload recebido */
//    for(int i = 0; i < length; i++) 
//    {
//       char c = (char)payload[i];
//       msg += c;
//    }
// 
//    Serial.print("Chegou a seguinte string via MQTT: ");
//    Serial.println(msg);
//    
//    /* toma ação dependendo da string recebida */
//    if (msg.equals("1"))
//    {
////        digitalWrite(PIN_LED, HIGH);
//        Serial.print("LED aceso mediante comando MQTT");
//    }
//  
//    if (msg.equals("0"))
//    {
////        digitalWrite(PIN_LED, LOW);    
//        Serial.print("LED apagado mediante comando MQTT"); 
//    }
//}
 
/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
 *         em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
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
//            MQTT.subscribe(TOPICO_SUBSCRIBE_LED); 
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
 *         Em caso de desconexão (qualquer uma das duas), a conexão
 *         é refeita.
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()){
      reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    }
        
     reconnectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
 
/* Função: reconecta-se ao WiFi
 * Parâmetros: nenhum
 * Retorno: nenhum
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
  
/* Função de setup */
void setup() 
{
    Serial.begin(115200);  

    Serial.print("++++++ alou 1");

    doc["idDevice"] = 456;
    doc["idUser"] = 1;
    serializeJson(doc, jsonBuffer);
 
    /* Inicializa a conexao wi-fi */
    initWiFi();
 
    /* Inicializa a conexao ao broker MQTT */
    initMQTT();
}
 
/* Loop principal */
void loop() 
{

  Serial.print("++++++ alou");


     
    /* garante funcionamento das conexões WiFi e ao broker MQTT */
  VerificaConexoesWiFIEMQTT();

//  HTTPClient httpClient;
//  httpClient.begin("https://jsonplaceholder.typicode.com/todos/1");
//  int httpCode = httpClient.GET();
//  if(httpCode > 0) {
//    String payload = httpClient.getString();  
//    Serial.println(payload);
//  } else {
//    Serial.println("Error on HTTP");
//   }

Serial.print("++++++ alou 3");
//    MQTT.publish(EMG_PUBLISH_TOPIC, "teste 3");
    MQTT.publish(EMG_PUBLISH_TOPIC, jsonBuffer);
//   
//    /* keep-alive da comunicação com broker MQTT */
    MQTT.loop();
// 
//    /* Refaz o ciclo após 2 segundos */
    delay(2000);
}
