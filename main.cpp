#include "declar.h"

void initialize() {
    initscr(); //initialize ncurses
    cbreak(); //take in any input immediately without needing to press enter
    noecho(); //dont show typed keys
    curs_set(0); //hide text cursor
    keypad(stdscr, TRUE); //allow arrowkeys
    nodelay(stdscr, FALSE); //makes getch() not wait for key to be pressed
    timeout(-1); //how long getch() waits for input
}

void end() {
    endwin(); //stop ncurses
}

void pinput(pair<int, int> &p, int n, string &state) {
    int c = getch();
    switch (c) {
        case 'w':
            if (p.y > 0) p.y--;
            break;
        case 'a':
            if (p.x > 0) p.x--;
            break;
        case 's':
            if (p.y < n-1) p.y++;
            break;
        case 'd':
            if (p.x < n-1) p.x++;
            break;
        default:
            state="stop";
    }
}

void enemy(pair<int, int> p, pair<int, int> &e) {
    if (abs(e.x - p.x) > abs(e.y - p.y)) {
        if (p.x > e.x) e.x++;
        else e.x--;
    } else {
        if (p.y > e.y) e.y++;
        else e.y--;
    }
}

void display( map<string, pair<int, int>> e, int n, char c) {
    clear();
    mvprintw(0, 0, "WASD to move, Q to quit");
    for (int i = 0; i < n*n; i++) {
        mvaddch(2 + (i / n), 2 * (i % n), c | COLOR_PAIR(3));
    }
    
    for (auto i: e) {
        if (i.first == "player") {
            mvaddch(2 + i.second.y, 2 * i.second.x, 'P' | COLOR_PAIR(1));
        }
        if (i.first == "enemy") {
            mvaddch(2 + i.second.y, 2 * i.second.x, 'E' | COLOR_PAIR(2));
        }
    }
    refresh();
}


signed main() {
    //initialize size of map
    int n; cin >> n;
    
    initialize();
    colorscale();

    //vector<char> gmap(n*n, '-'); not using this for now
    map<string, pair<int, int>> e; //entity list
    e["player"] = {0,0};
    e["enemy"] = {n-1, n-1};
    string state = "run";
    int tn = 0; //turn number

    while (state == "run") {
        tn++;
        display(e,n,'-');
        pinput(e["player"], n, state);
        if (tn%2 == 1) enemy(e["player"], e["enemy"]);
    }
    end();
}