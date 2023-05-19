#include <SPI.h>
#include <Ethernet.h>

// Insira abaixo um endereço MAC e um endereço IP para o seu controlador.
// O endereço IP dependerá da sua rede local:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 177);

// Inicializa a biblioteca do servidor Ethernet
// com o endereço IP e a porta desejada
// (a porta 80 é a padrão para HTTP):
EthernetServer server(80);

void setup() {
  // Você pode usar Ethernet.init(pino) para configurar o pino CS
  Ethernet.init(10); // A maioria dos shields Arduino
  //Ethernet.init(5); // Shield MKR ETH
  //Ethernet.init(0); // Teensy 2.0
  //Ethernet.init(20); // Teensy++ 2.0
  //Ethernet.init(15); // ESP8266 com Adafruit Featherwing Ethernet
  //Ethernet.init(33); // ESP32 com Adafruit Featherwing Ethernet

  // Abre a comunicação serial e aguarda a abertura da porta:
  Serial.begin(9600);
  while (!Serial) {
    ; // aguarda a conexão com a porta serial. Necessário apenas para a porta USB nativa
  }
  Serial.println("Exemplo de Servidor Web Ethernet");

  // Inicia a conexão Ethernet e o servidor:
  Ethernet.begin(mac, ip);

  // Verifica se há hardware Ethernet presente
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Shield Ethernet não encontrado. Desculpe, não é possível executar sem o hardware. :(");
    while (true) {
      delay(1); // não faz nada, não há motivo para continuar sem o hardware Ethernet
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("O cabo Ethernet não está conectado.");
  }

  // Inicia o servidor
  server.begin();
  Serial.print("O servidor está em ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // aguarda por clientes conectados
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Novo cliente");
    // uma requisição http termina com uma linha em branco
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // se você chegou ao final da linha (recebeu uma quebra de linha
        //) e a linha está em branco, a requisição http terminou,
        // então você pode enviar uma resposta
        if (c == '\n' && currentLineIsBlank) {
          // envia um cabeçalho de resposta http padrão
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); // a conexão será fechada após a conclusão da resposta
          client.println("Refresh: 5"); // atualiza a página automaticamente a cada 5 segundos
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // imprime o valor de cada pino de entrada analógica
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("Entrada analógica ");
            client.print(analogChannel);
            client.print(" é ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // você está começando uma nova linha
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // você recebeu um caractere na linha atual
          currentLineIsBlank = false;
        }
      }
    }
    // dá ao navegador tempo para receber os dados
    delay(1);
    // fecha a conexão:
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
