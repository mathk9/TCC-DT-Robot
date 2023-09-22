#include <Ethernet.h>     //Carregar a biblioteca Ethernet
#include <EthernetUdp.h>  //Carregar a biblioteca Udp
#include <SPI.h>          //Carregar a biblioteca SPI
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <Servo.h>
#include <Regexp.h>
#include <ThreadController.h>

#include "Wire.h" //Importar a biblioteca Wire

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}; // Atribuir o endereço MAC
IPAddress ip(192, 168, 0, 51);                     // Atribuir o endereço IP
unsigned int localPort = 80;                       // Atribuir uma porta para comunicação
IPAddress server(52, 149, 214, 24);

const char *BROKER_MQTT = "52.149.214.24"; // URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                    // Porta do Broker MQTT

// defines:
// defines de id mqtt e tópicos para publicação e subscribe denominado TEF(Telemetria e Monitoramento de Equipamentos)
#define TOPICO_SUBSCRIBE "/TEF/DeviceRoboArm001/cmd" // tópico MQTT de escuta
#define TOPICO_PUBLISH "/TEF/DeviceRoboArm001/attrs" // tópico MQTT de envio de informações para Broker

// IMPORTANTE: recomendamos fortemente alterar os nomes
//             desses tópicos. Caso contrário, há grandes
//             chances de você controlar e monitorar o NodeMCU
//             de outra pessoa.

#define ID_MQTT "fiware_01" // id mqtt (para identificação de sessão)
// IMPORTANTE: este deve ser único no broker (ou seja,
//             se um client MQTT tentar entrar com o mesmo
//             id de outro já conectado ao broker, o broker
//             irá fechar a conexão de um deles).

EthernetClient ethClient;
PubSubClient client(ethClient);

ThreadController mqttThread = ThreadController();
ThreadController mqttThreadRead = ThreadController();

void mqtt_callback(char *topic, byte *payload, unsigned int length);

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup()
{
  servo1.attach(9);
  servo2.attach(11);
  servo3.attach(8);
  servo4.attach(10);

  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  delay(1000);
  servo4.detach();

  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);

  client.setServer(BROKER_MQTT, BROKER_PORT);
  client.setCallback(mqtt_callback);
  client.setKeepAlive(6); // Configurar Keep Alive Interval para 60 segundos

  // Inicia a thread MQTT
  mqttThread.onRun(mqttLoop);
  mqttThread.setInterval(1000); // Intervalo de verificação em milissegundos
  mqttThread.enabled = true;

  mqttThreadRead.onRun(readMQTT);
  mqttThread.setInterval(1000); // Intervalo de verificação em milissegundos
  mqttThread.enabled = true;
}

// Função: função de callback
//         esta função é chamada toda vez que uma informação de
//         um dos tópicos subescritos chega)
// Parâmetros: nenhum
// Retorno: nenhum
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
  String msg;

  // obtem a string do payload recebido
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.print("- Mensagem recebida: ");
  Serial.println(msg);

  // verifica se deve colocar nivel alto de tensão na saída D0:
  if (msg.equals("DeviceRoboArm001@moveMotor1|"))
  {
    // delay(1000);
    // long randNumber;
    // randNumber = random(40, 120);
    // servo1.write(randNumber);

    // Serial.println("Motor1: " + String(servo1.read()));
    // delay(5000);

    // long randNumber2;
    // randNumber2 = random(40, 120);
    // servo2.write(randNumber2);

    // Serial.println("Motor2: " + String(servo2.read()));
    // delay(5000);

    // long randNumber3;
    // randNumber3 = random(40, 80);
    // servo3.write(randNumber3);

    // Serial.println("Motor3: " + String(servo3.read()));
    // delay(5000);

    // long randNumber4;
    // randNumber4 = random(40, 120);
    // servo4.attach(10);
    // delay(10);
    // servo4.write(randNumber4);
    // Serial.println("Motor4: " + String(servo4.read()));
    // delay(2000);
    // servo4.detach();

    // delay(100);

    moveRoboArm();

    EnviaAngloServosMQTT();
  }
}

// Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//         em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
// Parâmetros: nenhum
// Retorno: nenhum
void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (client.connect(ID_MQTT))
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

void EnviaAngloServosMQTT()
{  
  client.publish(TOPICO_PUBLISH, ("mt1|" + String(servo1.read())).c_str());
  client.publish(TOPICO_PUBLISH, ("mt2|" + String(servo2.read())).c_str());
  client.publish(TOPICO_PUBLISH, ("mt3|" + String(servo3.read())).c_str());
  client.publish(TOPICO_PUBLISH, ("mt4|" + String(servo4.read())).c_str());

  Serial.println("*--------//--------*");
  Serial.println("Motor1: " + String(servo1.read()));
  Serial.println("Motor2: " + String(servo2.read()));
  Serial.println("Motor3: " + String(servo3.read()));
  Serial.println("Motor4: " + String(servo4.read()));
  Serial.println("*--------//--------*");

  delay(100);
}

void readMQTT()
{
  if (client.connect(ID_MQTT))
  {
    Serial.print(client.state());
    Serial.println(" readMQTT");

    //client.subscribe(TOPICO_SUBSCRIBE);
    client.loop();
    delay(1000);
  }
}

void mqttLoop()
{
  // Verifica a conexão MQTT e reconecta-se, se necessário
  if (!client.connected())
  {
    reconnectMQTT();
  }
}

void moveRoboArm() {
  // Mova o braço para pegar o objeto
  servo1.write(45);
  delay(3000);

  EnviaAngloServosMQTT();

  // Feche a garra
  servo2.write(135);
  delay(3000);

  EnviaAngloServosMQTT();

  // Levante o objeto
  servo4.attach(10);
  servo4.write(135);
  delay(3000);
  servo4.detach();

  EnviaAngloServosMQTT();

  // Vire para a esquerda
  servo3.write(45);
  delay(3000);

  EnviaAngloServosMQTT();

  // Abaixe o objeto
  servo4.attach(10);
  servo4.write(50);
  delay(3000);
  servo4.detach();

  EnviaAngloServosMQTT();

  // Abra a garra para soltar o objeto
  servo2.write(90);
  delay(3000);

  EnviaAngloServosMQTT();

  // Feche a garra
  servo2.write(135);
  delay(3000);

  EnviaAngloServosMQTT();

  // Levante o objeto novamente
  servo4.attach(10);
  servo4.write(135);
  delay(3000);
  servo4.detach();

  EnviaAngloServosMQTT();

  // Vire para a direita (ajuste o ângulo conforme necessário)
  servo3.write(135);
  delay(3000);

  EnviaAngloServosMQTT();

  // Abaixe o objeto
  servo4.attach(10);
  servo4.write(50);
  delay(3000);
  servo4.detach();

  EnviaAngloServosMQTT();

  // Feche a garra para pegar o objeto novamente
  servo2.write(135);
  delay(3000);

  EnviaAngloServosMQTT();

  // Retorne à posição inicial
  /*servo1.write(50);
  delay(1000);
  //servo2.write(90);
  delay(1500);
  servo3.write(90);
  delay(1500);
  servo4.attach(10);
  servo4.write(90);
  delay(2000);
  servo4.detach();*/
}

void loop()
{

  // Execute a thread MQTT
  mqttThread.run();
  mqttThreadRead.run();

  //EnviaAngloServosMQTT();
}