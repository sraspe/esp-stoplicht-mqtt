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

// Update these with values suitable for your network.
#include <config.h>

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String lampStatus;
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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    lampStatus="groen";
    digitalWrite(2, LOW);   // Turn the LED on (Note that LOW is the voltage level
    digitalWrite(0, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else if((char)payload[0] == '0') {
    lampStatus="rood";
    digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(0, LOW);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    lampStatus="Online";
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

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    //Serial.print("status: ", "operational ");
    Serial.println(msg);
    client.publish("stoplicht/status", msg);
  }
}
