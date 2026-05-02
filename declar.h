#ifndef declar
#define declar

//libraries used in project, namespace std declaration for easy access
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#ifdef __APPLE__
#include <curses.h>
#else
#include <ncurses/ncurses.h>
#endif

//initialize randomness
#include <random>
#include <chrono>
using namespace std;

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
};

//initialize colors
inline void colorscale() {
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_WHITE, COLOR_BLACK);
    }
}


#endif //declar
