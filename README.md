# ascii-game-project-26
# ASCII Knight

ASCII Knight is a terminal-based ASCII game made for the COMP2113 course project.

The goal of the game is to survive each level by, avoid enemies, open gates, collect pickups, and choose upgrades. The player has to clear 6 levels to win. The player goes to the next levell by opening all gates per level

## Game Description

The player controls `@` and moves around the map using the keyboard. Each level has gates, enemies, pickups, coins, and projectiles.

To clear a level, the player must open 3 gates. After each level, the player can choose upgrades. Coins collected during the level give more upgrade choices.

## Features

- Random level elements and enemy spawning
- Different enemy types with different movement and attacks
- Pickups such as time stop, kill enemy, and swap
- Upgrade system after each level
- Multiple levels with increasing difficulty
- Bestiary page that reads information from `bestiary.txt`
- Menu system for starting the game and viewing information

## Controls

### Game

text
W/A/S/D  Move
ESC      End run

### Menu

text
1-4  Choose option
ESC  Back or quit


### Bestiary

text
W/S  Scroll
A/D  Page
ESC  Back


## Files

text
main.cpp       Main game loop
menu.cpp       Menu and bestiary
director.cpp   Level generation
enemies.cpp    Enemy behavior
upgrades.cpp   Upgrades and pickups
bestiary.txt   Bestiary text
Makefile       Compilation

## How to Compile and Run

Compile:

bash
make main


Run:

bash
./main


Clean:

bash
make clean