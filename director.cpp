#include "declar.h"
#include "director.h"
//#include "enemies.cpp"

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
