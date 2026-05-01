#include "declar.h"
int n = 6;
int m = 10;

//initialize randomness
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();   
    mt19937 engine(seed);
    uniform_int_distribution<int> dist(0,99);
    #define rand dist(engine) 

void initialize() {
    initscr(); //initialize ncurses
    cbreak(); //take in any input immediately without needing to press enter
    noecho(); //dont show typed keys
    curs_set(0); //hide text cursor
    keypad(stdscr, TRUE); //allow arrowkeys
    nodelay(stdscr, FALSE); //makes getch() not wait for key to be pressed
    timeout(-1); //how long getch() waits for input
}

void display(int n, int m, vector<entity> e, entity p) {
    char c = '.';
    erase();
    mvprintw(0, 0, "WASD to move, Q to quit");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            mvaddch(2+i, 2*j, c | COLOR_PAIR(3));
        }
    }
    mvaddch(2+p.x, 2*p.y, p.c | COLOR_PAIR(1));
    for (auto i: e) {
         mvaddch(2+i.x, 2*i.y, i.c | COLOR_PAIR(2));
    }
    refresh();
}

//!!!!!! Enemy Behaviour (to move to enemies.cpp abnd enemies.h)
void bBouncer(entity &e) {
    if (e.x == 0) e.ax = 1;
    else if (e.x == n - 1) e.ax = -1;
    if (e.y == 0) e.ay = 1;
    else if (e.y == m - 1) e.ay = -1;
    e.x += e.ax;
    e.y += e.ay;
}

void bFollow(entity &e, entity p) {
    if (e.t == 1) {
        if (abs(e.x - p.x)!= 0 && abs(e.y - p.y)!=0) {
            if (rand/50 == 0) {
                if (p.x > e.x) e.x++;
                else e.x--;
            } else {
                if (p.y > e.y) e.y++;
                else e.y--;
            }
        } else {
            if (abs(e.x - p.x) > 0) {
                if (p.x > e.x) e.x++;
                else e.x--;
            } else if (abs(e.y - p.y) > 0) {
                if (p.y > e.y) e.y++;
                else e.y--;
            }
        }
        e.t = 0;
    } else e.t++;
}

void updateentities(vector<entity> &e, entity p) {
    for (auto &i: e) {
        if (i.type == "bouncer") bBouncer(i);
        if (i.type == "follow") bFollow(i, p);
    }
}

void updateplayer(entity &p) {
    int c = getch();
    switch (c) {
        case 'w': if (p.x > 0) p.x--; break;
        case 'a': if (p.y > 0) p.y--; break;
        case 's': if (p.x < n-1) p.x++; break;
        case 'd': if (p.y < m-1) p.y++; break;
        case 'q': endwin();
    }
}

signed main() {
    //initialize size of map

    initialize();
    colorscale();
    string state = "run";

    vector<entity> elist;
    entity player{0,0,"player", '@', -1,-1,-1};

    entity bounce{2, 6, "follow", '!', 1, 1, -1};
    entity fol{1, 2, "bouncer", 'O', 1, 1, 0};
    elist.push_back(bounce);
    elist.push_back(fol);
    while (state == "run") {
        display(n, m, elist, player);
        updateplayer(player);
        updateentities(elist, player);

    }
    endwin();
}