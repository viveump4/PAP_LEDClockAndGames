#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "settings.h"

// Clock mode functions
void showClock();
void updateClockDisplay(int hours, int minutes, int seconds);
void printClockTime(int hours, int minutes, int seconds);

// Reaction game functions
void playReactionGame();
void handleReactionInput();
void updateReactionDisplay(int target, int pos);
void processReactionResult(int pos, int target);

// Boss fight - Main functions
void playBossFight();
void resetBossFight();
void handlePlayerMovement();
void handleAttackSystem();
void handleDropSystem();
void checkCollisions();

// Boss fight - Display functions
void drawBossFightDisplay();
void drawPlayer();
void drawBossHP();
void drawDrops();

// Boss fight - Attack system
void startAttack();
void updateAttack();
void drawPhase1Attack();
void drawPhase2Attack();
void checkPhase1Collision();
void checkPhase2Collision();
void drawPhase1Warning();
void drawPhase1Active();
void drawPhase2Warning();
void drawPhase2Active();

// Utility functions
int wrapPosition(int pos);
int findSafeDropPosition();
void successFlash();
void failFlash();
void clearStrips();

// Main program functions
void handleModeSwitch();
void runCurrentMode();
void initializeHardware();
void initializeGameState();

#endif
