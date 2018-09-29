#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int trigPin = 15;  //D4
const int echoPin = 0;  //D3

#define LED D4

// update these with values suitable for your network.
const char ssid[] = "Vodafone-5AA5";
const char password[] = "76767174";
const char* mqtt_server = "192.168.8.101";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
long duration;
int distance;
boolean somethingPassed = false;

// connecting to wifi and serialling iut the info
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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// printing the arrived messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  // if ((char)payload[0] == '1') {
  //   digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //   // but actually the LED is on; this is because
  //   // it is acive low on the ESP-01)
  // } else {
  //   digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  // }

}

// reconnecting in case of a lost connection to the MQTT server
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
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

void setup() {
  // pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(LED, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  // Might need to be 9600
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  
  if (now - lastMsg > 300) {

    // reading the echoPin, returning the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // calculating the distance
    distance= duration*0.034/2;

    if (distance < 200 && !somethingPassed) {
      somethingPassed = true;
      client.publish("sensor1", "in");
      Serial.println("In " + String(distance) + "," + somethingPassed);
      digitalWrite(LED, HIGH);  
    }

    if (distance >= 200 && somethingPassed) {
      somethingPassed = false;  
      Serial.println("Out " + String(distance) + "," + somethingPassed);
      digitalWrite(LED, LOW);
    }

    lastMsg = now; 
  }
}




