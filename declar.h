#ifndef declar
#define declar

//libraries used in project, namespace std declaration for easy access
#include <cstdlib>
#include <string>
#include <random>
#include <chrono>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#ifdef __APPLE__
#include <curses.h>
#else
#include <ncurses/ncurses.h>
#endif

using namespace std;

//player class
class player {
    public:
        int x;
        int y;
};

//entity and projectile class
class entity {
    public:
        int x;  //col
        int y;  //row
        string type; //type of object (defines behaviour)
        char s; //symbol
        int c; //color
        int ax; //horizontal velocity
        int ay; //vertical velocity
        int t; //timer
        pair<char, int> spawnl; //spawn looks
        string spawntype; //spawn type
        int spawnval[3]; //spawn parameters
};

//information on game
class gamestate {
    private:
        entity bcer = {0, 0, "bouncer", 'O', 2, 1, 1, -1};
        entity ghst = {0, 0, "ghost", '%', 2, -1, -1, 0};
        entity shtr = {0, 0, "shooter",'&', 2, 0, 0, 1};
    public:
        vector<entity> elist;

        int wn = 1;
        vector<pair<int,int>> wlist;

        vector<pair<int, entity>> epool = {{2, bcer}, {3, ghst}, {5, shtr}};
        int level;
        string state;
};

class uppick {
    public:
        int upsize = 1;
        int uit = 0;
        int * u = (int *)malloc(upsize * sizeof(int));

        void push_back(int x) {
            if (uit >= upsize) {
                upsize*=2;
                u = (int *)realloc(u, upsize * (sizeof(int)));
            }
            u[uit] = x;
            uit++;
        }

        void release() {
            uit = 0;
            upsize = 1;
            free(u);
            u = nullptr;
        }
};

//initialize colors
inline void colorscale() {
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_WHITE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(10, COLOR_WHITE, COLOR_WHITE);  //wall
        init_pair(20, COLOR_WHITE, COLOR_RED);    //mushroom attack area
        
    }
}


#endif //declar
