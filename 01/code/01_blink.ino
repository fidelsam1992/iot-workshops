# define LED D4 // Use built-in LED which connected to D4 pin or GPIO 2

void setup() {
  pinMode(LED, OUTPUT);     // Initialize the LED as an output
}

void loop() {
  digitalWrite(LED, HIGH);  // Turn the LED off because the LED is active low
  delay(1000);            // Wait a second
  digitalWrite(LED, LOW);   // Turn the LED on because the LED is active low
  delay(1000);              // Wait a second
}