#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <Ultrasonic.h>

Ultrasonic ultrasonic(5, 4);
long distancia;

#define TOPICO_PUBLISH   "MQTTReceiveDistance"    //tópico MQTT de envio de informações para Broker

#define ID_MQTT  "HomeAut"

int lastCollectedData = 0;

int RED = D0;
int GREEN = D4;
int BLUE = D3;

const char* SSID = "GOMES";
const char* PASSWORD = "00888050267";
  
const char* BROKER_MQTT = "mqtt.eclipse.org"; 
int BROKER_PORT = 1883;

WiFiClient espClient; 
PubSubClient MQTT(espClient);


void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);

void setup() {
  InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
}

void initSerial() 
{
    Serial.begin(115200);
}

void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}

void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);            
}


void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
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

void reconectWiFi() 
{
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

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void VerifyCollectedData(int distance)
{
  Serial.print("Distancia recebida ");
  Serial.println(distance);

  Serial.print("Distancia armazenada ");
  Serial.println(lastCollectedData);

  if( distance > lastCollectedData + 1 ){
    Serial.println("- Distancia enviada ao broker!");
    MQTT.publish(TOPICO_PUBLISH, String(distancia).c_str());
    Serial.println("- Novo dado armazenado");
    lastCollectedData = distance;
  }

  if( distance < lastCollectedData - 1){
    Serial.println("- Distancia enviada ao broker!");
    MQTT.publish(TOPICO_PUBLISH, String(distancia).c_str());
    Serial.println("- Novo dado armazenado");
    lastCollectedData = distance;
  }
  
}


void EnviaEstadoOutputMQTT(void)
{

  distancia = ultrasonic.Ranging(CM);         
  Serial.print(distancia);
  Serial.println("cm");

  VerifyCollectedData(distancia);
  
  if (distancia <= 10){
    analogWrite(RED, 255);
    analogWrite(GREEN, 0);
    analogWrite(BLUE, 0);
  } else if ( distancia > 10 && distancia <= 20 ){
     analogWrite(RED, 255);
     analogWrite(GREEN, 255);
     analogWrite(BLUE, 0); 
  }else{
    analogWrite(RED, 0);
    analogWrite(GREEN, 255);
    analogWrite(BLUE, 0);
  }
  Serial.println(distancia);
  delay(1000);
  

}

void InitOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);

    analogWrite(RED, 0);
    analogWrite(GREEN, 255);
    analogWrite(BLUE, 0);             
}


void loop() {
      //garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();
 
    //envia o status de todos os outputs para o Broker no protocolo esperado
    EnviaEstadoOutputMQTT();
 
    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
}
