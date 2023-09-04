#include <Ethernet.h> //Carregar a biblioteca Ethernet
#include <EthernetUdp.h> //Carregar a biblioteca Udp
#include <SPI.h> //Carregar a biblioteca SPI
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <Servo.h>
#include <Regexp.h>

#include "Wire.h" //Importar a biblioteca Wire

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}; //Atribuir o endereço MAC
IPAddress ip(192, 168, 0, 51); //Atribuir o endereço IP
unsigned int localPort = 80; //Atribuir uma porta para comunicação
IPAddress server(52, 149, 214, 24);

const char* BROKER_MQTT = "52.149.214.24"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

//defines:
//defines de id mqtt e tópicos para publicação e subscribe denominado TEF(Telemetria e Monitoramento de Equipamentos)
#define TOPICO_SUBSCRIBE "/4jggokgpepnvsb2uv4s40d59ag/robo_arm_002/cmd"    //tópico MQTT de escuta
#define TOPICO_PUBLISH   "ul/4jggokgpepnvsb2uv4s40d59ag/robo_arm_002/attrs"  //tópico MQTT de envio de informações para Broker

//IMPORTANTE: recomendamos fortemente alterar os nomes
//            desses tópicos. Caso contrário, há grandes
//            chances de você controlar e monitorar o NodeMCU
//            de outra pessoa.

#define ID_MQTT  "fiware"     //id mqtt (para identificação de sessão)
//IMPORTANTE: este deve ser único no broker (ou seja,
//            se um client MQTT tentar entrar com o mesmo
//            id de outro já conectado ao broker, o broker
//            irá fechar a conexão de um deles).

EthernetClient ethClient;
PubSubClient client(ethClient);

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Entrou no callback");
  // Convertendo o byte* para char* para uso com a biblioteca Regex
  char* payloadStr = reinterpret_cast<char*>(payload);

  String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    Serial.print("msg: ");
    Serial.println(msg);

  /*if (msg.equals("lamp001@on|")) {

    // Limitar o ângulo entre 0 e 180
    int angle = constrain(value.toInt(), 0, 180);

    // Mover o servo para o ângulo desejado
    if (command.equals("altMotor1")) {
      servo1.write(angle);
    }
    else if (command.equals("altMotor2")) {
      servo2.write(angle);
    }
    else if (command.equals("altMotor3")) {
      servo3.write(angle);
    }
    else if (command.equals("altMotor4")) {
      servo4.write(angle);
    }

    delay(1000);
  }*/
}


//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT()
{
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

void setup()
{
  servo1.attach(9);
  servo2.attach(11);
  servo3.attach(8);
  servo4.attach(10);

  Serial.begin(57600);  

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);

  client.setServer(BROKER_MQTT, BROKER_PORT);
  client.setCallback(callback);
}

void EnviaAngloServosMQTT() {
  client.publish(TOPICO_PUBLISH, "mt1|12");
  client.publish(TOPICO_PUBLISH, "mt2|" + servo2.read());
  client.publish(TOPICO_PUBLISH, "mt3|" + servo3.read());
  client.publish(TOPICO_PUBLISH, "mt4|" + servo4.read());
  
  Serial.println("Motor1: " + String(servo1.read()));
  Serial.println("Motor2: " + String(servo2.read()));
  Serial.println("Motor3: " + String(servo3.read()));
  Serial.println("Motor4: " + String(servo4.read()));
  
  delay(6000);
}

void loop()
{
  if (!client.connected()) {
    reconnectMQTT();
  }

  EnviaAngloServosMQTT();

  client.loop();
}