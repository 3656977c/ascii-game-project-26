#ifndef declar
#define declar

//libraries used in project, namespace std declaration for easy access
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <ncurses/ncurses.h>
using namespace std;

//initialize colors
inline void colorscale() {
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    }
}

//Defined funcitons
#define x first
#define y second

#endif //declar