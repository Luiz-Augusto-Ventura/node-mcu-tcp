#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP udp;//Objeto da classe UDP.

void connect();//Cabeçalho do método que faz conexão com o servidor
void send();//Cabeçalho do método que envia dados
void listen();//Cabeçalho do método que recebe dados

const char* ssid = "Server";//Nome da rede 
const char* password = "12345678";//Senha da rede

IPAddress staticIP(192, 168, 4, 3); // Configura um IP estático 
IPAddress gateway(192, 168, 4, 1);// Rota default
IPAddress subnet(255, 255, 255, 0);// Máscara do IP 

long x;//Variavel a ser enviada.
String req;
const int LM35 = A0;//Pino sensor
float Temperatura = 0;
const int LED = D7;//Pino LED

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  WiFi.mode(WIFI_STA);//Configura o dispositivo atual como ESTAÇÃO
  WiFi.config(staticIP, gateway, subnet);//Configura o IP do Cliente
  WiFi.begin(ssid, password);//Conecta ao Servidor

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conexao estabelecida com sucesso");
  Serial.println(WiFi.localIP());
  if (udp.begin(554)) //Inicializa a recepçao de pacotes UDP através da porta 555
  {
    Serial.println("Porta Cliente pronta");
  }
}

void loop()
{
  connect();//Conecta-se à rede
  send();//Envia dados 
  delay(500);
  listen();//Escuta o meio buscando por dados 
}

void connect()//Sub-rotina para verificar a conexao 
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);//Conecta à rede usando seu nome e senha 
    Serial.println("Tentando estabelecer conexao");
    delay(2000);
  }
}

void send()//Sub-rotina para o envio de dados 
{
  if (WiFi.status() == WL_CONNECTED)//Envio de dados só acontece somente se houver conexão
  {
    Temperatura = analogRead(LM35);//Lê do sensor de temperatura
    Temperatura = (3.3 * Temperatura * 100.0) / 1024;//Cálculo responsável por transformar o valor recebido pelo ADC em unidade de temperatura 
    udp.beginPacket(gateway, 555);//Inicializa um pacote para transmissao ao IP(cliente) através da porta 554.
    udp.println(Temperatura);//Inclui a temperatura lida ao pacote
    udp.endPacket();//Finaliza o pacote e envia.
  }
  else//Caso não contectado
  {
    Serial.println("Sem conexao");
  }
}
void listen()//Sub-rotina que verifica se há pacotes UDPs a serem lidos do Servidor.
{
  Serial.println(".");
  if (udp.parsePacket() > 0)//Se 1, há pacotes a serem lidos
  {
    req = "";//String para receber informações
    while (udp.available() > 0)//Enquanto houver dados a serem lidos
    {
      char z = udp.read();//Recebe o byte lido
      req += z;//Adiciona o char à string
    }
    float aux = req.toFloat();//Representa a temperatura lida do Servidor
    if (( ((Temperatura - aux) > -2) && ((Temperatura - aux) < 2)  ) || ( (Temperatura - aux) >= 2))
    {
      digitalWrite(LED, HIGH);
    }
    else
    {
      digitalWrite(LED, LOW);
    }
    //Prints das informações receidas no monitor serial
    Serial.println("==============================");
    Serial.print("Temperatura do Cliente : ");
    Serial.println(Temperatura);
    Serial.print("Temperatura do Servidor: ");
    Serial.println(req.toFloat());
    Serial.println("==============================\n");
  }
}
