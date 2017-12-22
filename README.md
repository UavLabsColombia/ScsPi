"SCSPi" Sistema de Control Escalable Basado en Raspberry Pi


Al diseñar un sistema de control automático, nuestra mayor satisfacción es el saber que funciona de manera adecuada y sin percances sobre su ejecución, a raíz de mejorar la estabilidad en los funcionamientos de los sistemas de control automático, nace la necesidad de un constante monitoreo del sistema, este monitoreo consiste en darse cuenta si el proceso se está ejecutando de manera correcta, ya sea confirmando el estado del sistema personalmente o analizando su estatus por medio de un panel que permita su control ante algún evento, por esto vemos la necesidad de implementar un SCS “Scalable Control System o Sistema de Control Escalable” que permite amoldarse a cualquier entorno de control automático en producción y analizar en RT su funcionamiento. 

Objetivo.
Implementar un Sistema de Control Escalable con tecnologías Open Source basado en la arquitectura de los DCS, que permita interactuar en tiempo real con el sistema controlado.

Objetivos Generales. 
* Simular un entorno o proceso de control automático el cual estará monitoreado y controlado, este entorno estará adaptado a pruebas de error en la ejecución del sistema para probar el funcionamiento del SCS. 
* Entender los diferentes tipos de protocolos permitidos para controlar dispositivos “IoT” y sensores
* Aplicar las técnicas necesarias para linealizar los datos de los sensores a monitorear
* Analizar las herramientas disponibles para diseñar el Dashboard
* Interfazar todos los dispositivos que componen el SCS
* Diseñar el Frontend o Panel de control
* Publicar el SCS en la nube monitoreando el sistema. 

Planta o Sistema a controlar:
Elevador clasificador para paquetes
El proceso se inicia con el transporte de uno de los paquetes a la báscula; una vez clasificado el paquete en la báscula, se encenderá una luz indicadora del tipo de paquete (luz 1 será paquete grande y luz 2 será paquete pequeño). A continuación, el paquete es transportado por la cinta 1 hasta el plano elevador. El cilindro C eleva los paquetes. Acto seguido los paquetes son clasificados; los paquetes pequeños son colocados en la cinta 2 por el cilindro A, y los paquetes grandes son colocados en la cinta 3 por el cilindro B. el cilindro elevador C se recupera sólo cuando los cilindros A y B llegan a la posición final. El sistema contará con botones locales de stop, start y reinicio; los cuales también aparecerán en la HMI, en donde se mostrará las fases del proceso y llevará la cuenta de los paquetes grandes y pequeños.



Tecnologias Utilizadas 

Raspberry Pi: 
Sistema embebido en un solo Chip con capacidades de MultiProceso https://www.raspberrypi.org/, Documentacion https://www.raspberrypi.org/documentation/

ESP8266: 
Modulo de domunicacion Inalambrica 802.11 b/g/n, DataSheet https://cdn-shop.adafruit.com/product-files/2471/0A-ESP8266__Datasheet__EN_v4.3.pdf

PIC18F2550:
MicroControlador de la familia MicroChip, DataSheet http://ww1.microchip.com/downloads/en/DeviceDoc/39632e.pdf


Raspbian: 
Sistema Operativo Basado en Tecnologias GNU/Linux https://www.raspbian.org/, Documentacion https://www.raspbian.org/RaspbianDocumentation


Node-RED: 
Herramienta de programacion basada en Flows, editor de flujos con tecnologias basadas en APIs para conectar dispositivos de Hardware del Internet de las Cosas "IoT" https://nodered.org/, Documentacion https://nodered.org/docs/

Mosquitto:
Servidor de aplicacion intermediario para los mensajes bajo el protocolo MQTT https://mosquitto.org/, Documentacion https://mosquitto.org/documentation/

MQTT:
Protocolo de comunicacion Maquina a Maquina (M2M) http://mqtt.org/, Dodumentacion http://mqtt.org/documentation, Ejemplo de uso https://github.com/knolleary/pubsubclient/tree/master/examples/mqtt_esp8266

Arduino: IDE de programacion para Chips basados en la arquitectura Atmel AVR https://www.arduino.cc/, Documentacion https://www.arduino.cc/en/Guide/HomePage

Referencia a la programacion del chip ESP8266 con Arduino http://arduino-esp8266.readthedocs.io/en/latest/


Libreria ESP8266Wifi https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi

Libreria  PubSubClient, Cliente MQTT para el chip ESP8266 https://github.com/knolleary/pubsubclient

Pic C compiler: IDE utilizado para programar Chips de la arquitectura MicroChip 
http://www.ccsinfo.com/, Documentacion https://www.ccsinfo.com/downloads/ccs_c_manual.pdf


Java Script: Lenguaje de Programacion Orientado a Objetos utilizado para programar sobre Node-Red https://www.javascript.com/ Documentacion https://developer.mozilla.org/es/docs/Web/JavaScript/Referencia




