#ifndef SETTINGS_H
#define SETTINGS_H

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
extern Adafruit_NeoPixel strip1;
extern Adafruit_NeoPixel strip2;

// Game modes
enum Mode { CLOCK_MODE, REACTION_MODE, BOSS_MODE };
extern Mode currentMode;

// Clock mode variables
extern unsigned long lastClockUpdate;

// Reaction game variables
extern int difficulty;

// Boss fight - Player
extern int playerPos;
extern int playerDir;
extern float playerSpeed;
extern unsigned long lastPlayerMove;

// Boss fight - Attack system
extern bool attackActive;
extern unsigned long attackStartTime;
extern unsigned long attackCooldown;
extern const unsigned long attackWarningDuration;
extern const unsigned long attackHitDuration;
extern unsigned long lastAttackTime;

// Boss fight - Phase 1 (closing walls)
extern bool wallsClosing;
extern int leftWallStart;
extern int rightWallStart;
extern int wallWidth;
extern bool phase1Flash;
extern unsigned long lastPhase1Flash;

// Boss fight - Phase 2 (complex patterns)
extern bool phase2;
extern int attackPattern;
extern int dangerZone1Start;
extern int dangerZone1Width;
extern int dangerZone2Start;
extern int dangerZone2Width;
extern int dangerZone3Start;
extern int dangerZone3Width;
extern bool phase2Flash;
extern unsigned long lastPhase2Flash;

// Boss fight - Boss and drops
extern int bossHP;
extern int dropPos;
extern bool dropActive;
extern unsigned long lastDropTime;
extern unsigned long dropCooldown;
extern unsigned long fightStartTime;
extern const unsigned long initialDelay;

#endif
