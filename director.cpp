#include "declar.h"
#include "director.h"
//#include "enemies.cpp"

/*!!!!!!!!!!MAybe we use everything here? idk it depends because were still on generation
#include <random>
#include <chrono>
#include <vector>
#include <string>
using namespace std;

// open is defined in main.cpp
extern int open;

//Random generator
static unsigned seed= chrono::system_clock::now().time_since_epoch().count();
static mt19937 engine(seed);

int randomInt(int low, int high) {
    uniform_int_distribution<int> dist(low, high);
    return dist(engine);
}
// Checks whether a tile already has something on it
bool occupied(vector<entity> &elist, int x, int y, player p) {
    if (p.x == x && p.y == y) {
        return true;
    }

    for (auto &e: elist) {
        if (e.c != -1 && e.x == x && e.y == y) {
            return true;
        }
    }

    return false;
}

void getFreePosition(vector<entity> &elist, player p, int n, int m, int &x, int &y) {
    for (int tries = 0; tries < 500; tries++) {
        x = randomInt(0, n - 1);
        y = randomInt(0, m - 1);

        if (!occupied(elist, x, y, p)) {
            return;
        }
    }

    for (x = 0; x < n; x++) {
        for (y = 0; y < m; y++) {
            if (!occupied(elist, x, y, p)) {
                return;
            }
        }
    }
}

void getFreePositionInSection(vector<entity> &elist, player p, int n, int m, int section, int &x, int &y) {
    int sectionWidth = m / 3;
    int lowY = section * sectionWidth;
    int highY = (section == 2) ? m - 1 : lowY + sectionWidth - 1;

    for (int tries = 0; tries < 500; tries++) {
        x = randomInt(0, n - 1);
        y = randomInt(lowY, highY);

        if (!occupied(elist, x, y, p)) {
            return;
        }
    }

    getFreePosition(elist, p, n, m, x, y);
}

// Adds the 3 required gates in 3 different map sections
void spawnGates(vector<entity> &elist, player p, int n, int m) {
    for (int section = 0; section < 3; section++) {
        int x, y;
        getFreePositionInSection(elist, p, n, m, section, x, y);
        entity gate{x, y, "gate", 'H', 3, -1, -1, -1};
        elist.push_back(gate);
    }
}

// Adds the coins
void spawnCoins(vector<entity> &elist, player p, int n, int m, int level) {
    int coinCount = 2 + level;
    for (int i = 0; i < coinCount; i++) {
        int x, y;
        getFreePosition(elist, p, n, m, x, y);
        entity coin{x, y, "coin", 'c', 3, -1, -1, -1};
        elist.push_back(coin);
    }
}

// Adds one bouncer enemy
void spawnBouncer(vector<entity> &elist, player p, int n, int m) {
    int x, y;
    getFreePosition(elist, p, n, m, x, y);

    int ax = randomInt(0, 1) == 0 ? -1 : 1;
    int ay = randomInt(0, 1) == 0 ? -1 : 1;
    entity enemy{x, y, "bouncer", 'O', 2, ax, ay, 0};
    elist.push_back(enemy);
}

// Adds one follower enemy
void spawnFollower(vector<entity> &elist, player p, int n, int m) {
    int x, y;
    getFreePosition(elist, p, n, m, x, y);
    entity enemy{x, y, "follow", '!', 2, 1, 1, 0};
    elist.push_back(enemy);
}

// Chooses an enemy from the current enemy pool
void spawnRandomEnemy(vector<entity> &elist, player p, int n, int m, int enemyPoolSize) {
    int enemyType = randomInt(1, enemyPoolSize);
    if (enemyType == 1) {
        spawnBouncer(elist, p, n, m);
    } else if (enemyType == 2) {
        spawnFollower(elist, p, n, m);
    }
}

// Initial enemies at the start of the level
void spawnStartingEnemies(vector<entity> &elist, player p, int n, int m, int level, int enemyPoolSize) {
    int enemyCount = 2 + level;
    for (int i = 0; i < enemyCount; i++) {
        spawnRandomEnemy(elist, p, n, m, enemyPoolSize);
    }
}

*/

//rand val - declared in main.cpp
extern unsigned seed;
extern mt19937 engine;
extern uniform_int_distribution<int> dist;
#define rand dist(engine) 

bool occupied(int x, int y, player p, vector<entity> elist, vector<pair<int,int>> wall) {
    if (p.x == x && p.y == y) return false;
    for (entity i: elist) {
        if (i.x == x && i.y == y) return false;
    }
    for (auto i: wall) {
        if (i.first == x && i.second == y) return false;
    }

    return true;
}

vector<string> listgen(int pts, vector<pair<int, string>> epool) {
    int temp = rand%(epool.size());
    vector<string> list;

    while (pts - epool[temp].first > 0)
    return {};
}

void director(vector<entity> &elist, vector<pair<int,int>> &wall, player &p, int level, vector<pair<int, string>> epool) {
    //elist is the list of all entities, p is player, level is the danger value
    vector<int> difficulty = {50, 60, 70, 80, 90};
    int pts = difficulty[level];
    vector<string> enemylineup = listgen(pts, epool);

    vector<int> section = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int temp = rand%9;
    //add more stuff
    vector<vector<pair<int,int>>> layout = {
        {},{},{},{}, //empty type layouts are 4 times as likely to spawn
        {{1, 1}, {1, 7}, {3, 7}},
        {{3, 1}, {1, 7}},
        {{1, 1}, {3, 1}},
        {{1, 8}, {2, 5}, {2, 6}, {2, 7}, {2, 8}, {3, 8}},
        {{1, 0}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {3, 0}},
        {{1, 4}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 4}},
        {{1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}, {3, 3}, {3, 4}, {3, 5}},
        {{0, 1}, {0, 2}, {0, 5}, {0, 6}, {0, 7}, {4, 1}, {4, 2}, {4, 3}, {4, 6}, {4, 7}}
    };

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
        p.x = bpx + 2 + rand%3 - 1;
        p.y = bpy + 4 + rand%3 - 1;
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
    //GATE SECTION
    for (int i = 8; i > 5; i--) {
        temp = rand%i;
        //choose section
            int gsection = section[temp];
            section.erase(section.begin() + temp);
        //init section borders
            int bgx = 5 *((gsection - 1)%3);
            int bgy = 9 *((gsection - 1)/3);
        //choose layout
            temp = rand%12;
            for (auto &i : layout[temp]) {
                wall.emplace_back(bgx + i.first, bgy + i.second);
            }
        //spawn gate
            int x = 0, y = 0;
            for (int i = 0; i < 50; i++) {
                x = rand%5 + bgx; y = rand%9 + bgy;
                if (occupied(x, y, p, elist, wall)) break;
                x = 1; y = 2;
            }
            entity gate = {x, y, "gate", 'G', 3, -1, -1, -1};
            elist.push_back(gate);
        //spawn enemy
//ADDDDHERE   
    }

    //COIN SECTION
    for (int i = 5; i > 3; i--) {
        temp = rand%i;
    //choose section
        int gsection = section[temp];
        section.erase(section.begin() + temp);
    //init section borders
        int bgx = 5 *((gsection - 1)%3);
        int bgy = 9 *((gsection - 1)/3);
    //choose layout
        temp = rand%12;
        for (auto &i : layout[temp]) {
            wall.emplace_back(bgx + i.first, bgy + i.second);
        }
    //spawn coins
        int x = 0, y = 0;
        for (int i = 0; i < 50; i++) {
            x = rand%5 + bgx; y = rand%9 + bgy;
            if (occupied(x, y, p, elist, wall)) break;
            x = 1; y = 2;
        }
        entity coin = {x, y, "coin", 'c', 3, -1, -1, -1};
        elist.push_back(coin);
    //spawn enemy
//ADDDDHERE   
    }

    //ENEMY SECTION
    for (int i = 3; i > 0; i--) {
        temp = rand%i;
    //choose section
        int gsection = section[temp];
        section.erase(section.begin() + temp);
    //init section borders
        int bgx = 5 *((gsection - 1)%3);
        int bgy = 9 *((gsection - 1)/3);
    //choose layout
        temp = rand%12;
        for (auto &i : layout[temp]) {
            wall.emplace_back(bgx + i.first, bgy + i.second);
        }
    //spawn enemy
//ADDDDHERE   
    }
}
