# ASCII Knight

ASCII Knight is a terminal-based ASCII game made for the COMP2113 course project.

## Team Members
- Ezekiel Meir Co
- Carl Heinrich Ong
- ChingKhei

## Application Description

The goal of the game is to survive each level by avoiding enemies, opening gates, collecting pickups, and choosing upgrades. The player must clear 6 levels to win.

The player controls `@` and moves around a grid map. Each level contains enemies, gates, pickups, coins, and projectiles. To progress, the player needs to open all 3 gates in the level.

After each level, the player can choose upgrades. Coins collected during the level increase the number of upgrade choices.

## Features

- Random level elements and enemy spawning  
- Different enemy types with unique movement and attacks  
- Pickups such as time stop, kill enemy, and swap  
- Upgrade system after each level  
- Multiple levels with increasing difficulty  
- Bestiary system using file input (bestiary.txt)  
- Menu system for navigation  

## How Features Satisfy Coding Requirements

- **Random events**: Enemy spawning, pickups, and level elements are generated randomly each run  
- **Data structures**: Arrays/vectors are used to store the map, enemies, pickups, and projectiles  
- **Dynamic memory management**: Game entities and objects are created and managed during runtime  
- **File input/output**: `bestiary.txt` is read to display enemy and entity information  
- **Multiple files**: The program is split into multiple `.cpp` and `.h` files for modular design  
- **Multiple difficulty levels**: The game becomes progressively harder as the player advances through levels and the player can choose difficulty in the menu

## Controls

Game:
W/A/S/D  Move  
ESC      End run  

Menu:
1-4  Choose option  
ESC  Back or quit  

Bestiary:
W/S  Scroll  
A/D  Page  
ESC  Back  

## Files

main.cpp       Main game loop  
menu.cpp       Menu and bestiary  
director.cpp   Level generation  
enemies.cpp    Enemy behavior  
upgrades.cpp   Upgrades and pickups  
bestiary.txt   Bestiary text  
Makefile       Compilation  

## Compilation & Execution

Compile:
make main

Run:
./main

Clean:
make clean

## Libraries Used

No non-standard C/C++ libraries are used.

## Post Submission Patch Notes
v1.1 - Added a cooldown to the grappler enemy
