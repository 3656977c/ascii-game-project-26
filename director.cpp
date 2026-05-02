#include "declar.h"
#include "director.h"
//#include "enemies.cpp"

//!!!!!!!!!!MAybe we use everything here? idk it depends because were still on generation
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
// Checks wether a tile already has something on it
bool occupied(vector<entity> &elist,int x,int y, player p){
    if (p.x== x && p.y== y) {
      return true;
    }
  for (auto &e :elist){
    if (e.c != -1 && e.x == x && e.y ==y) {
      return true;
    }
  }
  return false;
}
// gets a free tile anywhere  on the map
void getFreePosition(vector<entity> &elist, player p, int n, int m, int &x, int &y) {
      do{
          x = randomInt(0, n-1);
          y = randomInt(0, m-1);
    } while(occupied(elist, x, y, p))
  }







// Adds the 3 required gates in 3 differnt map sections
void spawnGates(vector<entity> &elist, player p, int n, int m) {
  for (int section = 0;section < 3; section++) {
        int x,y;
        getFreePositionInSection(elist,p,n,m,section,x,y);
        entity gate{x,y, "gate",'H',3,-1,-1,-1};
        elist.push_back(gate);
  }
}
//Adds the coins
void spawnCoins(vector<entity> &elist, player p,int n,int m, int level){
    int coinCount = 2+level;
    for (int i=0; i < coinCount ; i++) {
        int x,y;
        getFreePosition(elist,p,n,m,x,y);
        entity coin{x,y, "coin", 'c',3,-1,-1,-1};
        elist.push_back(coin);
    }
}

//Adss one bouncer enemy
void spawnBouncer(vector<entity> &elist, player p, int n, int m) {
  int x,y;
  getFreePosition(elist,p,n,m,x,y);

  int ax = randomInt(0,1) == 0 ? -1 : 1;
  int ay = randomInt(0,1) == 0 ? -1 : 1;
  entity enemy{x,y,"bouncer", 'O', 2, ax, ay, 0};
  elist.push_back(enemy);
}

//Adss one follower enemy
void spawnFollower(vector<entity> &elist, player p, int n, int m) {
  int x,y;
  getFreePosition(elist, p, n, m, x, y);
  entity enemy{x, y, "follow", '!',2, 1, 1, 0};
  entity.push_back(enemy);
}

//Chooses an enemy from the current enemy pool
void spawnRandomEnemies(vector<entity> &elist, player p, int n,int m, int enemyPoolSize) {
  int enemyType= randomInt(1, enemyPoolSize);
  if (enemyType ==1) {
      spawnBouncer(elist, p, n, m);
} else if (enemyType == 2) {
      spawnFollower(elist,p, n, m);
}
}

// Initial enemies at he start of the level
void spawnStartingEnemies(vector<entity> &elist, player p, int n, int m, int level, int enemyPoolSize){
  int enemyCount= 2+level;
  for(int i = 0; i< enemyCount; i++) {
      spawnRandomEnemy(elist,p, n,m, enemyPoolSize);
  }
}

// spawns extra enemies as turns pass, This creates a time pressure
void directorTurnUpdate(vector<entity> &elist, player p,int n, int m, int level, int turnNumber, int enemyPoolSize){
      int spawnRate = 12 - level;
      if (spawnRate<4) {
          spawnRate = 4;
      
      }if (turnNumber >0 && turnNumber % spawnRate == 0) {
          spawnRandomEnemy(elist,p, n, m, enemyPoolSize);
      }
}



//rand val - declared in main.cpp
extern unsigned seed;
extern mt19937 engine;
extern uniform_int_distribution<int> dist;
#define rand dist(engine) 

void director(vector<entity> &elist, vector<pair<int,int>> &wall, player &p, int danval) {
    //elist is the list of all entities, p is player, danval is the danger value

    vector<int> section = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int temp = rand%9;

    //pick a section of the map for the player.
    //the player spawns in the middle of this section
    //the director then chooses a random starting layout for this

    //PLAYER SECTION
    //choose section
        int playersection = section[temp];
        section.erase(section.begin() + temp);
    //init section borders
        int bpx = 5 *((playersection - 1)%3);
        int bpy = 9 *((playersection - 1)/3);
    //spawn player
        p.x = bpx + 2 + (rand%3) - 1;
        p.y = bpy + 4 + (rand%3) - 1;
    //choose layout
        vector<vector<pair<int,int>>> playout = {
            {},
            {{1, 2},{1, 6},{2, 1},{2, 2},{2, 6},{2, 7},{3, 2},{3, 6}},
            {{1, 1},{1, 2},{1, 7},{3, 1},{3, 6},{3, 7}}
        };
        temp = rand%3;
        for (auto &i : playout[temp]) {
            wall.emplace_back(bpx + i.first, bpy + i.second);
        }
}
