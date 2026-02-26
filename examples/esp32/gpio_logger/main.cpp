#include <Arduino.h>

// --- Signal Mitaro v0.0.1 ---
// Minimal GPIO -> Event(JSON) logger that follows schemas/event_v1.json

// Adjust pins for your ESP32-S3 wiring
const int inputPins[8] = {4, 5, 6, 7, 10, 11, 12, 13};

// Simple static identifiers (later: config / NVS / BLE provisioning etc.)
static const char* DEVICE_ID  = "mitaro-001";
static const char* FW_VER     = "0.0.1";
static const char* BOARD_NAME = "esp32s3";
static const char* VIN_CLASS  = "12V";   // e.g. 12V / 24V / 36V / 48V

// Store last read states
static uint8_t lastState[8];

static inline void printEventJson(int ch, const char* edge, int level) {
  // v1 event (minimal fields + meta)
  // Note: Keep JSON compact and single-line for log parsing
  const unsigned long ts = millis();

  Serial.print("{\"v\":1");
  Serial.print(",\"device_id\":\""); Serial.print(DEVICE_ID); Serial.print("\"");
  Serial.print(",\"ts_ms\":"); Serial.print(ts);
  Serial.print(",\"src\":\"gpio\"");
  Serial.print(",\"ch\":"); Serial.print(ch);
  Serial.print(",\"edge\":\""); Serial.print(edge); Serial.print("\"");
  Serial.print(",\"level\":"); Serial.print(level);
  Serial.print(",\"pulse_ms\":0"); // reserved (future)
  Serial.print(",\"vin_class\":\""); Serial.print(VIN_CLASS); Serial.print("\"");
  Serial.print(",\"meta\":{");
  Serial.print("\"fw\":\""); Serial.print(FW_VER); Serial.print("\"");
  Serial.print(",\"board\":\""); Serial.print(BOARD_NAME); Serial.print("\"");
  Serial.print("}}");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // Inputs: for optocoupler/open-collector style, INPUT_PULLUP is a safe default
  for (int i = 0; i < 8; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
    lastState[i] = (uint8_t)digitalRead(inputPins[i]);
  }

  Serial.println("{\"boot\":\"Signal Mitaro gpio_logger\",\"fw\":\"0.0.1\"}");
}

void loop() {
  for (int i = 0; i < 8; i++) {
    const uint8_t cur = (uint8_t)digitalRead(inputPins[i]);
    if (cur == lastState[i]) continue;

    // Edge detection based on transition
    const char* edge = (cur > lastState[i]) ? "rise" : "fall";
    lastState[i] = cur;

    // Level: use 1/0 as integer
    printEventJson(i, edge, (int)cur);
  }

  delay(2);
}
