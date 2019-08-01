#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
 
WiFiUDP udp;//Objeto da classe UDP.

void listen();//Cabeçalho do método que recebe dados
void send();//Cabeçalho do método que envia dados

const char* ssid = "Server";//Nome da rede
const char* password = "12345678";//Senha da rede

String req;//Variável que faz requisição de dados
long x;
const int LM35 = A0;//Pino sensor
float TempServidor = 0;
const int LED = D7;//Pino Led

IPAddress cliente(192, 168, 4, 3); //IP estático do cliente
IPAddress staticIP(192, 168, 4, 2); //IP estático do servidor
IPAddress gateway(192, 168, 4, 1);//Rota default
IPAddress subnet(255, 255, 255, 0);//Máscara de rede 
 
void setup()
{
   Serial.begin(115200);
   pinMode(LED, OUTPUT);
   WiFi.mode(WIFI_AP);//Define o dispositivo atual como Acess Point.
   WiFi.softAP(ssid, password, 2, 0);
   WiFi.config(staticIP, gateway, subnet);
   delay(2000);
   if(udp.begin(555))//Inicializa a recepçao de dados via protocolo UDP através da porta 555
   {
    Serial.println("Porta Servidor pronta");
   }
}
 
void loop()
{
   listen();//Escuta o meio buscando por dados .
   send();//Envia dados
   delay(500);
}
 
void listen()//Sub-rotina que verifica se há pacotes UDPs a serem lidos do Cliente.
{
   Serial.println(".");
   if (udp.parsePacket() > 0)//Se 1, Existem pacotes a serem lidos 
   {
       req = "";//String para receber informações
       while (udp.available() > 0)//Enquanto houver dados a serem lidos
       {
           char z = udp.read();//Recebe o byte lido
           req += z;//Adiciona o char à uma string
       }
       float aux = req.toFloat();//Representa a temperatura lida do Cliente
       if(( ((TempServidor - aux) > -2) && ((TempServidor - aux) < 2)  ) || ( (TempServidor - aux) >= 2))
       {
         digitalWrite(LED,HIGH);
       }
       else
       {
         digitalWrite(LED,LOW);
       }
        //Prints das informações receidas no monitor serial
        Serial.println("==============================");
        Serial.print("Temperatura do servidor: ");
        Serial.println(TempServidor);
        Serial.print("Temperatura do cliente : ");
        Serial.println(req.toFloat());
        Serial.println("==============================\n");
       }
}
void send()//Sub-rotina para o envio de dados
{
   TempServidor = analogRead(LM35);//Lê do sensor de temperatura 
   TempServidor = (3.3 * TempServidor * 100.0) / 1024;//Cálculo responsável por transformar o valor recebido pelo ADC em unidade de temperatura 
   udp.beginPacket(cliente, 554);//Inicializa um pacote para transmissao ao IP(cliente) através da porta 554.
   udp.println(TempServidor);//Inclusão da temperatura lida ao pacote.
   udp.endPacket();//Finaliza o pacote e envia.
}
