#include "functions.h"

// Reaction game variables
int difficulty = 0;

// Main reaction game loop
void playReactionGame() {
  static int target = random(STRIP1_LEDS);  // Target position (red)
  static int pos = 0;                       // Moving position (yellow)
  static unsigned long lastMove = 0;        // For controlling LED movement speed
  static int lastDifficulty = 0;           // Track difficulty changes
  
  unsigned long now = millis();
  
  // If difficulty changed, move the target
  if (lastDifficulty != difficulty) {
    do {
      target = random(STRIP1_LEDS);
    } while (target == pos);
    lastDifficulty = difficulty;
  }
  
  // Calculate speed based on difficulty (starts faster, gets very fast)
  int baseDelay = map(difficulty, 0, STRIP2_LEDS-1, 50, 10); // 50ms at start, 10ms at max difficulty
  
  // Move the yellow LED based on speed
  if (now - lastMove > baseDelay) {
    pos = (pos + 1) % STRIP1_LEDS;  // Wrap around
    lastMove = now;
    
    // Print debug info
    Serial.print("Target: ");
    Serial.print(target);
    Serial.print(" Pos: ");
    Serial.print(pos);
    Serial.print(" Difficulty: ");
    Serial.println(difficulty);
  }

  updateReactionDisplay(target, pos);
  handleReactionInput(pos, target);
}

// Update LED display for reaction game
void updateReactionDisplay(int target, int pos) {
  clearStrips();

  // Red target (single LED)
  strip1.setPixelColor(target, strip1.Color(255, 0, 0));

  // Yellow moving position
  strip1.setPixelColor(pos, strip1.Color(255, 255, 0));

  // Show difficulty level on strip2
  for (int i = 0; i < difficulty; i++) {
    strip2.setPixelColor(i, strip2.Color(255, 255, 0));
  }

  strip1.show();
  strip2.show();
}

// Handle button input for reaction game
void handleReactionInput(int pos, int target) {
  if (!digitalRead(BTN_ACTION)) {
    if (pos == target) {  // Must hit exactly on target
      successFlash();
      if (difficulty < STRIP2_LEDS-1) {
        difficulty++;
        Serial.print("HIT! New difficulty: ");
        Serial.println(difficulty);
      } else {
        Serial.println("PERFECT! Maximum difficulty!");
      }
    } else {
      failFlash();
      if (difficulty > 0) {
        difficulty--;
        Serial.print("MISS! New difficulty: ");
        Serial.println(difficulty);
      }
    }

    // Generate new target position (make sure it's not at current position)
    do {
      target = random(STRIP1_LEDS);
    } while (target == pos);
    
    delay(200); // Shorter debounce for faster gameplay
  }
}
}
