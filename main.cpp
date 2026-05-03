#include "declar.h"
#include "upgrades.h"
#include "enemies.h"
#include "director.h"
#include "menu.h"

int n = 15; //enforced # of columns
int m = 27; //enforced # of rows
int open = 0; //global "open" val
int coin = 0; //global "coin" val
int framems = 100; //how long one frame is

//initialize randomness
unsigned seed = chrono::system_clock::now().time_since_epoch().count();   
mt19937 engine(seed);
uniform_int_distribution<int> dist(0,7559); //7560 is divisible from 1-10, 15, and 27
#define rand dist(engine) 

//for updater
int turns;
pair<int,entity> targ;

void initialize() {
    initscr(); //initialize ncurses
    cbreak(); //take in any input immediately without needing to press enter
    noecho(); //dont show typed keys
    curs_set(0); //hide text cursor
    keypad(stdscr, TRUE); //allow arrowkeys
    keypad(stdscr, FALSE);
    timeout(framems);
}

//print the screen for the player to see
void display(gamestate g, player p, uppick pickedUpgrades) {
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
    // draw harming tiles first so enemies/player appear above them
    for (auto i: g.elist) {
        if (i.type != "harming tile") continue;
        if (i.c == -1) continue;

        mvaddch(3+i.x+a, 2*i.y+b, ' ' | COLOR_PAIR(20));
        mvaddch(3+i.x+a, 2*i.y+b-1, ' ' | COLOR_PAIR(20));
        mvaddch(3+i.x+a, 2*i.y+b+1, ' ' | COLOR_PAIR(20));
}
  
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

  for (auto i: g.elist) {
    if (
        i.type == "gate" ||
        i.type == "coin" ||
        i.type == "projectile" ||
        i.type == "harming tile"
    ) continue;

    if (i.c == -1) continue;
    mvaddch(3+i.x+a, 2*i.y+b, i.s | COLOR_PAIR(i.c));
}

    //print player and refresh
    mvaddch(3+p.x+a, 2*p.y+b, '@' | COLOR_PAIR(1));
    refresh();

    mvprintw(n+a+5, 0, "Collected upgrades: ( %d picked up so far)", pickedUpgrades.uit);
    for (int i = 0; i < pickedUpgrades.uit; i++) {
         mvprintw(n+a+5+i+1, 0, "%s", getUpgradeName(pickedUpgrades.u[i]).c_str());
    }
}

//level generation (goes into director.cpp)
void genLevel(gamestate &g, player &p) {
    g.elist.clear();
    g.wlist.clear();
    open = 0;
    coin = 0;
    turns = 0;
    targ = g.epool[rand%3];
    director(g, p);
}

//Win Screen
void showWinScreen() {
    erase();
    mvprintw(0, 0, "Congradulations! You beat all 6 levels!");
    mvprintw(1, 0, "Press any key to exit.");
    refresh();
    nodelay(stdscr, FALSE);
    timeout(-1);
    getch();
}

//Apply a hit onto the player
bool applyPlayerHit(UpgradeState &upgrades, player &p, vector<entity> &elist, int enemyIndex) {
    onPlayerHit(upgrades, p, elist, enemyIndex);
    p.health = max(0, p.health);
    return p.health <= 0;
}

//Get Input and Move Player
void updateplayer(player &p, UpgradeState &upgrades, gamestate &g) {
    auto inputStart = chrono::steady_clock::now();
    int c = tolower(getch());
    auto inputEnd = chrono::steady_clock::now();

    int elapsedMs = static_cast<int>(
        chrono::duration_cast<chrono::milliseconds>(inputEnd - inputStart).count());
    int remainingMs = max(0, framems - elapsedMs);
    napms(remainingMs);

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

string playGame(int diff) {
    resetDirector();

    //init gamestate
    gamestate gmst;
    gmst.state = "run";
    gmst.level = 1;
    gmst.diff = diff;

    //init upgrades
    UpgradeState upgrades;
    uppick pickedUpgrades;
    //initplayer
    player player{0,0,3,3};

    //generate a layout
    genLevel(gmst, player);

    while (gmst.state == "run") {
        //show screen
        display(gmst, player, pickedUpgrades);

        //collect player input and update player position, pickups check
        updateplayer(player, upgrades, gmst);
        if (gmst.state != "run") break;
        collectPickups(upgrades, gmst.elist, player);

        //update entities (if time is stopped, all entities dont update)
        if (upgrades.timeStopTurns > 0) {
            upgrades.timeStopTurns--;
        } else {
            updateentities(gmst, player, upgrades);
        }

        //damage check
        int enemyIndex = getPlayerHitIndex(gmst.elist, player);
        if (enemyIndex != -1) {
            if (applyPlayerHit(upgrades, player, gmst.elist, enemyIndex)) {
                gmst.state = "dead";
            }
        }

        //win check
        if (open >= 3) {
            if (gmst.level >= 6) {
                gmst.state = "win";
                break;
            }

            //select an upgrade
            int selectedUpgrade = chooseUpgrade(gmst.level, player, pickedUpgrades);
            pickedUpgrades.push_back(selectedUpgrade);
            
            //update values of certain upgrades
            if (selectedUpgrade == 3) {
                upgrades.healOneEndStage = true;
            }
            else if (selectedUpgrade == 4) {
                upgrades.healHalfEndStage = true;
            }

            //carry on and proceed to next stage
            onStageClear(upgrades, player);
            gmst.level++;
            genLevel(gmst, player);
            addCarriedUpgradePickups(upgrades, gmst.elist, gmst.wlist);
            applyUpgrade(selectedUpgrade, upgrades, player, gmst.elist, gmst.wlist, n, m);
        }

        //progress upgrades with time-related abilities
        nextUpgradeTurn(upgrades);
        //progress enemy time spawner
        updater(turns, targ, gmst, player);
    }

    //free the dynamic array pickedUpgrades when the game ends
    pickedUpgrades.release();
    if (gmst.state == "dead") return "dead";
    if (gmst.state == "win") return "win";
    return "quit";
}

signed main() {
    initialize();
    colorscale();
    gamestate menuSettings;
    menuSettings.diff = 1;

    while (true) {
        string menuChoice = showMainMenu(menuSettings);

        if (menuChoice == "quit") {
            break;
        }

        string result = playGame(menuSettings.diff);
        string endChoice = showEndMenu(result);

        while (endChoice == "restart") {
            result = playGame(menuSettings.diff);
            endChoice = showEndMenu(result);
        }

        if (endChoice == "quit") {
            break;
        }
    }

    endwin();
}
