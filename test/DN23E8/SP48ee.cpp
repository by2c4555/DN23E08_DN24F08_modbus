#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.print("Serial 0 active");
 pinMode(13,OUTPUT);
 digitalWrite(13,LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    // Read the incoming byte
    char received = Serial.read();

    // Echo the received byte back to the sender
    Serial.print("Received: ");
    Serial.println(received);
  }
}
