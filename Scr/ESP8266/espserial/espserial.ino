
/*
Conexion de modulo Wi-Fi ESP8266 Con servidor Mosquitto del protocolo MQTT (M2M)

Este codigo, es una modificacion al ejemplo de conexion con el servidor MQTT  de arduino 
el cual utiliza la librerias ESP8266Wifi y PubSubclient, fue modificado y adaptado para el control de nuestro proceso, se conecta a un servidor de MQTT el cual se encuentra en
una red alcanzable via TCP/IP

Funcionamiento: 
- AL conectarse con el servidor MQTT, imprime el mensaje "Hola mundo" indicando que se ha conectado correctamente, esto lo realiza con el topic
"outopic", este hola mundo es utilizado sobre Node-Red para detectar que la planta se ha conectado con el servidor de aplicacion
- Se suscribe a un topic "inTopic" por el cual recibe los eventos de desde el mismo servidor MQTT y son enviados por serial al pic18f2550
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

// FUncion que realiza la conexion a la red Wifi.
//Imprime por el serial un . mientras intenta ralizar la conexion con la red Wifi
// Una vez conectado imprime por serial los datos de la conexion realizada y la ip asignada. 
void setup_wifi() {

  delay(10);
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

//Funcion que recoge el evento de llegada recogido por el servidor mqtt, lo almacena en buffer y lo imprime por serial
void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  // Enciende el BUILTIN_LED si se recibe un 1 sobre el primer espacio del buffer de datos recibido
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Enciende el led con logica 0
    // 
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Apaga el led con logica 1
  }

}

void reconnect() {
  // Loop de reconexoin con el servidor MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Intento de conexion
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Una vez conectado, realiza una publicacion
      client.publish("outTopic", "hello world");
      // Se suscribe a un topic de entrada
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Espera 5 segundos antes de intentar reconectar de nuevo 
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
    // Buffer que recoge los datos leidos por el puerto serial y lo publica sobre el topic de salida. 
    char myArray[5];
    Serial.readBytes(myArray,6);
    client.publish("outTopic", myArray);
  }
// no dejar linea en blanco al final del codigo 
}
