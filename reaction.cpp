#include "functions.h"

// Reaction game variables
int difficulty = 0;

// Main reaction game loop
void playReactionGame() {
  static int target = random(STRIP1_LEDS);
  static int pos = 0;

  updateReactionDisplay(target, pos);
  handleReactionInput();
  
  // Move player position
  pos++;
  if (pos >= STRIP1_LEDS) pos = 0;
  delay(map(difficulty, 1, STRIP2_LEDS, 100, 30));
}

// Update LED display for reaction game
void updateReactionDisplay(int target, int pos) {
  clearStrips();

  // Green target, blue hints (only at low difficulty)
  strip1.setPixelColor(target, strip1.Color(0, 255, 0));
  if (difficulty < (STRIP2_LEDS/2)) {
    strip1.setPixelColor(target - 1, strip1.Color(0, 0, 255));
    strip1.setPixelColor(target + 1, strip1.Color(0, 0, 255));
  }

  // Red player position
  strip1.setPixelColor(pos, strip1.Color(255, 0, 0));

  // Yellow difficulty indicator
  for (int i = 0; i < difficulty; i++)
    strip2.setPixelColor(i, strip2.Color(255, 255, 0));

  strip1.show();
  strip2.show();
}

// Handle button input for reaction game
void handleReactionInput() {
  if (!digitalRead(BTN_ACTION)) {
    static int target = random(STRIP1_LEDS);
    static int pos = 0;
    
    processReactionResult(pos, target);
    target = random(STRIP1_LEDS);
    delay(300); // Debounce
  }
}

// Process hit/miss result and adjust difficulty
void processReactionResult(int pos, int target) {
  if (pos == target || pos == target - 1 || pos == target + 1) {
    successFlash();
    if (difficulty < STRIP2_LEDS) difficulty++;
  } else {
    failFlash();
    if (difficulty > 1) difficulty--;
  }
}
