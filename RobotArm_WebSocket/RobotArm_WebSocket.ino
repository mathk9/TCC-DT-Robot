#include <Ethernet.h> //Carregar a biblioteca Ethernet
#include <EthernetUdp.h> //Carregar a biblioteca Udp
#include <SPI.h> //Carregar a biblioteca SPI
#include <Servo.h>

#include "Wire.h" //Importar a biblioteca Wire

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}; //Atribuir o endereço MAC
IPAddress ip(192, 168, 15, 102); //Atribuir o endereço IP
unsigned int localPort = 80; //Atribuir uma porta para comunicação

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //Dimensionar um array de caracteres para armazenar nosso pacote de dados
String datReq; //String para armazenar nossos dados

int packetSize; //Tamanho do pacote

EthernetUDP Udp; //Criar um objeto UDP

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup() {
  servo1.attach(9);
  servo2.attach(11);
  servo3.attach(8);
  servo4.attach(10);

  /* base.attach(8);
    braco.attach(9);
    garra.attach(11);
    cotovelo.attach(10); */

  Serial.begin(9600); //Inicializar a porta serial
  Ethernet.begin(mac, ip); //Inicializar a Ethernet
  Udp.begin(localPort); //Inicializar o Udp
  delay(1500); //Atraso

}

void loop() {

  packetSize = Udp.parsePacket(); //Ler o tamanho do pacote

  if (packetSize > 0) { //Se o tamanho do pacote for >0, isso significa que alguém enviou uma solicitação
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); //Ler a solicitação de dados
    String datReq(packetBuffer); //Converter o array de caracteres packetBuffer em uma string chamada datReq

    Serial.println("datReq: " + datReq);
    // Processar o comando HTTP do servidor Flask
    processRequest(datReq);

  }
  memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE); //Limpar o array packetBuffer
}

void processRequest(String request) {
  // Extrair o ID do servo e o ângulo da solicitação
  int servoId = extractServoId(request);
  int angle = extractAngle(request);

  Serial.println("angle: " + String(angle));
  Serial.println("servoId: " + String(servoId));

  // Mover o servo conforme necessário
  switch (servoId) {
    case 1:
      moveServo(servo1, angle);
      break;
    case 2:
      moveServo(servo2, angle);
      break;
    case 3:
      moveServo(servo3, angle);
      break;
    case 4:
      moveServo(servo4, angle);
      break;
    default:
      break;
  }
}

int extractServoId(String request) {
  int idIndex = request.indexOf("servo_id=");
  if (idIndex != -1) {
    int endIndex = request.indexOf('&', idIndex);
    if (endIndex != -1) {
      String idString = request.substring(idIndex + 9, endIndex);
      return idString.toInt();
    }
  }
  return -1; // Retornar -1 se não encontrar o ID do servo na solicitação
}

int extractAngle(String request) {
  int angleIndex = request.indexOf("angle=");
  if (angleIndex != -1) {
    int endIndex = request.indexOf(' ', angleIndex);
    if (endIndex != -1) {
      String angleString = request.substring(angleIndex + 6, endIndex);
      return angleString.toInt();
    }
  }
  return -1; // Retornar -1 se não encontrar o ângulo na solicitação
}

void moveServo(Servo servo, int angle) {
  delay(5);

  // Limitar o ângulo entre 0 e 180
  angle = constrain(angle, 0, 180);

  // Mover o servo para o ângulo desejado
  servo.write(angle);

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //Inicializar o envio do pacote
  Udp.print(String(servo1.read()) + ';' + String(servo2.read()) + ';' + String(servo3.read()) + ';' + String(servo4.read())); //Enviar os dados de temperatura
  Udp.endPacket(); //Finalizar o pacote
}
