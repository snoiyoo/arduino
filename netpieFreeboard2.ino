#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "DHT.h"
// constants won't change. They're used here to
// set pin numbers:

#define D0 16 // USER LED Wake
#define D1 5
#define D2 4
#define D3 0 // FLASH
#define D4 2 // TXD1
#define D5 14
#define D6 12
#define D7 13 // RXD2 Serial.swap(); after Serial.begin();.
#define D8 15 // TXD2 Serial.swap(); after Serial.begin();.
#define D9 3 // RXD0
#define D10 1 // TXD0
#define Lamp D5
#define DHTPIN D4 // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
#define APPID "NetpieFreeBoard2"
#define KEY "QjxvL5EciT7PHrg"
#define SECRET "iNBE9O7XfvpbITrI2ILfv6eV8"
#define ALIAS "NodeMCU1"
#define Subscribe_Topic "/LEDcontrol"
#define Publish_Topic "/Node1Data"
#define retain false
#define Ping_Pin D1 // Trigger Pin of Ultrasonic Sensor
#define Echo_Pin D2 // Echo Pin of Ultrasonic Sensor
unsigned long int duration;
double inches, cm;
WiFiClient client;
AuthClient *authclient;
MicroGear microgear(client);
DHT dht(DHTPIN, DHTTYPE);
const char* ssid = "noiyoo";
const char* password = "admin1234";
String str = "";
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen)
{
Serial.print("Incoming message --> ");
Serial.print(topic);
Serial.print(" : ");
char strState[msglen];
for (int i = 0; i < msglen; i++)
{
strState[i] = (char)msg[i];
Serial.print((char)msg[i]);
}
Serial.println();
String stateStr = String(strState).substring(0, msglen);
if(stateStr == "1" || stateStr == "ON")
{
digitalWrite(Lamp, HIGH); // Turn ON Lamp
}
else if (stateStr == "0" || stateStr == "OFF")
{
digitalWrite(Lamp, LOW); // Turn OFF Lamp
}
float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
// Check if any reads failed and exit early (to try again).
if (isnan(h) || isnan(t) || (h > 100) || (t > 100))
{
Serial.println("Failed to read from DHT sensor!");
}
else
{
if(digitalRead(Lamp) == LOW)
{
str = "1," + String(h) + "," + String(t);
microgear.publish(Publish_Topic, str, retain);
}
else
{
str = "0," + String(h) + "," + String(t);
microgear.publish(Publish_Topic, str, retain);
}
}
}
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen)
{
Serial.println("Connected to NETPIE...");
microgear.setAlias(ALIAS);
microgear.subscribe(Subscribe_Topic);
}
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen)
{
Serial.print("Found new member --> ");
for (int i=0; i<msglen; i++)
{
Serial.print((char)msg[i]);
}
Serial.println();
}
void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen)
{
Serial.print("Lost member --> ");
for (int i=0; i<msglen; i++)
{
Serial.print((char)msg[i]);
}
Serial.println();
}
void setup()
{
  pinMode(Ping_Pin, OUTPUT);
pinMode(Echo_Pin, INPUT);
/* Event listener */
microgear.on(MESSAGE,onMsghandler);
microgear.on(PRESENT,onFoundgear);
microgear.on(ABSENT,onLostgear);
microgear.on(CONNECTED,onConnected);
//Serial.begin(115200);
Serial.begin(9600);
dht.begin();
Serial.println("Starting...");
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
delay(250);
Serial.print(".");
}
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
//uncomment the line below if you want to reset token -->
microgear.resetToken();
microgear.init(KEY,SECRET,ALIAS);
microgear.connect(APPID);
pinMode(Lamp,OUTPUT);
digitalWrite(Lamp,LOW); // Turn off LED
}
void loop()
{
if(microgear.connected())
{
microgear.loop();
Serial.println("connect...");
float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
digitalWrite(Ping_Pin, HIGH);
delayMicroseconds(10);
digitalWrite(Ping_Pin, LOW);
duration = pulseIn(Echo_Pin, HIGH); // the length of the pulse (in microseconds) or 0 if no pulse started before the timeout (unsigned long)
inches = duration / 148.00;
cm = duration / 58.00;
Serial.print(inches);
Serial.print(" in, ");
Serial.print(cm);
Serial.print(" cm");
Serial.println();
delay(100);
// Check if any reads failed and exit early (to try again).
if (isnan(inches) || isnan(cm) || (inches > 100) || (cm > 100))
{
Serial.println("Failed to read from DHT sensor!");
}
else
{
if(digitalRead(Lamp) == HIGH)
{
str = "1," + String(inches) + "," + String(cm);
}
else
{
str = "0," + String(inches) + "," + String(cm);
}
}
microgear.publish(Publish_Topic, str, retain);
}
else
{
Serial.println("connection lost, reconnect...");
microgear.connect(APPID);
}
delay(250);
}
