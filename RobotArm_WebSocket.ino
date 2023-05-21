#include <Ethernet.h> //Load Ethernet Library
#include <EthernetUdp.h> //Load the Udp Library
#include <SPI.h> //Load SPI Library
#include <Servo.h>

#include "Wire.h" //imports the wire library

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}; //Assign mac address
IPAddress ip(192, 168, 0, 243); //Assign the IP Adress
unsigned int localPort = 80; // Assign a port to talk over
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //dimensian a char array to hold our data packet
String datReq; //String for our data
int packetSize; //Size of the packet
EthernetUDP Udp; // Create a UDP Object
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup() {
  servo1.attach(9);
  servo2.attach(11);
  servo3.attach(8);
  servo4.attach(10);

  /*base.attach(8);
  braco.attach(9);
  garra.attach(11);
  cotovelo.attach(10);*/

  Serial.begin(9600); //Initialize Serial Port
  Ethernet.begin( mac, ip); //Inialize the Ethernet
  Udp.begin(localPort); //Initialize Udp
  delay(1500); //delay

}

void loop() {

  packetSize = Udp.parsePacket(); //Reads the packet size

  if (packetSize > 0) { //if packetSize is >0, that means someone has sent a request

    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); //Read the data request
    String datReq(packetBuffer); //Convert char array packetBuffer into a string called datReq

    Serial.println("datReq: " + datReq);
    // Processa o comando HTTP do servidor Flask
    processRequest(datReq);

  }
  memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE); //clear out the packetBuffer array
}

void processRequest(String request) {
  // Extrai o ID do servo e o ângulo da solicitação
  int servoId = extractServoId(request);
  int angle = extractAngle(request);

  Serial.println("angle: " + String(angle));
  Serial.println("servoId: " + String(servoId));

  // Move o servo conforme necessário
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
  return -1;  // Retornar -1 se não encontrar o ID do servo na solicitação
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
  return -1;  // Retornar -1 se não encontrar o ângulo na solicitação
}

void moveServo(Servo servo, int angle) {
  // Limita o ângulo entre 0 e 180
  angle = constrain(angle, 0, 180);

  // Move o servo para o ângulo desejado
  servo.write(angle);

  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //Initialize packet send
  Udp.print(angle); //Send the temperature data
  Udp.endPacket(); //End the packet
}
