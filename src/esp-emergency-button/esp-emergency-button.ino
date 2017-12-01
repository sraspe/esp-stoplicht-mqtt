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
char msg[50];
long lastMsg = 0;
int value = 0;
int lampStatus;
int stoplichtStatus;



void setup() {
  pinMode(0, INPUT);     // Initialize PIN0 as input
  pinMode(2, INPUT);     // Initialize PIN2 as input
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
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


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("noodknop", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("stoplicht/status", "online");
      // ... and resubscribe
      client.subscribe("noodknop");
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
  int knop = digitalRead(0);
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    if(knop == 1){
      snprintf (msg, 75, "%ld",knop);
      client.publish("stoplicht", msg);
      Serial.println(msg);    
    }


  }

}





