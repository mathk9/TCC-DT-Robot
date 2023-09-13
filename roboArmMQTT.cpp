#include <Ethernet.h> //Carregar a biblioteca Ethernet
#include <EthernetUdp.h> //Carregar a biblioteca Udp
#include <SPI.h> //Carregar a biblioteca SPI
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <Servo.h>
#include <Regexp.h>
#include <ThreadController.h>

#include "Wire.h" //Importar a biblioteca Wire

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}; //Atribuir o endereço MAC
IPAddress ip(192, 168, 0, 51); //Atribuir o endereço IP
unsigned int localPort = 80; //Atribuir uma porta para comunicação
IPAddress server(52, 149, 214, 24);

const char* BROKER_MQTT = "52.149.214.24"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

//defines:
//defines de id mqtt e tópicos para publicação e subscribe denominado TEF(Telemetria e Monitoramento de Equipamentos)
#define TOPICO_SUBSCRIBE "/TEF/DeviceRoboArm001/cmd"    //tópico MQTT de escuta
#define TOPICO_PUBLISH   "/TEF/DeviceRoboArm001/attrs"  //tópico MQTT de envio de informações para Broker

//IMPORTANTE: recomendamos fortemente alterar os nomes
//            desses tópicos. Caso contrário, há grandes
//            chances de você controlar e monitorar o NodeMCU
//            de outra pessoa.

#define ID_MQTT  "fiware_01"     //id mqtt (para identificação de sessão)
//IMPORTANTE: este deve ser único no broker (ou seja,
//            se um client MQTT tentar entrar com o mesmo
//            id de outro já conectado ao broker, o broker
//            irá fechar a conexão de um deles).

EthernetClient ethClient;
PubSubClient client(ethClient);

ThreadController  mqttThread = ThreadController();
ThreadController  mqttThreadRead = ThreadController();

void mqtt_callback(char* topic, byte* payload, unsigned int length);

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void mqtt_callback(char* topic, byte* payload, unsigned int length){
    String msg;
     
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
    
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);

    EnviaAngloServosMQTT();
    
    delay(1000);
}


//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT(){
  while (!client.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if(client.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      client.subscribe(TOPICO_SUBSCRIBE);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentativa de conexao em 2s");
      delay(2000);
    }
  }
}

void setup(){
  servo1.attach(9);
  servo2.attach(11);
  servo3.attach(8);
  servo4.attach(10);

  Serial.begin(9600);  

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);

  client.setServer(BROKER_MQTT, BROKER_PORT);
  client.setCallback(mqtt_callback);

  // Inicia a thread MQTT
  mqttThread.onRun(mqttLoop);
  mqttThread.setInterval(100);  // Intervalo de verificação em milissegundos
  mqttThread.enabled = true;

  mqttThreadRead.onRun(readMQTT);
  mqttThread.setInterval(1000);  // Intervalo de verificação em milissegundos
  mqttThread.enabled = true;
}

void EnviaAngloServosMQTT() {
  client.publish(TOPICO_PUBLISH, ("mt1|" + String(servo1.read())).c_str());
  client.publish(TOPICO_PUBLISH, ("mt2|" + String(servo2.read())).c_str());
  client.publish(TOPICO_PUBLISH, ("mt3|" + String(servo3.read())).c_str());
  client.publish(TOPICO_PUBLISH, ("mt4|" + String(servo4.read())).c_str());
  
  Serial.println("Motor1: " + String(servo1.read()));
  Serial.println("Motor2: " + String(servo2.read()));
  Serial.println("Motor3: " + String(servo3.read()));
  Serial.println("Motor4: " + String(servo4.read()));
  Serial.println("*--------//--------*");
  
  delay(2000);
}

void readMQTT(){
  if(client.connected()){
    Serial.println("readMQTT");
    client.loop();
    delay(10);
  }
}

void mqttLoop() {
  // Verifica a conexão MQTT e reconecta-se, se necessário
  if (!client.connected()) {
    reconnectMQTT();
  }

  // Mantenha a conexão MQTT ativa
  client.loop();
}

void loop(){
  // Execute a thread MQTT
  mqttThread.run();
  mqttThreadRead.run();
    
  //EnviaAngloServosMQTT();  
}