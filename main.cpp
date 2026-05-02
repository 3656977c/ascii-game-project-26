#include "declar.h"
#include "upgrades.h"
#include "enemies.h"
#include "director.h"
#include <algorithm>
int n = 15;
int m = 27;
int open = 0;

//initialize randomness
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();   
    mt19937 engine(seed);
    uniform_int_distribution<int> dist(0,7559);
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
void drawMushroomAttackRadius(vector<entity> e, vector<pair<int,int>> w, int a, int b) {
    for (auto mushroom : e) {
        if (mushroom.type != "mushroom" || mushroom.c == -1) {
            continue;
        }

        for (int dx = -2; dx <= 2; dx++) {
            for (int dy = -2; dy <= 2; dy++) {
                if (dx * dx + dy * dy <= 4) {
                    int tileX = mushroom.x + dx;
                    int tileY = mushroom.y + dy;

                    if (tileX < 0 || tileX >= n || tileY < 0 || tileY >= m) {
                        continue;
                    }

                    if (isWall(w, tileX, tileY)) {
                        continue;
                    }

                    mvaddch(3 + tileX + a, 2 * tileY + b, '.' | COLOR_PAIR(20));
                    mvaddch(3 + tileX + a, 2 * tileY + b - 1, ' ' | COLOR_PAIR(20));
                    mvaddch(3 + tileX + a, 2 * tileY + b + 1, ' ' | COLOR_PAIR(20));
                }
            }
        }
    }
}
void display(int n, int m, vector<entity> e, vector<pair<int,int>> w, player p, int health, int maxHealth, int level) {
    char c = '.';
    int a = 2, b = 4; //map displacement
    erase();
    if (open < 3) mvprintw(0, 0, "WASD to move, Q to quit %d, %d", p.x, p.y);
    else mvprintw(0, 0, "You Beat This Level!");
    mvprintw(1, 0, "Level: %d/6  Health: %d/%d", level, health, maxHealth);

    for (int i = 0; i < n+2; i++) {
        mvaddch(3+i+a-1, 2*(-1)+b-1, '|' | COLOR_PAIR(10));
        mvaddch(3+i+a-1, 2*(-1)+b, '|' | COLOR_PAIR(10));
        mvaddch(3+i+a-1, 2*m+b, '|' | COLOR_PAIR(10));
        mvaddch(3+i+a-1, 2*m+b+1, '|' | COLOR_PAIR(10));
    }
    for (int i = -1; i < m+1; i++) {
        mvaddch(3+a-1, 2*i+b, '-' | COLOR_PAIR(10));
        mvaddch(3+a-1, 2*i+b - 1, '-' | COLOR_PAIR(10));
        mvaddch(3+n+a, 2*i+b, '-' | COLOR_PAIR(10));
         mvaddch(3+n+a, 2*i+b - 1, '-' | COLOR_PAIR(10));
    }
    for (auto i: w) {
        mvaddch(3+i.first+a, 2*i.second+b, 'W' | COLOR_PAIR(10));
        mvaddch(3+i.first+a, 2*i.second+b-1, 'W' | COLOR_PAIR(10));
        mvaddch(3+i.first+a, 2*i.second+b + 1, 'W' | COLOR_PAIR(10));
    }
    drawMushroomAttackRadius(e,w,a,b);
  

    for (auto i: e) {
        if (i.type != "gate" && i.type != "coin") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }

    for (auto i: e) {
        if (i.type == "gate" || i.type == "coin") continue;
        if (i.type != "projectile") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }

    for (auto i: e) {
        if (i.type == "gate" || i.type == "coin") continue;
        if (i.type == "projectile") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }
    mvaddch(3+p.x+a, 2*p.y+b, '@' | COLOR_PAIR(1));
    refresh();
}

void setupLevel(vector<entity> &elist, player &p, int level, vector<pair<int, entity>> epool) {
    vector<pair<int, int>> ignoredWalls;
    elist.clear();
    open = 0;
    director(elist, ignoredWalls, p, level - 1, epool);

}

void setupLevel(vector<entity> &elist, vector<pair<int, int>> &wlist, player &p, int level, vector<pair<int, entity>> epool) {
    elist.clear();
    wlist.clear();
    open = 0;
    director(elist, wlist, p, level - 1, epool);
}

void addCarriedUpgradePickups(UpgradeState &upgrades, vector<entity> &elist, vector<pair<int, int>> &wlist) {
    if (upgrades.spawnTimeStopPickups) {
        spawnPickup(elist, wlist, n, m, "time_stop", 'T');
        spawnPickup(elist, wlist, n, m, "time_stop", 'T');
    }

    if (upgrades.spawnKillPickups) {
        spawnPickup(elist, wlist, n, m, "kill_pickup", 'K');
        spawnPickup(elist, wlist, n, m, "kill_pickup", 'K');
    }

    if (upgrades.spawnSwapPickups) {
        spawnPickup(elist, wlist, n, m, "swap_pickup", 'S');
        spawnPickup(elist, wlist, n, m, "swap_pickup", 'S');
    }
}

bool hasUpgrade(vector<int> pickedUpgrades, int upgradeNumber) {
    for (int upgrade: pickedUpgrades) {
        if (upgrade == upgradeNumber) {
            return true;
        }
    }

    return false;
}

vector<int> getUpgradeChoices(vector<int> pickedUpgrades) {
    vector<int> choices;

    while ((int)choices.size() < 3) {
        int upgradeNumber = rand % 15 + 1;
        bool alreadyChosen = false;

        for (int choice: choices) {
            if (choice == upgradeNumber) alreadyChosen = true;
        }

        if (hasUpgrade(pickedUpgrades, upgradeNumber)) alreadyChosen = true;

        if (!alreadyChosen) choices.push_back(upgradeNumber);
    }

    return choices;
}

int chooseUpgrade(int level, int health, int maxHealth, vector<int> pickedUpgrades) {
    vector<int> choices = getUpgradeChoices(pickedUpgrades);

    while (true) {
        erase();
        mvprintw(0, 0, "Level %d cleared!", level);
        mvprintw(1, 0, "Health: %d/%d", health, maxHealth);
        mvprintw(3, 0, "Choose an upgrade:");

        for (int i = 0; i < (int)choices.size(); i++) {
            string name = getUpgradeName(choices[i]);
            mvprintw(5 + i, 0, "%d. %s", i + 1, name.c_str());
        }

        mvprintw(9, 0, "Press 1, 2, or 3");
        refresh();

        int c = getch();
        if (c >= '1' && c <= '3') {
            return choices[c - '1'];
        }
    }
}

void showWinScreen() {
    erase();
    mvprintw(0, 0, "You beat all 6 levels!");
    mvprintw(1, 0, "Press any key to exit.");
    refresh();
    nodelay(stdscr, FALSE);
    timeout(-1);
    getch();
}

bool isPickup(entity e) {
    return e.c != -1 && (e.type == "coin" || e.type == "time_stop" || e.type == "kill_pickup" || e.type == "swap_pickup");
}

void removeInactiveEntities(vector<entity> &elist) {
    for (int i = (int)elist.size() - 1; i >= 0; i--) {
        if (elist[i].c == -1) {
            elist.erase(elist.begin() + i);
        }
    }
}

void clampHealth(int &health) {
    if (health < 0) health = 0;
}

bool applyPlayerHit(UpgradeState &upgrades, int &health, vector<entity> &elist, int enemyIndex) {
    onPlayerHit(upgrades, health, elist, enemyIndex);
    clampHealth(health);
    return health <= 0;
}

bool useSwapPickup(vector<entity> &elist, player &p, int swapIndex) {
    for (int i = 0; i < (int)elist.size(); i++) {
        if (i != swapIndex && elist[i].type == "swap_pickup" && elist[i].c != -1) {
            int oldPlayerX = p.x;
            int oldPlayerY = p.y;

            p.x = elist[i].x;
            p.y = elist[i].y;
            elist[i].x = oldPlayerX;
            elist[i].y = oldPlayerY;
            elist[swapIndex].t = 0;
            elist[swapIndex].c = -1;
            return true;
        }
    }

    return false;
}

void collectPickups(UpgradeState &upgrades, int &health, int maxHealth, vector<entity> &elist, player &p) {
    for (int i = 0; i < (int)elist.size(); i++) {
        if (isPickup(elist[i]) && elist[i].x == p.x && elist[i].y == p.y) {
            bool collected = true;

            if (elist[i].type == "kill_pickup") {
                killFirstEnemy(elist);
            }

            if (elist[i].type == "time_stop") {
                upgrades.timeStopPickupCount++;
                if (upgrades.timeStopPickupCount >= 2) {
                    upgrades.timeStopTurns = 2;
                    upgrades.timeStopPickupCount = 0;
                }
            }

            if (elist[i].type == "swap_pickup") {
                collected = useSwapPickup(elist, p, i);
            } else {
                elist[i].t = 0;
                elist[i].c = -1;
            }

            if (collected) {
                onPickupCollected(upgrades, health, maxHealth, elist);
            }
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

void updateplayer(player &p, string &state, UpgradeState &upgrades, vector<pair<int,int>> &w) {
    int c = getch();
    bool canLoop = upgrades.loopAroundMap && upgrades.loopCharges > 0;
    int oldX = p.x;
    int oldY = p.y;

    switch (c) {
        case 'w': if (p.x > 0 || canLoop) p.x--; break;
        case 'a': if (p.y > 0 || canLoop) p.y--; break;
        case 's': if (p.x < n-1 || canLoop) p.x++; break;
        case 'd': if (p.y < m-1 || canLoop) p.y++; break;
        case 'q': state = "quit"; break;
    }

    if (canLoop) {
        bool used = false;

        if (p.x < 0) {
            p.x = n - 1;
            used = true;
        } else if (p.x >= n) {
            p.x = 0;
            used = true;
        }

        if (p.y < 0) {
            p.y = m - 1;
            used = true;
        } else if (p.y >= m) {
            p.y = 0;
            used = true;
        }

        if (used) upgrades.loopCharges--;
    }

    if (isWall(w, p.x, p.y)) {
        p.x = oldX;
        p.y = oldY;
    }
}

signed main() {
    //initialize size of map

    initialize();
    colorscale();
    string state = "run";
    int level = 1;
    int maxHealth = 3;
    int health = maxHealth;
    UpgradeState upgrades;

    vector<entity> elist;
    vector<pair<int, int>> wlist;
    vector<int> pickedUpgrades;

    //enemies with defaul start vals
    entity bcer = {0, 0, "bouncer", 'O', 2, 1, 1, -1};
    entity ghst = {0, 0, "ghost", '%', 2, -1, -1, 0};
    entity shtr = {0, 0, "shooter",'&', 2, 0, 0, 1};
    vector<pair<int, entity>> epool = {{2, bcer}, {3, ghst}, {5, shtr}}; //enemy pool
    player player{0,0};
    int turns = 0;
    pair<int,entity> targ = epool[rand%3];
    setupLevel(elist, wlist, player, level, epool);
    
    while (state == "run") {
        display(n, m, elist, wlist, player, health, maxHealth, level);
        updateplayer(player, state, upgrades, wlist);
        if (state != "run") break;

        collectPickups(upgrades, health, maxHealth, elist, player);
        removeInactiveEntities(elist);

        if (upgrades.timeStopTurns > 0) {
            upgrades.timeStopTurns--;
        } else {
            updateentities(elist, player, wlist, upgrades);
        }
        removeInactiveEntities(elist);

        collectPickups(upgrades, health, maxHealth, elist, player);
        removeInactiveEntities(elist);

        int enemyIndex = getPlayerHitIndex(elist, player);
        if (enemyIndex != -1) {
            if (applyPlayerHit(upgrades, health, elist, enemyIndex)) {
                state = "dead";
            }
            removeInactiveEntities(elist);
        }
        if (open >= 3) {
            onStageClear(upgrades, health, maxHealth);

            if (level >= 6) {
                state = "win";
                break;
            }

            int selectedUpgrade = chooseUpgrade(level, health, maxHealth, pickedUpgrades);
            pickedUpgrades.push_back(selectedUpgrade);
            level++;
            setupLevel(elist, wlist, player, level, epool);
            addCarriedUpgradePickups(upgrades, elist, wlist);
            applyUpgrade(selectedUpgrade, upgrades, health, maxHealth, elist, wlist, n, m);
        }

        nextUpgradeTurn(upgrades);
        updater(turns, targ, epool, player, elist, wlist);
    }

    if (state == "dead") {
        display(n, m, elist, wlist, player, health, maxHealth, level);
        showDeathScreen();
    }

    if (state == "win") {
        showWinScreen();
    }

    endwin();
}
