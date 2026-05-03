#include "upgrades.h"

extern int n;
extern int m;
extern int coin;

// Upgrade state and small helpers used by the main game loop.

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

// Heal without passing the current max health.
void healPlayer(int &health, int maxHealth, int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

bool isProjectileEntity(entity item) {
    return item.type == "projectile" && item.c != -1;
}

// Only enemy projectile entities that are still active count.
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

// Remove the first enemy still active on the board.
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

// Put one pickup on a free random tile.
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

// Turn on the chosen upgrade, and spawn anything it gives immediately.
void applyUpgrade(int upgradeNumber, UpgradeState &upgrades, int &health, int &maxHealth, vector<entity> &elist, vector<pair<int, int>> &wlist, int n, int m) {
    if (upgradeNumber == 1) {
        // More max hp, with a full heal.
        maxHealth += 1;
        health = maxHealth;
    }
    else if (upgradeNumber == 2) {
        // The enemy that hits you dies too.
        upgrades.revengeKill = true;
    }
    else if (upgradeNumber == 3) {
        // Small heal after each stage.
        upgrades.healOneEndStage = true;
    }
    else if (upgradeNumber == 4) {
        // Never start a stage below half health.
        upgrades.healHalfEndStage = true;
    }
    else if (upgradeNumber == 5) {
        // Add time-stop pickups to this and later stages.
        upgrades.spawnTimeStopPickups = true;
        spawnPickup(elist, wlist, n, m, "time_stop", 'T');
        spawnPickup(elist, wlist, n, m, "time_stop", 'T');
    }
    else if (upgradeNumber == 6) {
        // Add kill pickups to this and later stages.
        upgrades.spawnKillPickups = true;
        spawnPickup(elist, wlist, n, m, "kill_pickup", 'K');
        spawnPickup(elist, wlist, n, m, "kill_pickup", 'K');
    }
    else if (upgradeNumber == 7) {
        // Add swap pickups to this and later stages.
        upgrades.spawnSwapPickups = true;
        spawnPickup(elist, wlist, n, m, "swap_pickup", 'S');
        spawnPickup(elist, wlist, n, m, "swap_pickup", 'S');
    }
    else if (upgradeNumber == 8) {
        // Every two pickups also removes an enemy.
        upgrades.killEveryTwoPickups = true;
    }
    else if (upgradeNumber == 9) {
        // Every two pickups also heals 1 hp.
        upgrades.healEveryTwoPickups = true;
    }
    else if (upgradeNumber == 10) {
        // One wrap from one edge to the opposite edge.
        upgrades.loopAroundMap = true;
        upgrades.loopCharges = 1;
    }
    else if (upgradeNumber == 11) {
        // Ignore damage every fifth turn.
        upgrades.fifthTurnImmune = true;
    }
    else if (upgradeNumber == 12) {
        // One projectile block per stage.
        upgrades.blockOneProjectile = true;
        upgrades.projectileBlocks = 1;
    }
    else if (upgradeNumber == 13) {
        // Keep the projectile count capped.
        upgrades.limitProjectiles = true;
        upgrades.projectileLimit = 12;
    }
    else if (upgradeNumber == 14) {
        // Make projectile movement skip every other turn.
        upgrades.slowProjectiles = true;
    }
}

// Stage-end effects.
void onStageClear(UpgradeState &upgrades, int &health, int maxHealth) {
    if (upgrades.healOneEndStage == true) {
        healPlayer(health, maxHealth, 1);
    }

    if (upgrades.healHalfEndStage == true) {
        int halfHealth = (maxHealth + 1) / 2;
        if (health < halfHealth) {
            health = halfHealth;
        }
    }

    if (upgrades.blockOneProjectile == true) {
        upgrades.projectileBlocks = 1;
    }
}

// Pickup-count effects.
void onPickupCollected(UpgradeState &upgrades, int &health, int maxHealth, vector<entity> &elist) {
    upgrades.pickupCount++;

    if (upgrades.pickupCount % 2 == 0) {
        if (upgrades.killEveryTwoPickups == true) {
            killFirstEnemy(elist);
        }

        if (upgrades.healEveryTwoPickups == true) {
            healPlayer(health, maxHealth, 1);
        }
    }
}

bool isPickup(entity e) {
    return e.c != -1 && (e.type == "coin" || e.type == "time_stop" || e.type == "kill_pickup" || e.type == "swap_pickup");
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
                if (elist[i].type == "coin") {
                    coin++;
                }
                elist[i].t = 0;
                elist[i].c = -1;
            }

            if (collected) {
                onPickupCollected(upgrades, health, maxHealth, elist);
            }
        }
    }
}

// Fifth-turn immunity check.
bool isPlayerImmune(UpgradeState &upgrades) {
    if (upgrades.fifthTurnImmune == true && upgrades.turnCount % 5 == 0) {
        return true;
    }

    return false;
}

// Apply damage and any revenge effect.
void onPlayerHit(UpgradeState &upgrades, int &health, vector<entity> &elist, int enemyIndex) {
    if (isPlayerImmune(upgrades) == true) {
        return;
    }

    if (enemyIndex >= 0 && enemyIndex < (int)elist.size()) {
        if (isProjectileEntity(elist[enemyIndex]) && tryBlockProjectile(upgrades)) {
            elist[enemyIndex].c = -1;
            return;
        }
    }

    health--;

    if (upgrades.revengeKill == true) {
        if (enemyIndex >= 0 && enemyIndex < (int)elist.size()) {
            elist[enemyIndex].c = -1;
        }
    }
}

// Spend a projectile block if one is available.
bool tryBlockProjectile(UpgradeState &upgrades) {
    if (upgrades.blockOneProjectile == true && upgrades.projectileBlocks > 0) {
        upgrades.projectileBlocks--;
        return true;
    }

    return false;
}

// Projectile spawn cap.
bool canSpawnMoreProjectiles(UpgradeState &upgrades, vector<entity> &elist) {
    if (upgrades.limitProjectiles == false) {
        return true;
    }

    if (countProjectiles(elist) < upgrades.projectileLimit) {
        return true;
    }

    return false;
}

// Slow projectiles move every other turn.
bool shouldProjectileMove(UpgradeState &upgrades) {
    if (upgrades.slowProjectiles == false) {
        return true;
    }

    if (upgrades.turnCount % 2 == 0) {
        return true;
    }

    return false;
}

// Advance upgrade timers.
void nextUpgradeTurn(UpgradeState &upgrades) {
    upgrades.turnCount++;
}

// Text shown in the upgrade picker.
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

int chooseUpgrade(int level, int health, int maxHealth, const uppick& pickedUpgrades) {
    vector<int> choices = getUpgradeChoices(pickedUpgrades);

    while (true) {
        erase();
        mvprintw(0, 0, "Level %d cleared!", level);
        mvprintw(1, 0, "Health: %d/%d", health, maxHealth);
        mvprintw(3, 0, "Choose an upgrade:");

        for (int i = 0; i < coin + 2; i++) {
            string name = getUpgradeName(choices[i]);
            mvprintw(5 + i, 0, "%d. %s", i + 1, name.c_str());
        }

        mvprintw(9, 0, "Press 1, 2, or 3");
        refresh();

        int c = getch();
        if (c >= '1' && c <= 48 + 2 + coin) {
            return choices[c - '1'];
        }
    }
}
