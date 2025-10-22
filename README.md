# Arduino LED Strip Game - Refactored Structure

This project has been refactored into a modular structure for better organization and maintainability.

## File Structure

### Core Files
- **main.ino** - Main Arduino file with setup() and loop() functions
- **settings.h** - All pin definitions, constants, and global variable declarations
- **functions.h** - Function declarations for all modules

### Game Modules
- **clock.cpp** - Clock mode functionality
- **reaction.cpp** - Reaction game functionality  
- **bossfight.cpp** - Boss fight game functionality

## Key Improvements

1. **Modular Design**: Each game mode is now in its own file
2. **Smaller Functions**: Large functions have been broken down into smaller, more manageable pieces
3. **Clear Separation**: Settings, declarations, and implementations are properly separated
4. **Better Organization**: Related functionality is grouped together
5. **Easier Maintenance**: Changes to one game mode don't affect others

## Game Modes

- **Clock Mode**: Displays time using LED strips
- **Reaction Mode**: Reaction-based game with increasing difficulty
- **Boss Fight Mode**: Complex boss battle with multiple attack patterns

## Hardware Requirements

- 2x NeoPixel LED strips (24 and 8 LEDs)
- 2x Push buttons
- Arduino-compatible board
