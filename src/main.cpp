#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.printf("Chip ID: %08X\n", ESP.getChipId());
  Serial.printf("Flash Size: %u bytes\n", ESP.getFlashChipRealSize());
  Serial.printf("CPU Freq: %u MHz\n", ESP.getCpuFreqMHz());
}

void loop() {}