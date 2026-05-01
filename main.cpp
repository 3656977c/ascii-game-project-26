#include "declar.h"
int n = 6;
int m = 10;
int open = 0;

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
    if (open < 3) mvprintw(0, 0, "WASD to move, Q to quit");
    else mvprintw(0, 0, "You Beat This Level!");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            mvaddch(2+i, 2*j, c | COLOR_PAIR(4));
        }
    }

    for (auto i: e) {
        if (i.type != "gate" && i.type != "coin") continue;
        if (i.c == -1) continue;
        mvaddch(2+i.x, 2*i.y, i.s | COLOR_PAIR(i.c));
    }

    for (auto i: e) {
        if (i.type == "gate" || i.type == "coin") continue;
        if (i.c == -1) continue;
        mvaddch(2+i.x, 2*i.y, i.s | COLOR_PAIR(i.c));
    }
    mvaddch(2+p.x, 2*p.y, p.s | COLOR_PAIR(1));
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

void bGate(entity &e, entity p) {
    if (e.x == p.x && e.y == p.y && e.t == -1) {
        open++;
        e.t = 0;
        e.c = 1;
    }
}

void bCoin(entity &e, entity p) {
    if (e.x == p.x && e.y == p.y && e.c > -1) {
        e.t = 0;
        e.c = -1;
    }
}

void updateentities(vector<entity> &e, entity p) {
    for (auto &i: e) {
        if (i.type == "bouncer") bBouncer(i);
        if (i.type == "follow") bFollow(i, p);
        if (i.type == "gate") bGate(i, p);
        if (i.type == "coin") bCoin(i, p);
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
    entity player{0,0,"player", '@', -1, -1,-1,-1};

    entity bounce{2, 6, "follow", '!', 2, 1, 1, -1};
    entity fol{1, 2, "bouncer", 'O', 2, 1, 1, 0};
    elist.push_back(bounce);
    elist.push_back(fol);

    entity gate1{5, 9, "gate", 'H', 3, -1, -1, -1};
    elist.push_back(gate1);
    entity gate2{5, 1, "gate", 'H', 3, -1, -1, -1};
    elist.push_back(gate2);
    entity gate3{0, 8, "gate", 'H', 3, -1, -1, -1};
    elist.push_back(gate3);

    entity coin1{3, 4, "coin", 'c', 3, -1, -1, -1};
    elist.push_back(coin1);
    entity coin2{3, 6, "coin", 'c', 3, -1, -1, -1};
    elist.push_back(coin2);

    while (state == "run") {
        display(n, m, elist, player);
        updateplayer(player);
        updateentities(elist, player);
    }
    endwin();
}