#include <Adafruit_NeoPixel.h>

// Hardware pin definitions
#define STRIP1_PIN 6
#define STRIP2_PIN 7
#define STRIP1_LEDS 24
#define STRIP2_LEDS 8
#define BTN_MODE 2
#define BTN_ACTION 3
#define CLOCK_SPEED 10

// NeoPixel objects
Adafruit_NeoPixel strip1(STRIP1_LEDS, STRIP1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(STRIP2_LEDS, STRIP2_PIN, NEO_GRB + NEO_KHZ800);

// Game modes
enum Mode { CLOCK_MODE, REACTION_MODE, BOSS_MODE };
Mode currentMode = CLOCK_MODE;

// Clock mode variables
unsigned long lastClockUpdate = 0;

// Reaction game variables
int difficulty = 0;

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
int wallWidth = STRIP1_LEDS / 8;
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
    clearStrips(); // Clear strips on mode change
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

// ==================== CLOCK MODE ====================

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

// ==================== REACTION MODE ====================

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

// ==================== BOSS FIGHT MODE ====================

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
  
  // Increase player speed in phase 2 for better reaction time
  float currentSpeed = phase2 ? playerSpeed * 1.5 : playerSpeed;
  
  if (now - lastPlayerMove > (1000.0 / currentSpeed)) {
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

// Draw phase 1 attack (closing walls)
void drawPhase1Attack() {
  unsigned long now = millis();
  unsigned long attackElapsed = now - attackStartTime;
  
  // Warning duration decreases as boss HP decreases
  float hpRatio = (float)bossHP / STRIP2_LEDS;
  unsigned long dynamicWarningDuration = attackWarningDuration * (0.5 + 0.5 * hpRatio);
  
  if (attackElapsed < dynamicWarningDuration) {
    drawPhase1Warning();
  } else {
    drawPhase1Active();
  }
}

// Draw phase 1 warning (flashing walls)
void drawPhase1Warning() {
  unsigned long now = millis();
  
  // Flash speed increases as boss HP decreases
  float hpRatio = (float)bossHP / STRIP2_LEDS;
  int flashSpeed = 400 + (400 * (1.0 - hpRatio)); // 400ms to 800ms based on HP
  
  if (now - lastPhase1Flash > flashSpeed) {
    phase1Flash = !phase1Flash;
    lastPhase1Flash = now;
  }
  
  if (phase1Flash) {
    for (int i = 0; i < wallWidth; i++) {
      strip1.setPixelColor(wrapPosition(leftWallStart + i), strip1.Color(100, 0, 0));
      strip1.setPixelColor(wrapPosition(rightWallStart + i), strip1.Color(100, 0, 0));
    }
  }
}

// Draw phase 1 active (solid walls)
void drawPhase1Active() {
  for (int i = 0; i < wallWidth; i++) {
    strip1.setPixelColor(wrapPosition(leftWallStart + i), strip1.Color(255, 0, 0));
    strip1.setPixelColor(wrapPosition(rightWallStart + i), strip1.Color(255, 0, 0));
  }
}

// Draw phase 2 attack (complex patterns)
void drawPhase2Attack() {
  unsigned long now = millis();
  unsigned long attackElapsed = now - attackStartTime;
  
  // Warning duration decreases as boss HP decreases
  float hpRatio = (float)bossHP / STRIP2_LEDS;
  unsigned long dynamicWarningDuration = attackWarningDuration * (0.5 + 0.5 * hpRatio);
  
  if (attackElapsed < dynamicWarningDuration) {
    drawPhase2Warning();
  } else {
    drawPhase2Active();
  }
}

// Draw phase 2 warning (flashing danger zones)
void drawPhase2Warning() {
  unsigned long now = millis();
  
  // Flash speed increases as boss HP decreases
  float hpRatio = (float)bossHP / STRIP2_LEDS;
  int flashSpeed = 300 + (300 * (1.0 - hpRatio)); // 300ms to 600ms based on HP
  
  if (now - lastPhase2Flash > flashSpeed) {
    phase2Flash = !phase2Flash;
    lastPhase2Flash = now;
  }
  
  if (phase2Flash) {
    switch (attackPattern) {
      case 0: // Hourglass pattern
        for (int i = 0; i < dangerZone1Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 100, 0));
        }
        for (int i = 0; i < dangerZone2Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 100, 0));
        }
        break;
        
      case 1: // Double walls pattern
        for (int i = 0; i < dangerZone1Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 100, 0));
        }
        for (int i = 0; i < dangerZone2Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 100, 0));
        }
        break;
        
      case 2: // Triple danger zones pattern
        for (int i = 0; i < dangerZone1Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 100, 0));
        }
        for (int i = 0; i < dangerZone2Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 100, 0));
        }
        for (int i = 0; i < dangerZone3Width; i++) {
          strip1.setPixelColor(wrapPosition(dangerZone3Start + i), strip1.Color(255, 100, 0));
        }
        break;
    }
  }
}

// Draw phase 2 active (solid danger zones)
void drawPhase2Active() {
  switch (attackPattern) {
    case 0: // Hourglass pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 0, 0));
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 0, 0));
      }
      break;
      
    case 1: // Double walls pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 0, 0));
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 0, 0));
      }
      break;
      
    case 2: // Triple danger zones pattern
      for (int i = 0; i < dangerZone1Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone1Start + i), strip1.Color(255, 0, 0));
      }
      for (int i = 0; i < dangerZone2Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone2Start + i), strip1.Color(255, 0, 0));
      }
      for (int i = 0; i < dangerZone3Width; i++) {
        strip1.setPixelColor(wrapPosition(dangerZone3Start + i), strip1.Color(255, 0, 0));
      }
      break;
  }
}

// Check all collision types
void checkCollisions() {
  unsigned long now = millis();
  
  if (attackActive) {
    unsigned long attackElapsed = now - attackStartTime;
    
    // Use dynamic warning duration based on boss HP
    float hpRatio = (float)bossHP / STRIP2_LEDS;
    unsigned long dynamicWarningDuration = attackWarningDuration * (0.5 + 0.5 * hpRatio);
    
    if (attackElapsed >= dynamicWarningDuration) { 
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
    leftWallStart = wrapPosition(playerPos - STRIP1_LEDS / 4);
    rightWallStart = wrapPosition(playerPos + STRIP1_LEDS / 8);
    
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
        dangerZone1Start = wrapPosition(playerPos - STRIP1_LEDS / 6);
        dangerZone1Width = STRIP1_LEDS / 3;
        dangerZone2Start = wrapPosition(playerPos + STRIP1_LEDS / 3);
        dangerZone2Width = STRIP1_LEDS / 3;
        Serial.println("HOURGLASS PATTERN! Find the narrow safe path!");
        break;
        
      case 1: // Double walls pattern
        dangerZone1Start = wrapPosition(playerPos - STRIP1_LEDS / 4);
        dangerZone1Width = STRIP1_LEDS / 6;
        dangerZone2Start = wrapPosition(playerPos + STRIP1_LEDS / 6);
        dangerZone2Width = STRIP1_LEDS / 6;
        Serial.println("DOUBLE WALLS! Safe zone in the middle!");
        break;
        
      case 2: // Triple danger zones pattern
        dangerZone1Start = wrapPosition(playerPos - STRIP1_LEDS / 3);
        dangerZone1Width = STRIP1_LEDS / 8;
        dangerZone2Start = wrapPosition(playerPos);
        dangerZone2Width = STRIP1_LEDS / 8;
        dangerZone3Start = wrapPosition(playerPos + STRIP1_LEDS / 4);
        dangerZone3Width = STRIP1_LEDS / 8;
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

// ==================== UTILITY FUNCTIONS ====================

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
