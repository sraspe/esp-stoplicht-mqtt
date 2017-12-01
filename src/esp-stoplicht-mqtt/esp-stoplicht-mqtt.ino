#include <Arduino.h>

/*
Basic MQTT stoplicht
PIN0 is green
PIN2 is red
// Status table
State - desc - Green - Red
0 -   off    - Off -Off
1 - Critical - Off - Blink Continuously
2 - Urgent   - Off - ON
3 - Warning  - On - Blink Continously
4 - Notify   - On - Blink 3x
5 - Prepare  - Blink Continuously - Off
6 - Party    - Blink Continously - Blink Continously
7 - OK       -On - Off


*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// get the params from a separate config file
#include "config.h"

WiFiClient espClient;
PubSubClient client(espClient);
int green=0;
int red=2;
long lastMsg = 0;
long lastBlink =0;
char msg[50];
int value = 0;
int lampStatus;
int stoplichtStatus;



void setup() {
  pinMode(0, OUTPUT);     // Initialize PIN2 as output
  pinMode(2, OUTPUT);     // Initialize PIN2 as output
  Serial.begin(115200);
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
  Serial.print("Message arrived on topic [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //Set status 0
  if((char)payload[0] == '0') {
    lampStatus=0;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);
  }
  if ((char)payload[0] == '1') {
    lampStatus=1;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);
  } 
if((char)payload[0] == '2') {
    lampStatus=2;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);    
  }  
if((char)payload[0] == '3') {
    lampStatus=3;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);
  }
  if((char)payload[0] == '4') {
    lampStatus=4;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);
   }
  if((char)payload[0] == '5') {
    lampStatus=5;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);    
  }
  if((char)payload[0] == '6') {
    lampStatus=6;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);
  }
  if((char)payload[0] == '7') {
    lampStatus=7;
    Serial.print("Set lampstatus to :");
    Serial.println(lampStatus);
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
      client.publish("stoplicht/status", "online");
      // ... and resubscribe
      client.subscribe("stoplicht");
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
  if (lampStatus==0){
    //alles uit
    digitalWrite(green, HIGH); 
    digitalWrite(red, HIGH);
  }
  if (lampStatus==1){
    digitalWrite(green, HIGH);
    blink(red,500);
  }
  if (lampStatus==2){
    digitalWrite(green, HIGH); 
    digitalWrite(red, LOW);
  }
  if (lampStatus==3){
    digitalWrite(green, LOW); 
    blink(red,500);
  }
  if (lampStatus==4){
    digitalWrite(green, LOW); 
    digitalWrite(red, LOW);
    delay(500);
    digitalWrite(red, HIGH);
    delay(500);
    digitalWrite(red, LOW);
    delay(500);
    digitalWrite(red, HIGH);
    delay(500);
    digitalWrite(red, HIGH);
    delay(500);
    digitalWrite(red, LOW);
  }
  if (lampStatus==5){
    blink(green,500);
    digitalWrite(red, HIGH); 
  }  
  if (lampStatus==6){
    blink(green,450);
    blink(red,550);
  }  
  if (lampStatus==7){
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH); 
  }
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.println(lampStatus);
    Serial.println(msg);
    client.publish("stoplicht/status", msg);
  }
}




void blink(int pin, int duration ){
  long now = millis();
  if(now-lastBlink > duration && digitalRead(pin)==HIGH){
    digitalWrite(pin, LOW);
  lastBlink=now;
  }
  if(now-lastBlink > duration && digitalRead(pin)==LOW){
    digitalWrite(pin,HIGH);
  lastBlink=now;
  }
}

/*
PIN0 is green
PIN2 is red
// Status table
State - desc - Green - Red
0 -   off    - Off -Off
1 - Critical - Off - Blink Continuously
2 - Urgent   - Off - ON
3 - Warning  - On - Blink Continously
4 - Notify   - On - Blink 3x then goto on
5 - Prepare  - Blink Continuously - Off
6 - Party    - Blink Continously - Blink Continously
7 - OK       -On - Off
8 - WTF       -on - on


*/


