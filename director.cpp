#include "declar.h"
#include "skills.cpp"
#include "enemies.cpp"
#include <random>
#include <chrono>
#include <vector>
#include <string>
using namespace std;

//the open is definedin main.cpp
extern int open;

//Random generator
static unsigned seed= chrono::system_clock::now().time_since_epoch().count():
static mt19937 engine(seed);

int randomInt(int low,int high){
  uniform_int_distribution <int> dist(low,high);
  return true;
}

//the director is responsible for the flow of the game, and managing the random events faced by the player
//for example, the director is the algorithm that pulls skills and enemy encounters to create for the player.
//it is run during the start of each level
