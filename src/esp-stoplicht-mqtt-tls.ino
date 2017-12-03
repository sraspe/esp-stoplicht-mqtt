#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "config.h"


int green=0;
int red=2;
long lastMsg = 0;
long lastBlink =0;
char msg[50];
int value = 0;
int lampStatus;
int stoplichtStatus;


// WiFiFlientSecure for SSL/TLS support
WiFiClientSecure client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, mqtt_server, mqtt_port, mqtt_username, mqtt_password);


/****************************** Feeds ***************************************/

// Subscribe to the feed called 'stoplicht' for getting the stoplicht state.
Adafruit_MQTT_Subscribe stoplicht = Adafruit_MQTT_Subscribe(&mqtt,"stoplicht");

// Setup the feed called 'status' for publishing the status of the stoplicht.
Adafruit_MQTT_Publish status = Adafruit_MQTT_Publish(&mqtt,"stoplicht/status");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();
void verifyFingerprint();

void setup() {
  pinMode(0, OUTPUT);     // Initialize PIN2 as output
  pinMode(2, OUTPUT);     // Initialize PIN2 as output
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit IO MQTTS (SSL/TLS) Example"));

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

  // check the fingerprint of io.adafruit.com's SSL cert
  verifyFingerprint();
  mqtt.subscribe(&stoplicht);

}


void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected). 
  if (!mqtt.connected()) {
    MQTT_connect();
  }
  
  //Check Subscriptions
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    // Check if its the stoplicht
    if (subscription == &stoplicht) {
      Serial.print(F("Stoplicht updated to: "));
      Serial.println((char *)stoplicht.lastread);
      lampStatus=int(stoplicht.lastread[0])-48;
      
    }
  }

  //Set light
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
    digitalWrite(red, LOW);
    delay(500);
    digitalWrite(red, HIGH);
    lampStatus=7;
  }
  if (lampStatus==5){
    blink(green,500);
    digitalWrite(red, HIGH); 
  }  
  if (lampStatus==6){
    party(450);
  }  
  if (lampStatus==7){
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH); 
  }
  if (lampStatus==8){
    digitalWrite(green, LOW);
    digitalWrite(red, LOW); 
  }

  
  //Publish Status
  long now_status = millis();
  if (now_status - lastMsg > 2000) {
    lastMsg = now_status;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.println(lampStatus);
    Serial.println(msg);
    status.publish(msg);
  }
}


void verifyFingerprint() {
  const char* host = mqtt_server;

  Serial.print("Connecting to ");
  Serial.println(host);

  if (! client.connect(host, mqtt_port)) {
    Serial.println("Connection failed. Halting execution.");
    while(1);
  }

  if (client.verify(mqtt_server_fingerprint, host)) {
    Serial.println("Connection secure.");
  } else {
    Serial.println("Connection insecure! Halting execution.");
    while(1);
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
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

void party(int duration ){
  long now = millis();
  if(now-lastBlink > duration && digitalRead(red)==HIGH){
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
    lastBlink=now;
  }
  if(now-lastBlink > duration && digitalRead(red)==LOW){
    digitalWrite(red,HIGH);
    digitalWrite(green, LOW);
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
