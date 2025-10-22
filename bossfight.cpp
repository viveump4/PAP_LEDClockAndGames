#include "functions.h"

// Boss fight variables
int playerPos = 0;
int playerDir = 1;
float playerSpeed = 2.0;
unsigned long lastPlayerMove = 0;

// Attack system
bool attackActive = false;
unsigned long attackStartTime = 0;
unsigned long attackCooldown = 5000;
const unsigned long attackWarningDuration = 2000;
const unsigned long attackHitDuration = 2000;
unsigned long lastAttackTime = 0;

// Phase 1 (closing walls)
bool wallsClosing = false;
int leftWallStart = -1;
int rightWallStart = -1;
int wallWidth = 3;
bool phase1Flash = false;
unsigned long lastPhase1Flash = 0;

// Phase 2 (complex patterns)
bool phase2 = false;
int attackPattern = 0;
int dangerZone1Start = -1;
int dangerZone1Width = 0;
int dangerZone2Start = -1;
int dangerZone2Width = 0;
int dangerZone3Start = -1;
int dangerZone3Width = 0;
bool phase2Flash = false;
unsigned long lastPhase2Flash = 0;

int bossHP = STRIP2_LEDS;

int dropPos = 0;
bool dropActive = true;
unsigned long lastDropTime = 0;
unsigned long dropCooldown = 3000;

unsigned long fightStartTime = 0;
const unsigned long initialDelay = 3000;

// Main boss fight game loop
void playBossFight() {
  unsigned long now = millis();
  
  handlePlayerMovement();
  handleAttackSystem();
  handleDropSystem();
  drawBossFightDisplay();
  checkCollisions();

  delay(50);
}

// Handle player movement and direction changes
void handlePlayerMovement() {
  unsigned long now = millis();
  
  static unsigned long lastButtonPress = 0;
  if (!digitalRead(BTN_ACTION) && now - lastButtonPress > 200) {
    playerDir = -playerDir;
    lastButtonPress = now;
  }
  
  if (now - lastPlayerMove > (1000.0 / playerSpeed)) {
    playerPos = wrapPosition(playerPos + playerDir);
    lastPlayerMove = now;
  }
}

// Handle attack system logic
void handleAttackSystem() {
  unsigned long now = millis();
  
  if (now - fightStartTime > initialDelay) {
    if (!attackActive && now - lastAttackTime > attackCooldown) {
      startAttack();
    }
    
    if (attackActive) {
      updateAttack();
    }
  }
}

// Handle drop spawning and collection
void handleDropSystem() {
  unsigned long now = millis();
  
  if (!attackActive && !dropActive && now - lastDropTime > dropCooldown) {
    dropPos = findSafeDropPosition();
    dropActive = true;
    Serial.println("New drop appeared!");
  }
}

// Draw all boss fight elements
void drawBossFightDisplay() {
  clearStrips();
  
  if (attackActive) {
    if (!phase2) {
      drawPhase1Attack();
    } else {
      drawPhase2Attack();
    }
  }

  drawPlayer();
  drawDrops();
  drawBossHP();

  strip1.show();
  strip2.show();
}

// Draw player as yellow dot
void drawPlayer() {
  strip1.setPixelColor(playerPos, strip1.Color(255, 255, 0));
}

// Draw boss HP bar
void drawBossHP() {
  for (int i = 0; i < STRIP2_LEDS; i++) {
    if (i < bossHP) {
      strip2.setPixelColor(i, strip2.Color(255, 0, 0));
    } else {
      strip2.setPixelColor(i, strip2.Color(0, 0, 0));
    }
  }
}

// Draw collectible drops
void drawDrops() {
  if (!attackActive && dropActive) {
    strip1.setPixelColor(dropPos, strip1.Color(0, 0, 255));
  }
}

void drawPhase1Attack() {
  unsigned long now = millis();
  unsigned long attackElapsed = now - attackStartTime;
  
  if (attackElapsed < attackWarningDuration) {
    drawPhase1Warning();
  } else {
    drawPhase1Active();
  }
}

void drawPhase1Warning() {
  unsigned long now = millis();
  
  // Warning phase - flashing walls that will appear (dark red)
  if (now - lastPhase1Flash > (400 / (playerSpeed / 2.0))) {
    phase1Flash = !phase1Flash;
    lastPhase1Flash = now;
  }
  
  if (phase1Flash) {
    // Show where walls WILL appear (dark red)
    for (int i = 0; i < wallWidth; i++) {
      strip1.setPixelColor(wrapPosition(leftWallStart + i), strip1.Color(100, 0, 0)); // Dark red warning
      strip1.setPixelColor(wrapPosition(rightWallStart + i), strip1.Color(100, 0, 0)); // Dark red warning
    }
  }
}

void drawPhase1Active() {
  // Attack phase - walls are active (bright red) - FIXED POSITION
  for (int i = 0; i < wallWidth; i++) {
    strip1.setPixelColor(wrapPosition(leftWallStart + i), strip1.Color(255, 0, 0)); // Bright red active
    strip1.setPixelColor(wrapPosition(rightWallStart + i), strip1.Color(255, 0, 0)); // Bright red active
  }
}

void drawPhase2Attack() {
  unsigned long now = millis();
  unsigned long attackElapsed = now - attackStartTime;
  
  if (attackElapsed < attackWarningDuration) {
    drawPhase2Warning();
  } else {
    drawPhase2Active();
  }
}

void drawPhase2Warning() {
  unsigned long now = millis();
  
  // Warning phase - flash danger zones and show safe areas
  if (now - lastPhase2Flash > (300 / (playerSpeed / 2.0))) {
    phase2Flash = !phase2Flash;
    lastPhase2Flash = now;
  }
  
  if (phase2Flash) {
    // Flash danger zones (orange) and show safe areas (green)
    // Declare variables outside switch to avoid scope issues
    int safeZone, safeZoneMiddle, safeZone1, safeZone2;
    
    switch (attackPattern) {
      case 0: // HOURGLASS pattern
        for (int i = 0; i < dangerZone1Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 100, 0)); // Orange danger
        }
        for (int i = 0; i < dangerZone2Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 100, 0)); // Orange danger
        }
        // Show safe zones
        safeZone = wrapPosition(dangerZone1Start + dangerZone1Width + 2);
        strip1.setPixelColor(safeZone, strip1.Color(0, 255, 0)); // Green safe
        break;
        
      case 1: // DOUBLE WALLS pattern
        for (int i = 0; i < dangerZone1Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 100, 0)); // Orange danger
        }
        for (int i = 0; i < dangerZone2Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 100, 0)); // Orange danger
        }
        // Show safe zone between walls
        safeZoneMiddle = wrapPosition((dangerZone1Start + dangerZone1Width + dangerZone2Start) / 2);
        strip1.setPixelColor(safeZoneMiddle, strip1.Color(0, 255, 0)); // Green safe
        break;
        
      case 2: // TRIPLE DANGER ZONES pattern
        for (int i = 0; i < dangerZone1Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 100, 0)); // Orange danger
        }
        for (int i = 0; i < dangerZone2Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 100, 0)); // Orange danger
        }
        for (int i = 0; i < dangerZone3Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone3Start + i), strip1.Color(255, 100, 0)); // Orange danger
        }
        // Show safe zones
        safeZone1 = wrapPosition(dangerZone1Start - 3);
        safeZone2 = wrapPosition(dangerZone2Start + dangerZone2Width + 3);
        strip1.setPixelColor(safeZone1, strip1.Color(0, 255, 0)); // Green safe
        strip1.setPixelColor(safeZone2, strip1.Color(0, 255, 0)); // Green safe
        break;
    }
  }
}

void drawPhase2Active() {
  // Attack phase - danger zones active (red)
  switch (attackPattern) {
    case 0: // HOURGLASS pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 0, 0)); // Red active
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 0, 0)); // Red active
      }
      break;
      
    case 1: // DOUBLE WALLS pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 0, 0)); // Red active
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 0, 0)); // Red active
      }
      break;
      
    case 2: // TRIPLE DANGER ZONES pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 0, 0)); // Red active
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 0, 0)); // Red active
      }
      for (int i = 0; i < dangerZone3Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone3Start + i), strip1.Color(255, 0, 0)); // Red active
      }
      break;
  }
}

// Check all collision types
void checkCollisions() {
  unsigned long now = millis();
  
  if (attackActive) {
    unsigned long attackElapsed = now - attackStartTime;
    
    if (attackElapsed >= attackWarningDuration) { 
      if (!phase2) {
        checkPhase1Collision();
      } else {
        checkPhase2Collision();
      }
    }
  }

  // Drop collection
  if (!attackActive && dropActive && playerPos == dropPos) {
    bossHP--;
    dropActive = false;
    lastDropTime = now;
    Serial.print("Boss hit! HP: ");
    Serial.println(bossHP);

    if (bossHP <= STRIP2_LEDS / 2 && !phase2) {
      phase2 = true;
      attackCooldown = 3500; 
      attackActive = false;
      Serial.println("Phase 2 activated! Complex attack patterns incoming!");
    }

    if (bossHP <= 0) {
      Serial.println("Boss defeated! You win!");
      successFlash();
      resetBossFight();
      return;
    }
  }
}

// Check collision with phase 1 closing walls
void checkPhase1Collision() {
  for (int i = 0; i < wallWidth; i++) {
    if (playerPos == wrapPosition(leftWallStart + i) || 
        playerPos == wrapPosition(rightWallStart + i)) {
      Serial.println("Touched closing wall! Game Over!");
      failFlash();
      resetBossFight();
      return;
    }
  }
}

// Check collision with phase 2 danger zones
void checkPhase2Collision() {
  switch (attackPattern) {
    case 0: // Hourglass pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        if (playerPos == wrapPosition(dangerZone1Start + i)) {
          Serial.println("Hit by hourglass attack! Game Over!");
          failFlash();
          resetBossFight();
          return;
        }
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        if (playerPos == wrapPosition(dangerZone2Start + i)) {
          Serial.println("Hit by hourglass attack! Game Over!");
          failFlash();
          resetBossFight();
          return;
        }
      }
      break;
      
    case 1: // Double walls pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        if (playerPos == wrapPosition(dangerZone1Start + i)) {
          Serial.println("Hit by double wall! Game Over!");
          failFlash();
          resetBossFight();
          return;
        }
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        if (playerPos == wrapPosition(dangerZone2Start + i)) {
          Serial.println("Hit by double wall! Game Over!");
          failFlash();
          resetBossFight();
          return;
        }
      }
      break;
      
    case 2: // Triple danger zones pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        if (playerPos == wrapPosition(dangerZone1Start + i)) {
          Serial.println("Hit by triple danger zone! Game Over!");
          failFlash();
          resetBossFight();
          return;
        }
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        if (playerPos == wrapPosition(dangerZone2Start + i)) {
          Serial.println("Hit by triple danger zone! Game Over!");
          failFlash();
          resetBossFight();
          return;
        }
      }
      for (int i = 0; i < dangerZone3Width; i++) {
        if (playerPos == wrapPosition(dangerZone3Start + i)) {
          Serial.println("Hit by triple danger zone! Game Over!");
          failFlash();
          resetBossFight();
          return;
        }
      }
      break;
  }
}

// Start a new attack based on current phase
void startAttack() {
  attackActive = true;
  attackStartTime = millis();
  
  if (!phase2) {
    // Phase 1: Closing walls
    leftWallStart = wrapPosition(playerPos - 6);
    rightWallStart = wrapPosition(playerPos + 3);
    
    Serial.print("FIXED WALLS! Left: ");
    Serial.print(leftWallStart);
    Serial.print(" Right: ");
    Serial.println(rightWallStart);
    
    phase1Flash = true;
    lastPhase1Flash = millis();
  } else {
    // Phase 2: Complex patterns
    attackPattern = random(3); 
    
    switch (attackPattern) {
      case 0: // Hourglass pattern
        dangerZone1Start = wrapPosition(playerPos - 4);
        dangerZone1Width = 8;
        dangerZone2Start = wrapPosition(playerPos + 8);
        dangerZone2Width = 8;
        Serial.println("HOURGLASS PATTERN! Find the narrow safe path!");
        break;
        
      case 1: // Double walls pattern
        dangerZone1Start = wrapPosition(playerPos - 6);
        dangerZone1Width = 4;
        dangerZone2Start = wrapPosition(playerPos + 4);
        dangerZone2Width = 4;
        Serial.println("DOUBLE WALLS! Safe zone in the middle!");
        break;
        
      case 2: // Triple danger zones pattern
        dangerZone1Start = wrapPosition(playerPos - 8);
        dangerZone1Width = 3;
        dangerZone2Start = wrapPosition(playerPos);
        dangerZone2Width = 3;
        dangerZone3Start = wrapPosition(playerPos + 6);
        dangerZone3Width = 3;
        Serial.println("TRIPLE DANGER ZONES! Navigate the scattered safe areas!");
        break;
    }
    
    phase2Flash = true;
    lastPhase2Flash = millis();
  }
}

// Update attack state and end when duration expires
void updateAttack() {
  unsigned long now = millis();
  unsigned long attackElapsed = now - attackStartTime;
  
  if (attackElapsed > attackWarningDuration + attackHitDuration) {
    attackActive = false;
    lastAttackTime = now;
    Serial.println("Attack ended!");
  }
}

// Reset boss fight to initial state
void resetBossFight() {
  bossHP = STRIP2_LEDS;
  phase2 = false;
  playerPos = 0;
  playerDir = 1;
  playerSpeed = 2.0;
  attackActive = false;
  leftWallStart = -1;
  rightWallStart = -1;
  dangerZone1Start = -1;
  dangerZone2Start = -1;
  dangerZone3Start = -1;
  dropPos = findSafeDropPosition();
  dropActive = true;
  lastDropTime = millis();
  fightStartTime = millis();
  lastPlayerMove = millis();
  lastAttackTime = millis();
  attackCooldown = 5000;
  dropCooldown = 3000;
  Serial.println("Boss fight reset!");
}

// Find a safe position for drops (not on player)
int findSafeDropPosition() {
  int newPos;
  do {
    newPos = random(STRIP1_LEDS);
  } while (newPos == playerPos);
  
  return newPos;
}

// Toroidal position wrapping for circular LED strip
int wrapPosition(int pos) {
  if (pos < 0) {
    return STRIP1_LEDS + pos;
  } else if (pos >= STRIP1_LEDS) {
    return pos - STRIP1_LEDS;
  }
  return pos;
}

// Visual feedback for success
void successFlash() {
  for (int i = 0; i < 3; i++) {
    strip1.fill(strip1.Color(0, 255, 0));
    strip1.show();
    delay(100);
    strip1.clear();
    strip1.show();
    delay(100);
  }
}

// Visual feedback for failure
void failFlash() {
  for (int i = 0; i < 3; i++) {
    strip1.fill(strip1.Color(255, 0, 0));
    strip1.show();
    delay(100);
    strip1.clear();
    strip1.show();
    delay(100);
  }
}

// Clear both LED strips
void clearStrips() {
  strip1.clear();
  strip2.clear();
}