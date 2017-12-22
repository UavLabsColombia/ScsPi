/*
 Conexion de modulo Wi-Fi ESP8266 Con servidor del protocolo MQTT example

Este codigo, es una modificacion al ejemplo de conexion con el servidor MQTT de los ejemplos de arduino 
el cual utiliza la librerias ESP8266Wifi y PubSubclient
fue modificado y adaptado para el control de nuestro proceso, se conecta a un servidor de MQTT el cual se encuentra en
una red alcanzable via TCP/IP

Funcionamiento: 
- AL conectarse con el servidor MQTT, imprime el mensaje "Hola mundo" indicando que se ha conectado correctamente, esto lo realiza con el topic
"outopic", este hola mundo es utilizado sobre Node-Red para detectar que la planta se ha conectado con el servidor de aplicacion
- Se suscribe a un topic "inTopic" por el cual recibe los eventos de desde el mismo servidor MQTT
- Si el primer caracter del topic de entrada es un 1, enciende el led integrado sobre el mismo ESP8266
- Este se reconecta al servidor si en algun momento falla la comunicacion

Envia y recibe datos por el puerto serial el cual van y vienen del microcontrolador PIC18f4550

Para instalar la board ESP8266 sobre arduino 1.6.4 o mayor agregue el siguiente al administrador de Boards de arduino sobre
Archivo, Preferencias, Administrador de boards adicionales 
http://arduino.esp8266.com/stable/package_esp8266com_index.json
Despues habra "Herramientas > Board > Administrador de Boards", busque e instale el paquete ESP8266
Para seleecionar el modulo, clic en Tool > Board
*/

//Librerias:
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

// ACtualizar estos valores con los datos de la red Wifi y el servidor de aplicacion MQTT

const char* ssid = "SSIDDeLaRed";
const char* password = "ClaveDeLaRedWifi";
const char* mqtt_server = "DireccionIPoNombredeDominiodelServidor";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//Inicia el serial, el wifi y la conexion con el servidor mqtt por el puerto 1883
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Inicializa el BUILTIN_LED pin Como una salida
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 500) {
    lastMsg = now;
    ++value;
    char myArray[5];
    Serial.readBytes(myArray,6);
    //snprintf (msg, 75, "Hello", value);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    //Serial.print("Mi array " + (char)myArray[0]);
    client.publish("outTopic", myArray);
  }
// no dejar linea en blanco al final del codigo 
}
