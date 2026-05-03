#include "upgrades.h"

extern int n;
extern int m;
extern int coin;

//default upgrade values
UpgradeState::UpgradeState() {
    revengeKill = false;
    healOneEndStage = false;
    healHalfEndStage = false;
    killEveryTwoPickups = false;
    healEveryTwoPickups = false;
    loopAroundMap = false;
    fifthTurnImmune = false;
    blockOneProjectile = false;
    limitProjectiles = false;
    slowProjectiles = false;
    spawnTimeStopPickups = false;
    spawnKillPickups = false;
    spawnSwapPickups = false;

    pickupCount = 0;
    timeStopPickupCount = 0;
    timeStopTurns = 0;
    turnCount = 0;
    loopCharges = 0;
    projectileBlocks = 0;
    projectileLimit = 999;
}

//keeps hp from going over max hp
void healPlayer(player &p, int amount) {
    p.health += amount;
    if (p.health > p.maxHealth) {
        p.health = p.maxHealth;
    }
}

bool isProjectileEntity(entity item) {
    return item.type == "projectile" && item.c != -1;
}

//used by projectile limit upgrade
int countProjectiles(vector<entity> &elist) {
    int total = 0;

    for (int i = 0; i < (int)elist.size(); i++) {
        if (isProjectileEntity(elist[i])) {
            total++;
        }
    }

    return total;
}

bool isEnemyType(string type) {
    return type == "bouncer" ||
           type == "follow" ||
           type == "ghost" ||
           type == "leaper" ||
           type == "mushroom" ||
           type == "turret" ||
           type == "shooter" ||
           type == "grappler" ||
           type == "+tur" ||
           type == "xtur";
}

//used by kill pickups and some upgrades
void killFirstEnemy(vector<entity> &elist) {
    for (int i = 0; i < (int)elist.size(); i++) {
        if (isEnemyType(elist[i].type)) {
            if (elist[i].c != -1) {
                elist[i].c = -1;
                return;
            }
        }
    }
}

bool isPickupSpotFree(vector<entity> &elist, vector<pair<int, int>> &wlist, int x, int y) {
    for (auto wall: wlist) {
        if (wall.first == x && wall.second == y) {
            return false;
        }
    }

    for (auto item: elist) {
        if (item.c != -1 && item.x == x && item.y == y) {
            return false;
        }
    }

    return true;
}

//make the actual pickup entity
void addPickup(vector<entity> &elist, int x, int y, string pickupType, char symbol) {
    entity pickup;
    pickup.x = x;
    pickup.y = y;
    pickup.type = pickupType;
    pickup.s = symbol;
    pickup.c = 3;
    pickup.ax = -1;
    pickup.ay = -1;
    pickup.t = -1;

    elist.push_back(pickup);
}

//tries random spots first, then searches the map
void spawnPickup(vector<entity> &elist, vector<pair<int, int>> &wlist, int n, int m, string pickupType, char symbol) {
    int x = 0;
    int y = 0;

    for (int tries = 0; tries < 100; tries++) {
        x = rand() % n;
        y = rand() % m;

        if (isPickupSpotFree(elist, wlist, x, y)) {
            break;
        }
    }

    if (!isPickupSpotFree(elist, wlist, x, y)) {
        for (x = 0; x < n; x++) {
            for (y = 0; y < m; y++) {
                if (isPickupSpotFree(elist, wlist, x, y)) {
                    addPickup(elist, x, y, pickupType, symbol);
                    return;
                }
            }
        }

        return;
    }

    addPickup(elist, x, y, pickupType, symbol);
}

//adds pickups from upgrades you already own
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

//apply selected upgrade
void applyUpgrade(int upgradeNumber, UpgradeState &upgrades, player &p, vector<entity> &elist, vector<pair<int, int>> &wlist, int n, int m) {
    if (upgradeNumber == 1) {
        //more max hp and full heal
        p.maxHealth += 1;
        p.health = p.maxHealth;
    }
    else if (upgradeNumber == 2) {
        //enemy that hits you dies too
        upgrades.revengeKill = true;
    }
    else if (upgradeNumber == 3) {
        //heal after each stage
        upgrades.healOneEndStage = true;
    }
    else if (upgradeNumber == 4) {
        //start stage at least half hp
        upgrades.healHalfEndStage = true;
    }
    else if (upgradeNumber == 5) {
        //time stop pickups
        upgrades.spawnTimeStopPickups = true;
        spawnPickup(elist, wlist, n, m, "time_stop", 'T');
        spawnPickup(elist, wlist, n, m, "time_stop", 'T');
    }
    else if (upgradeNumber == 6) {
        //kill pickups
        upgrades.spawnKillPickups = true;
        spawnPickup(elist, wlist, n, m, "kill_pickup", 'K');
        spawnPickup(elist, wlist, n, m, "kill_pickup", 'K');
    }
    else if (upgradeNumber == 7) {
        //swap pickups
        upgrades.spawnSwapPickups = true;
        spawnPickup(elist, wlist, n, m, "swap_pickup", 'S');
        spawnPickup(elist, wlist, n, m, "swap_pickup", 'S');
    }
    else if (upgradeNumber == 8) {
        //every 2 pickups kills an enemy
        upgrades.killEveryTwoPickups = true;
    }
    else if (upgradeNumber == 9) {
        //every 2 pickups heals
        upgrades.healEveryTwoPickups = true;
    }
    else if (upgradeNumber == 10) {
        //one edge wrap
        upgrades.loopAroundMap = true;
        upgrades.loopCharges = 1;
    }
    else if (upgradeNumber == 11) {
        //every fifth turn immune
        upgrades.fifthTurnImmune = true;
    }
    else if (upgradeNumber == 12) {
        //one projectile block per stage
        upgrades.blockOneProjectile = true;
        upgrades.projectileBlocks = 1;
    }
    else if (upgradeNumber == 13) {
        //projectile cap
        upgrades.limitProjectiles = true;
        upgrades.projectileLimit = 12;
    }
    else if (upgradeNumber == 14) {
        //slow projectiles
        upgrades.slowProjectiles = true;
    }
}

//runs when a level is cleared
void onStageClear(UpgradeState &upgrades, player &p) {
    if (upgrades.healOneEndStage == true) {
        healPlayer(p, 1);
    }

    if (upgrades.healHalfEndStage == true) {
        int halfHealth = (p.maxHealth + 1) / 2;
        if (p.health < halfHealth) {
            p.health = halfHealth;
        }
    }

    if (upgrades.blockOneProjectile == true) {
        upgrades.projectileBlocks = 1;
    }
}

//some upgrades care about every 2 pickups
void onPickupCollected(UpgradeState &upgrades, player &p, vector<entity> &elist) {
    upgrades.pickupCount++;

    if (upgrades.pickupCount % 2 == 0) {
        if (upgrades.killEveryTwoPickups == true) {
            killFirstEnemy(elist);
        }

        if (upgrades.healEveryTwoPickups == true) {
            healPlayer(p, 1);
        }
    }
}

bool isPickup(entity e) {
    return e.c != -1 &&
           (e.type == "coin" ||
            e.type == "time_stop" ||
            e.type == "kill_pickup" ||
            e.type == "swap_pickup");
}

//teleport to the other swap pickup, then remove both
bool useSwapPickup(vector<entity> &elist, player &p, int swapIndex) {
    for (int i = 0; i < (int)elist.size(); i++) {
        if (i != swapIndex && elist[i].type == "swap_pickup" && elist[i].c != -1) {
            p.x = elist[i].x;
            p.y = elist[i].y;
            elist[i].t = 0;
            elist[i].c = -1;
            elist[swapIndex].t = 0;
            elist[swapIndex].c = -1;
            return true;
        }
    }

    return false;
}

//checks if the player is standing on a pickup
void collectPickups(UpgradeState &upgrades, vector<entity> &elist, player &p) {
    for (int i = 0; i < (int)elist.size(); i++) {
        if (isPickup(elist[i]) && elist[i].x == p.x && elist[i].y == p.y) {
            bool collected = true;

            if (elist[i].type == "kill_pickup") {
                killFirstEnemy(elist);
            }

            if (elist[i].type == "time_stop") {
                upgrades.timeStopPickupCount++;
                if (upgrades.timeStopPickupCount >= 2) {
                    upgrades.timeStopTurns = 3;
                    upgrades.timeStopPickupCount = 0;
                }
            }

            if (elist[i].type == "swap_pickup") {
                collected = useSwapPickup(elist, p, i);
            } else {
                if (elist[i].type == "coin") {
                    coin++;
                }
                elist[i].t = 0;
                elist[i].c = -1;
            }

            if (collected) {
                onPickupCollected(upgrades, p, elist);
            }
        }
    }
}

//damage skips on every fifth turn if owned
bool isPlayerImmune(UpgradeState &upgrades) {
    if (upgrades.fifthTurnImmune == true && upgrades.turnCount % 5 == 0) {
        return true;
    }

    return false;
}

//handles damage and revenge kill
void onPlayerHit(UpgradeState &upgrades, player &p, vector<entity> &elist, int enemyIndex) {
    if (isPlayerImmune(upgrades) == true) {
        return;
    }

    if (enemyIndex >= 0 && enemyIndex < (int)elist.size()) {
        if (isProjectileEntity(elist[enemyIndex]) && tryBlockProjectile(upgrades)) {
            elist[enemyIndex].c = -1;
            return;
        }
    }

    p.health--;

    if (upgrades.revengeKill == true) {
        if (enemyIndex >= 0 && enemyIndex < (int)elist.size()) {
            elist[enemyIndex].c = -1;
        }
    }
}

bool tryBlockProjectile(UpgradeState &upgrades) {
    if (upgrades.blockOneProjectile == true && upgrades.projectileBlocks > 0) {
        upgrades.projectileBlocks--;
        return true;
    }

    return false;
}

//called before enemies spawn projectiles
bool canSpawnMoreProjectiles(UpgradeState &upgrades, vector<entity> &elist) {
    if (upgrades.limitProjectiles == false) {
        return true;
    }

    if (countProjectiles(elist) < upgrades.projectileLimit) {
        return true;
    }

    return false;
}

bool shouldProjectileMove(UpgradeState &upgrades) {
    if (upgrades.slowProjectiles == false) {
        return true;
    }

    if (upgrades.turnCount % 2 == 0) {
        return true;
    }

    return false;
}

//one game turn passed
void nextUpgradeTurn(UpgradeState &upgrades) {
    upgrades.turnCount++;
}

//names shown in the upgrade menu
string getUpgradeName(int upgradeNumber) {
    if (upgradeNumber == 1) return "Increase max health and heal to full";
    if (upgradeNumber == 2) return "Kill the enemy that damages you";
    if (upgradeNumber == 3) return "Heal 1 HP after every stage";
    if (upgradeNumber == 4) return "Heal back to half HP after every stage";
    if (upgradeNumber == 5) return "Spawn 2 time stop pickups";
    if (upgradeNumber == 6) return "Spawn 2 kill enemy pickups";
    if (upgradeNumber == 7) return "Spawn 2 swap pickups";
    if (upgradeNumber == 8) return "Kill an enemy every 2 pickups";
    if (upgradeNumber == 9) return "Gain 1 HP every 2 pickups";
    if (upgradeNumber == 10) return "Loop around the map once";
    if (upgradeNumber == 11) return "Immune every fifth turn";
    if (upgradeNumber == 12) return "Block one projectile per floor";
    if (upgradeNumber == 13) return "Limit projectiles to 12";
    if (upgradeNumber == 14) return "Slow projectiles";

    return "Unknown upgrade";
}

bool hasUpgrade(const uppick& pickedUpgrades, int upgradeNumber) {
    for (int i = 0; i < pickedUpgrades.uit; i++) {
        if (pickedUpgrades.u[i] == upgradeNumber) {
            return true;
        }
    }

    return false;
}

vector<int> getUpgradeChoices(const uppick& pickedUpgrades) {
    vector<int> choices;

    while ((int)choices.size() < 4) {
        int upgradeNumber = rand() % 14 + 1;
        bool alreadyChosen = false;

        for (int choice: choices) {
            if (choice == upgradeNumber) alreadyChosen = true;
        }

        if (hasUpgrade(pickedUpgrades, upgradeNumber)) alreadyChosen = true;

        if (!alreadyChosen) choices.push_back(upgradeNumber);
    }

    return choices;
}

int chooseUpgrade(int level, player &p, const uppick& pickedUpgrades) {
    vector<int> choices = getUpgradeChoices(pickedUpgrades);
    int choiceCount = coin + 2;

    if (choiceCount > (int)choices.size()) {
        choiceCount = choices.size();
    }

    while (true) {
        erase();
        mvprintw(0, 0, "Level %d cleared!", level);
        mvprintw(1, 0, "Health: %d/%d", p.health, p.maxHealth);
        mvprintw(3, 0, "Choose an upgrade:");

        for (int i = 0; i < choiceCount; i++) {
            string name = getUpgradeName(choices[i]);
            mvprintw(5 + i, 0, "%d. %s", i + 1, name.c_str());
        }

        mvprintw(9, 0, "Press 1-%d", choiceCount);
        refresh();

        int c = getch();
        if (c >= '1' && c < '1' + choiceCount) {
            return choices[c - '1'];
        }
    }
}
