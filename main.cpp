#include "declar.h"

// Draw the board using ncurses (not std::cout) once curses mode is active.
void display(const vector<char> &gmap, int n, int x, int y, int ex, int ey) {
    clear();
    mvprintw(0, 0, "WASD to move, Q to quit");
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            int idx = col + row * n;
            char cell = gmap[idx];
            if (idx == x + y * n) {
                cell = 'P';
                mvaddch(row + 2, col * 2, cell | COLOR_PAIR(2));
            } else if (idx == ex + ey * n) {
                cell = 'E';
                mvaddch(row + 2, col * 2, cell | COLOR_PAIR(1));
            } else mvaddch(row + 2, col * 2, cell);
        }
    }
    refresh();
}

void enemy(int x, int y, int &ex, int &ey) {
    if (abs(ex-x) > abs(ey-y)) {
        if (x > ex) ex++;
        else ex--;
    } else {
        if (y > ey) ey++;
        else ey--;
    }
}

signed main() {
    int n;
    cin >> n;
    vector<char> gmap(n*n, '-');
    int x=0, y=0;
    int ex = n-1, ey = n-1;
    gmap[0] = 'P';
    gmap[n*n-1] = 'E';
    bool state = true;
    int co = 0;

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);
    timeout(-1);

    if (has_colors()) {     // Check if terminal supports colors
        start_color();      // Enable color
        init_pair(1, COLOR_RED, COLOR_BLACK); // Define color pair 1 (red foreground, black background)
        init_pair(2, COLOR_GREEN, COLOR_BLACK); // Define color pair 2 (green foreground, black background)
    }

    while (state) {
        display(gmap, n, x, y, ex, ey);
        gmap[x+y*n] = '-';
        int c = getch();
        if (c == 'w' && y != 0) y--;
        else if (c == 's' && y != n - 1) y++;
        else if (c == 'a' && x != 0) x--;
        else if (c == 'd' && x != n - 1) x++;
        else if (c == 'q' || c == 'Q') state = false;
        gmap[x+y*n] = 'P';
        gmap[ex+ey*n] = '-';
        if (co%2 == 0) {
            enemy(x,y,ex,ey);
        }
        co++;
        gmap[ex+ey*n] = 'E';
    }

    endwin();
}