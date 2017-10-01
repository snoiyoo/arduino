#include <DHT.h>


#include <ESP8266WiFi.h>
#include "DHT.h"
#include <MicroGear.h>
#include <Wire.h>


#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
// constants won't change. They're used here to
// set pin numbers:
//freeboard
//#define DHTPIN D1 // what digital pin we're connected to
//#define DHTTYPE DHT11 // DHT 11
#define APPID "NetpieFreeBoard"
#define KEY "QLnu3m40Chjy1rW"
#define SECRET "rEKB8LlJZ02G2B3VfoFKbNjiS"
#define ALIAS "NodeMCU1"
#define Subscribe_Topic "/LEDcontrol"
#define Publish_Topic "/Node1Data"
#define retain false
WiFiClient client;
AuthClient *authclient;
MicroGear microgear(client);

//freeboard
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
#define DHTPIN D4// what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
const char ssid[] = "Senate-Smartphone";
const char password[] = "admin123";
const char *host = "maker.ifttt.com";
const char *Maker_Event = "button_pressed";
const char *Your_Key = "dnLpfQ7AuGBBvMDiysSTYz";
void send_event(const char *event);
DHT dht(DHTPIN, DHTTYPE);
//freeboard

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
digitalWrite(D6, HIGH); // Turn ON Lamp
}
else if (stateStr == "0" || stateStr == "OFF")
{
digitalWrite(D6, LOW); // Turn OFF Lamp
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
if(digitalRead(D6) == LOW)
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
//freeboard


void setup()
{
  //OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  pinMode(D5,OUTPUT);
Serial.begin(115200);
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
/*********** Connect to WiFi network ***********/
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
delay(250);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected with Router");
Serial.print("IP = ");
Serial.println(WiFi.localIP());
pinMode(D0, OUTPUT);
pinMode(D5, INPUT_PULLUP);
digitalWrite(D0,HIGH);
dht.begin();


//freeboard

microgear.on(MESSAGE,onMsghandler);
microgear.on(PRESENT,onFoundgear);
microgear.on(ABSENT,onLostgear);
microgear.on(CONNECTED,onConnected);
Serial.begin(115200);
//dht.begin();
//Serial.println("Starting...");
//WiFi.begin(ssid, password);
//while (WiFi.status() != WL_CONNECTED)
//{
//delay(250);
//Serial.print(".");
//}
//Serial.println("WiFi connected");
//Serial.println("IP address: ");
//Serial.println(WiFi.localIP());
//uncomment the line below if you want to reset token -->
microgear.resetToken();
microgear.init(KEY,SECRET,ALIAS);
microgear.connect(APPID);
pinMode(D6,OUTPUT);
digitalWrite(D6,LOW); // Turn off LED
//freeboard

}
void loop()
{
// Wait a few seconds between measurements.
delay(2000);
// Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
// Read temperature as Fahrenheit (isFahrenheit = true)
float f = dht.readTemperature(true);
OLED.clearDisplay(); // clear display
OLED.setTextColor(WHITE); // set text color
OLED.setCursor(15,0);
OLED.setTextSize(2);
OLED.println("Read TEMP");
OLED.setCursor(15,35);
OLED.println("T = " + String(t));
OLED.display();
// Check if any reads failed and exit early (to try again).
if (isnan(h) || isnan(t) || isnan(f))
{
Serial.println("Failed to read from DHT sensor!");
return;
}
Serial.print("Humidity: ");
Serial.print(h);
Serial.print(" %\t");
Serial.print("Temperature: ");
Serial.print(t);
Serial.print(" *C ");
//Serial.print(f);
//Serial.print(" *F\t ");
//Serial.print(" Heat index: ");
//Serial.print(hic);
//Serial.print(" *C ");
//Serial.print(hif);
//Serial.println(" *F");

// Compute heat index in Fahrenheit (the default)
float hif = dht.computeHeatIndex(f, h);
// Compute heat index in Celsius (isFahreheit = false)
float hic = dht.computeHeatIndex(t, h, false);
int buttonState = digitalRead(D5);
Serial.print(buttonState);
if((t > 34) ||(buttonState == LOW))
{
Serial.print("Button is pressed");
Serial.print(t);
send_event(Maker_Event);
}

//delay(2500);

//freeboard

if(microgear.connected())
{
microgear.loop();
Serial.println("connect...");
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
if(digitalRead(D6) == HIGH)
{
str = "1," + String(h) + "," + String(t);
}
else
{
str = "0," + String(h) + "," + String(t);
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
//freeboard
}




void send_event(const char *event)
{
// set the LED on whle we are sending the event
digitalWrite(D6, LOW);
Serial.print("Connecting to ");
Serial.println(host);
// Use WiFiClient class to create TCP connections
WiFiClient client;
const int httpPort = 80;
if (!client.connect(host, httpPort))
{
Serial.println("Connection failed");
return;
}
// We now create a URI for the request
String url = "/trigger/";
url += event;
url += "/with/key/";
url += Your_Key;
Serial.print("Requesting URL: ");
Serial.println(url);
// This will send the request to the server
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"Connection: close\r\n\r\n");
// Read all the lines of the reply from server and print them to Serial,
// the connection will close when the server has sent all the data.
while(client.connected())
{
if(client.available())
{
String line = client.readStringUntil('\r');
Serial.print(line);
}
else
{
// No data yet, wait a bit
delay(50);
};
}
// All done
Serial.println();

Serial.println("closing connection");
client.stop();
// Finished sending the message, turn off the LED
digitalWrite(D6, HIGH);
//delay(900000);
}
