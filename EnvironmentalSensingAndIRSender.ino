/*
* Projeto Smart LSDi - UFMA
* Monitoramento do Ambiente
* Autora: Amanda Almeida Cardoso
* Orientador: Prof. Dr. Francisco Silva e Silva
* Julho 2024
*
*
*
* O código abaixo utiliza sensores para ler  temperatura, 
* umidade e presença de gás no ambiente e envia essas informações
* para um módulo BLE (HM10). Também recebe do módulo BLE um comando 
* para alterar a temperatura de um ar condicionado através de um emissor IR.
* Os arrays de infravermelho são guardados na memória flash do Arduino.
*
* 
*
* Os arrays são específicos para um ar condicionado da marca Gree cujo protocolo do controle remoto é yay1f1. 
*
*
* Conexões: 
* HM10 - pinos 10,11 (RX,TX)
* IRSender - pino 3
* DH11 - pino 4
* Sensor de Gás - pino A0
* 
*
*
* Referências:
* https://www.analysir.com/blog/2016/04/11/sending-long-ac-signals-flash-irremote/
* https://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/
*/


#include <IRremote.hpp>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <Arduino.h>
#include "IrSignals.h"

#define TRUE 1
#define FALSE 0
#define KHZ 38           // Carrier frequency
#define ARRAY_SIZE 419   // Tamanho de todos os arrays
#define DHTPIN 4         // Pino de dados do sensor DHT
#define DHTTYPE DHT11


SoftwareSerial HM10(10, 11); // RX, TX
DHT dht(DHTPIN, DHTTYPE);


float temperature = 0;
float humidity = 0;

int sensorReading = 0;
int gasSensorPin = A0;
int safeValue = 300; 

int commandComplete = FALSE;
int desiredTemperature = 0;
String bleCommand = " ";


void setup() {

  Serial.begin(9600);
  delay(2000);
 
  HM10.begin(9600); // Inicializa a comunicação com o módulo HM-10
  dht.begin(); // Inicializa sensor de temperatura e umidade
  pinMode(gasSensorPin, INPUT); // Detector de fumaça 
  IrSender.begin(3); 

}

void loop() {
  readTemperature();
  readHumidity();
  readGasSensor();
  receiveCommand();
  
  if(commandComplete){
    sendTemperature(desiredTemperature);    
    commandComplete = FALSE;
  }

  delay(2000);

}


void sendTemperature(int desiredTemperature){
      
      switch(desiredTemperature) {
        case 0:             
            sendRAW_Flash(AC_irSignal0);
            break;
        case 16:
            sendRAW_Flash(AC_irSignal16);
            break;
        case 17:
            sendRAW_Flash(AC_irSignal17);
            break;
        case 18:
            sendRAW_Flash(AC_irSignal18);
            break;
        case 19:
            sendRAW_Flash(AC_irSignal19);
            break;
        case 20:
            sendRAW_Flash(AC_irSignal20);
            break;
        case 21:
            sendRAW_Flash(AC_irSignal21);
            break;
        case 22:
            sendRAW_Flash(AC_irSignal22);
            break;
        case 23:
            sendRAW_Flash(AC_irSignal23);
            break;
        case 24:
            sendRAW_Flash(AC_irSignal24);
            break;
        case 25:
            sendRAW_Flash(AC_irSignal25);
            break;
        case 26:
            sendRAW_Flash(AC_irSignal26);
            break;
        case 27:
            sendRAW_Flash(AC_irSignal27);
            break;
        case 28:
            sendRAW_Flash(AC_irSignal28);
            break;
        case 29:
            sendRAW_Flash(AC_irSignal29);
            break;
        case 30:
            sendRAW_Flash(AC_irSignal30);
            break;
        default:
            Serial.println("Valor inválido");
            break;

    }
}

void readTemperature() { 
    temperature = dht.readTemperature(); 
    if (isnan(temperature)){
        //Serial.println("Erro ao ler dados do sensor DHT!");
        return;
    }
    HM10.print(temperature);
    HM10.print(";");
}

void readHumidity() {
    humidity = dht.readHumidity();
    if (isnan(humidity)){
        //Serial.println("Erro ao ler dados do sensor DHT!");
        return;
    }
    HM10.print(humidity);
    HM10.print(";");

}

void readGasSensor() {
    sensorReading = analogRead(gasSensorPin);
    if(isnan(sensorReading)){
        //Serial.println("Erro ao ler dados do sensor de gás!");
        return;
    }
    if(sensorReading > safeValue){ 
        HM10.print("1"); // Gás detectado
        HM10.println(";");

    } else {
        HM10.print("0"); // Gás não detectado
        HM10.println(";");

    }
}

void receiveCommand() {
    if(HM10.available()) {
        char recvChar = HM10.read();

        // Se encontrar o marcador de finalização
        if(recvChar == ';') { 
            Serial.println("Dados recebidos:");
            Serial.println(bleCommand);
            commandComplete = TRUE;
            desiredTemperature  = atoi(bleCommand.c_str()); // Transforma o comando enviado pelo bluetooth de char para int
            // Limpa a variável para receber mais dados na próxima iteração
            bleCommand = "";
        } else {
            // Continua adicionando os dados à variável
            bleCommand += recvChar;
        }
    }
}

//Envia array IR guardado na memória Flash
void sendRAW_Flash(const unsigned int *arrayIR) {
    
    IrSender.enableIROut(KHZ);
    for(unsigned int i = 0; i < ARRAY_SIZE; i++) {
        if(i & 1) {
            IrSender.space(pgm_read_word_near(&arrayIR[i]));
        } else {
            IrSender.mark(pgm_read_word_near(&arrayIR[i]));
        }
    }
    IrSender.space(1);
}


