#include "functions.h"

// Clock mode variables
unsigned long lastClockUpdate = 0;

// Main clock display function
void showClock() {
  unsigned long now = millis() / 1000.0 * CLOCK_SPEED;
  int seconds = now % 60;
  int minutes = (now / 60) % 60;
  int hours   = (now / 3600) % 12;

  updateClockDisplay(hours, minutes, seconds);
  
  // Print time every second
  if (millis() - lastClockUpdate > 1000) {
    printClockTime(hours, minutes, seconds);
    lastClockUpdate = millis();
  }

  delay(50);
}

// Update LED display with clock hands
void updateClockDisplay(int hours, int minutes, int seconds) {
  int secPos  = (seconds * STRIP1_LEDS) / 60; 
  int minPos  = (minutes * STRIP1_LEDS) / 60;
  int hourPos = (hours * STRIP1_LEDS) / 12;

  clearStrips();

  // Red = hours, Green = minutes, Blue = seconds
  strip1.setPixelColor(hourPos, strip1.Color(255, 0, 0));
  strip1.setPixelColor(minPos, strip1.Color(0, 255, 0));
  strip1.setPixelColor(secPos, strip1.Color(0, 0, 255));
  strip1.show();

  // Second strip shows white background
  strip2.fill(strip2.Color(255, 255, 255));
  strip2.show();
}

// Print current time to serial
void printClockTime(int hours, int minutes, int seconds) {
  Serial.print("Clock - H:");
  Serial.print(hours);
  Serial.print(" M:");
  Serial.print(minutes);
  Serial.print(" S:");
  Serial.println(seconds);
}
