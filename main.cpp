#include "declar.h"
#include "upgrades.h"
#include "enemies.h"
#include "director.h"
#include "menu.h"
#include <algorithm>
int n = 15; //enforced # of columns
int m = 27; //enforced # of rows
int open = 0; //global "open" val
int coin = 0; //global "coin" val

//initialize randomness
unsigned seed = chrono::system_clock::now().time_since_epoch().count();   
mt19937 engine(seed);
uniform_int_distribution<int> dist(0,7559); //7560 is divisible from 1-10, 15, and 27
#define rand dist(engine) 

void initialize() {
    initscr(); //initialize ncurses
    cbreak(); //take in any input immediately without needing to press enter
    noecho(); //dont show typed keys
    curs_set(0); //hide text cursor
    keypad(stdscr, TRUE); //allow arrowkeys
    //nodelay(stdscr, FALSE); //makes getch() not wait for key to be pressed
    //timeout(-1); //how long getch() waits for input
    keypad(stdscr, FALSE);
    timeout(200);
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
void display(gamestate g, player p) {
    int a = 2, b = 4; //map displacement
    erase();
    if (open < 3) mvprintw(0, 0, "WASD to move, ESC to quit");
    else mvprintw(0, 0, "You Beat This Level!");
    mvprintw(1, 0, "Level: %d/6  Health: %d/%d", g.level, p.health, p.maxHealth);
    
    //print borders
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

    //draw walls
    for (auto i: g.wlist) {
        mvaddch(3+i.first+a, 2*i.second+b, 'W' | COLOR_PAIR(10));
        mvaddch(3+i.first+a, 2*i.second+b-1, 'W' | COLOR_PAIR(10));
        mvaddch(3+i.first+a, 2*i.second+b + 1, 'W' | COLOR_PAIR(10));
    }

    //maybe create a new func to help create "red" attack areas
    drawMushroomAttackRadius(g.elist,g.wlist,a,b);
  
    //print coins and gates first
    for (auto i: g.elist) {
        if (i.type != "gate" && i.type != "coin") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }

    //print projectiles
    for (auto i: g.elist) {
        if (i.type != "projectile") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }

    //print everything else
    for (auto i: g.elist) {
        if (i.type == "gate" || i.type == "coin" || i.type == "projectile") continue;
        if (i.c == -1) continue;
        mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
    }

    //print player and refresh
    mvaddch(3+p.x+a, 2*p.y+b, '@' | COLOR_PAIR(1));
    refresh();
}

//cleaned, dont touch for now
void genLevel(gamestate &g, player &p) {
    g.elist.clear();
    g.wlist.clear();
    open = 0;
    coin = 0;
    director(g, p);
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

bool applyPlayerHit(UpgradeState &upgrades, player &p, vector<entity> &elist, int enemyIndex) {
    onPlayerHit(upgrades, p, elist, enemyIndex);
    p.health = max(0, p.health);
    return p.health <= 0;
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

void updateplayer(player &p, UpgradeState &upgrades, gamestate &g) {
    int c = tolower(getch());
    napms(100);
    bool canLoop = upgrades.loopAroundMap && upgrades.loopCharges > 0;
    int oldX = p.x;
    int oldY = p.y;

    switch (c) {
        case 'w': if (p.x > 0 || canLoop) p.x--; break;
        case 'a': if (p.y > 0 || canLoop) p.y--; break;
        case 's': if (p.x < n-1 || canLoop) p.x++; break;
        case 'd': if (p.y < m-1 || canLoop) p.y++; break;
        case 27: g.state = "dead"; break;
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

    if (isWall(g.wlist, p.x, p.y)) {
        p.x = oldX;
        p.y = oldY;
    }
}

string playGame() {
    gamestate gmst;
    gmst.state = "run";
    gmst.level = 1;

    UpgradeState upgrades;
    uppick pickedUpgrades;
    player player{0,0,3,3};

    //initialization for updater, ill consider moving it to gamestate
    int turns = 0;
    pair<int,entity> targ = gmst.epool[rand%3];

    genLevel(gmst, player);
    while (gmst.state == "run") {
        display(gmst, player);
        updateplayer(player, upgrades, gmst);
        if (gmst.state != "run") break;

        collectPickups(upgrades, gmst.elist, player);
  

        if (upgrades.timeStopTurns > 0) {
            upgrades.timeStopTurns--;
        } else {
            updateentities(gmst, player, upgrades);
        }
        removeInactiveEntities(gmst.elist);

        collectPickups(upgrades, gmst.elist, player);
        removeInactiveEntities(gmst.elist);

        int enemyIndex = getPlayerHitIndex(gmst.elist, player);
        if (enemyIndex != -1) {
            if (applyPlayerHit(upgrades, player, gmst.elist, enemyIndex)) {
                gmst.state = "dead";
            }
            removeInactiveEntities(gmst.elist);
        }
        if (open >= 3) {
            if (gmst.level >= 6) {
                gmst.state = "win";
                break;
            }

            int selectedUpgrade = chooseUpgrade(gmst.level, player, pickedUpgrades);
            pickedUpgrades.push_back(selectedUpgrade);

            if (selectedUpgrade == 3) {
                upgrades.healOneEndStage = true;
            }
            else if (selectedUpgrade == 4) {
                upgrades.healHalfEndStage = true;
            }

            onStageClear(upgrades, player);

            gmst.level++;
            genLevel(gmst, player);
            addCarriedUpgradePickups(upgrades, gmst.elist, gmst.wlist);
            applyUpgrade(selectedUpgrade, upgrades, player, gmst.elist, gmst.wlist, n, m);
        }

        nextUpgradeTurn(upgrades);
        updater(turns, targ, gmst, player);
    }

    if (gmst.state == "dead") {
<<<<<<< HEAD
        display(gmst, player, health, maxHealth);
        pickedUpgrades.release();
        return "dead";
=======
        display(gmst, player);
        showDeathScreen();
>>>>>>> 5a0a335e23071f1bd9fbc6ebecb1bb4691ee8074
    }

    if (gmst.state == "win") {
        pickedUpgrades.release();
        return "win";
    }

    pickedUpgrades.release();
    return "quit";
}

signed main() {
    initialize();
    colorscale();

    while (true) {
        string menuChoice = showMainMenu();

        if (menuChoice == "quit") {
            break;
        }

        string result = playGame();
        string endChoice = showEndMenu(result);

        while (endChoice == "restart") {
            result = playGame();
            endChoice = showEndMenu(result);
        }

        if (endChoice == "quit") {
            break;
        }
    }

    endwin();
}
