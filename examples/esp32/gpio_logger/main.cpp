#include <Arduino.h>

const int inputPins[8] = {4,5,6,7,10,11,12,13};

void setup() {
  Serial.begin(115200);
  for(int i=0;i<8;i++){
    pinMode(inputPins[i], INPUT_PULLUP);
  }
  Serial.println("Signal Mitaro v0.0.1");
}

void loop() {
  static uint8_t lastState[8] = {0};

  for(int i=0;i<8;i++){
    uint8_t current = digitalRead(inputPins[i]);

    if(current != lastState[i]){
      lastState[i] = current;

      Serial.print("{\"ch\":");
      Serial.print(i);
      Serial.print(",\"level\":");
      Serial.print(current);
      Serial.println("}");
    }
  }

  delay(5);
}
