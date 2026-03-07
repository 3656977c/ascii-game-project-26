#ifndef declar
#define declar

//libraries used in project, namespace std declaration for easy access
#include <iostream>
#include <vector>
#include <map>
#include <ncurses/ncurses.h> //for getch()
using namespace std;

//colors for cout text
void createcolor() {
    if (has_colors()) {//is color supported?
        start_color();  
        init_pair(1, COLOR_RED, COLOR_BLACK); 
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    }
}
#endif //declar