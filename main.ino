#include "settings.h"
#include "functions.h"

// NeoPixel strip objects
Adafruit_NeoPixel strip1(STRIP1_LEDS, STRIP1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(STRIP2_LEDS, STRIP2_PIN, NEO_GRB + NEO_KHZ800);

// Current game mode
Mode currentMode = BOSS_MODE;

void setup() {
  initializeHardware();
  initializeGameState();
  Serial.println("=== Boss Fight Game Started ===");
}

void loop() {
  handleModeSwitch();
  runCurrentMode();
}

// Handle mode switching with button press
void handleModeSwitch() {
  if (!digitalRead(BTN_MODE)) {
    currentMode = (Mode)((currentMode + 1) % 3);
    delay(300); // Debounce
    switch (currentMode) {
      case CLOCK_MODE: Serial.println(">> Mode: CLOCK"); break;
      case REACTION_MODE: Serial.println(">> Mode: REACTION"); break;
      case BOSS_MODE: 
        Serial.println(">> Mode: BOSS"); 
        resetBossFight();
        break;
    }
  }
}

// Run the current game mode
void runCurrentMode() {
  switch (currentMode) {
    case CLOCK_MODE: showClock(); break;
    case REACTION_MODE: playReactionGame(); break;
    case BOSS_MODE: playBossFight(); break;
  }
}

// Initialize hardware components
void initializeHardware() {
  Serial.begin(9600);
  strip1.begin();
  strip2.begin();
  strip1.show();
  strip2.show();

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);
  randomSeed(analogRead(A0));
}

// Initialize game state
void initializeGameState() {
  resetBossFight();
}