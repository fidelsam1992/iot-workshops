#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <StackArray.h>

// Update these with values suitable for your network.
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* mqtt_server = "MQTT_SERVER";

WiFiClient espClient;
PubSubClient client(espClient);

int counter = 0;
long lastTimeChecked;
const long interval = 4000;
bool sensorFlags[5] = {false, false, false, false, false};

// connecting to wifi and serialling iut the info
void setup_wifi() {
  delay(10);
  We start by connecting to a WiFi network
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

// handling the logic for the messages arrived to inc/dec the total counter
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();

   if (millis() - lastTimeChecked > interval) {
     sensorFlags[1] = false;
     sensorFlags[2] = false;
     sensorFlags[3] = false;
     sensorFlags[4] = false;
   }
   
   if (!sensorFlags[1] && !sensorFlags[2] && strcmp(topic, "sensor1") == 0) {
      sensorFlags[1] = true;
      lastTimeChecked = millis();
   } else if ((millis() - lastTimeChecked < interval) && sensorFlags[2] && strcmp(topic, "sensor1") == 0) {
      counter--;
      long value = 
      Serial.println(millis() - lastTimeChecked);
      sensorFlags[2] = false; 
   } else if (!sensorFlags[1] && !sensorFlags[2] && strcmp(topic, "sensor2") == 0) {
      sensorFlags[2] = true;
      lastTimeChecked = millis();
   } else if ((millis() - lastTimeChecked < interval) && sensorFlags[1] && strcmp(topic, "sensor2") == 0) {
      counter++;
      sensorFlags[1] = false; 
   }

   if (!sensorFlags[3] && !sensorFlags[4] && strcmp(topic, "sensor3") == 0) {
      sensorFlags[3] = true;
      lastTimeChecked = millis();
   } else if ((millis() - lastTimeChecked < interval) && sensorFlags[4] && strcmp(topic, "sensor3") == 0) {
      counter++;
      sensorFlags[4] = false;
   } else if (!sensorFlags[3] && !sensorFlags[4] && strcmp(topic, "sensor4") == 0) {
      sensorFlags[4] = true;
      lastTimeChecked = millis();
   } else if ((millis() - lastTimeChecked < interval) && sensorFlags[3] && strcmp(topic, "sensor4") == 0) {
      counter--;
      sensorFlags[3] = false; 
   }

   Serial.println("Counter so far:");
   Serial.println(counter);
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
      // ... and resubscribe
      client.subscribe("sensor1");
      client.subscribe("sensor2");
      client.subscribe("sensor3");
      client.subscribe("sensor4");
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
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
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
  delay(1000);
}