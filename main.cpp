#include "declar.h"
#include "upgrades.h"
int n = 15;
int m = 27;
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

void display(int n, int m, vector<entity> e, player p, int health, int maxHealth) {
    char c = '.';
    int a = 2, b = 4; //map displacement
    erase();
    if (open < 3) mvprintw(0, 0, "WASD to move, Q to quit");
    else mvprintw(0, 0, "You Beat This Level!");
    mvprintw(1, 0, "Health: %d/%d", health, maxHealth);
    for (int i = 0; i < n+2; i++) {
        mvaddch(3+i+a-1, 2*(-1)+b, '|' | COLOR_PAIR(4));
        mvaddch(3+i+a-1, 2*m+b, '|' | COLOR_PAIR(4));
    }
    for (int i = -1; i < m+1; i++) {
        mvaddch(3+a-1, 2*i+b, '-' | COLOR_PAIR(4));
        mvaddch(3+n+a, 2*i+b, '-' | COLOR_PAIR(4));
    }

  

    for (auto i: e) {
        if (i.type != "gate" && i.type != "coin") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }

    for (auto i: e) {
        if (i.type == "gate" || i.type == "coin") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }
    mvaddch(3+p.x+a, 2*p.y+b, '@' | COLOR_PAIR(1));
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

void bFollow(entity &e, player p) {
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

void bGate(entity &e, player p) {
    if (e.x == p.x && e.y == p.y && e.t == -1) {
        open++;
        e.t = 0;
        e.c = 1;
    }
}

void bCoin(entity &e, player p) {
    if (e.x == p.x && e.y == p.y && e.c > -1) {
        e.t = 0;
        e.c = -1;
    }
}

void updateentities(vector<entity> &e, player p) {
    for (auto &i: e) {
        if (i.type == "bouncer") bBouncer(i);
        if (i.type == "follow") bFollow(i, p);
        if (i.type == "gate") bGate(i, p);
        if (i.type == "coin") bCoin(i, p);
    }
}

bool isDamagingEnemy(entity e) {
    return e.c != -1 && (e.type == "bouncer" || e.type == "follow" || e.type == "projectile");
}

int getPlayerHitIndex(vector<entity> e, player p) {
    for (int i = 0; i < (int)e.size(); i++) {
        if (isDamagingEnemy(e[i]) && e[i].x == p.x && e[i].y == p.y) {
            return i;
        }
    }

    return -1;
}

bool isPickup(entity e) {
    return e.c != -1 && (e.type == "coin" || e.type == "time_stop" || e.type == "kill_pickup" || e.type == "swap_pickup");
}

void clampHealth(int &health) {
    if (health < 0) health = 0;
}

bool applyPlayerHit(UpgradeState &upgrades, int &health, vector<entity> &elist, int enemyIndex) {
    onPlayerHit(upgrades, health, elist, enemyIndex);
    clampHealth(health);
    return health <= 0;
}

void collectPickups(UpgradeState &upgrades, int &health, int maxHealth, vector<entity> &elist, player p) {
    for (auto &i: elist) {
        if (isPickup(i) && i.x == p.x && i.y == p.y) {
            if (i.type == "kill_pickup") {
                killFirstEnemy(elist);
            }

            i.t = 0;
            i.c = -1;
            onPickupCollected(upgrades, health, maxHealth, elist);
        }
    }
}

void showDeathScreen() {
    erase();
    mvprintw(0, 0, "You died!");
    mvprintw(1, 0, "Press any key to exit.");
    refresh();
    nodelay(stdscr, FALSE);
    timeout(-1);
    getch();
}

void updateplayer(player &p, string &state) {
    int c = getch();
    switch (c) {
        case 'w': if (p.x > 0) p.x--; break;
        case 'a': if (p.y > 0) p.y--; break;
        case 's': if (p.x < n-1) p.x++; break;
        case 'd': if (p.y < m-1) p.y++; break;
        case 'q': state = "quit"; break;
    }
}

signed main() {
    //initialize size of map

    initialize();
    colorscale();
    string state = "run";
    int maxHealth = 3;
    int health = maxHealth;
    bool stageClearApplied = false;
    UpgradeState upgrades;

    vector<entity> elist;
    player player{0,0};

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
        display(n, m, elist, player, health, maxHealth);
        updateplayer(player, state);
        if (state != "run") break;

        collectPickups(upgrades, health, maxHealth, elist, player);

        bool hitThisTurn = false;
        int enemyIndex = getPlayerHitIndex(elist, player);

        if (enemyIndex != -1) {
            if (applyPlayerHit(upgrades, health, elist, enemyIndex)) {
                state = "dead";
                break;
            }
            hitThisTurn = true;
        }

        updateentities(elist, player);

        collectPickups(upgrades, health, maxHealth, elist, player);

        enemyIndex = getPlayerHitIndex(elist, player);
        if (!hitThisTurn && enemyIndex != -1) {
            if (applyPlayerHit(upgrades, health, elist, enemyIndex)) {
                state = "dead";
            }
        }

        if (open >= 3 && !stageClearApplied) {
            onStageClear(upgrades, health, maxHealth);
            stageClearApplied = true;
        }

        nextUpgradeTurn(upgrades);
    }

    if (state == "dead") {
        display(n, m, elist, player, health, maxHealth);
        showDeathScreen();
    }

    endwin();
}
