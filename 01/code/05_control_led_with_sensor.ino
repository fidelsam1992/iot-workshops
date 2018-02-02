#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>

// WiFi parameters to be configured
const char* ssid = "YOUR WIFI NETWORK NAME";
const char* password = "YOUR WIFI PASSWARD";

void setup(void)
{ 
  Serial.begin(9600);
  // Connect to WiFi
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());

}

void loop() {
  HTTPClient http;
  int sensorValue = analogRead(A0);

  if (sensorValue > THRESHOLD){
    //Led On
    http.begin("http://ip-address/LED=ON");
    int httpCode = http.GET();
    String payload = http.getString();
    Serial.println(payload);
  }else{
    //Led Off
    http.begin("http://ip-address/LED=OFF");
    int httpCode = http.GET();
    String payload = http.getString();
    Serial.println(payload);
  }
  
  http.end();
  delay(500);
}
