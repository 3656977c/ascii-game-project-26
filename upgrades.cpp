#include "declar.h"

// upgrades.cpp
// This file contains the upgrade system for the Dodger game.
// It is written in a simple way so it can be connected to main.cpp/director.cpp later.
//
// How to use this file:
// 1. Create one UpgradeState variable for the player.
//        UpgradeState upgrades;
// 2. When the player chooses an upgrade, call:
//        applyUpgrade(choiceNumber, upgrades, playerHealth, maxHealth, elist, n, m);
// 3. When the player picks up a coin or pickup, call:
//        onPickupCollected(upgrades, playerHealth, maxHealth, elist);
// 4. When a stage ends, call:
//        onStageClear(upgrades, playerHealth, maxHealth);
// 5. Every turn, increase upgrades.turnCount by 1.

class UpgradeState {
    public:
        bool revengeKill;
        bool healOneEndStage;
        bool healHalfEndStage;
        bool killEveryThreePickups;
        bool healEveryThreePickups;
        bool loopAroundMap;
        bool fifthTurnImmune;
        bool swapStationaryEnemy;
        bool blockOneProjectile;
        bool limitProjectiles;
        bool slowProjectiles;

        int pickupCount;
        int turnCount;
        int loopCharges;
        int projectileBlocks;
        int projectileLimit;

        UpgradeState() {
            revengeKill = false;
            healOneEndStage = false;
            healHalfEndStage = false;
            killEveryThreePickups = false;
            healEveryThreePickups = false;
            loopAroundMap = false;
            fifthTurnImmune = false;
            swapStationaryEnemy = false;
            blockOneProjectile = false;
            limitProjectiles = false;
            slowProjectiles = false;

            pickupCount = 0;
            turnCount = 0;
            loopCharges = 0;
            projectileBlocks = 0;
            projectileLimit = 999;
        }
};

// Makes sure health does not go above max health.
void healPlayer(int &health, int maxHealth, int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

// Counts active projectiles. Projectiles should have type "projectile".
int countProjectiles(vector<entity> &elist) {
    int total = 0;

    for (int i = 0; i < (int)elist.size(); i++) {
        if (elist[i].type == "projectile" && elist[i].c != -1) {
            total++;
        }
    }

    return total;
}

// Kills the first active enemy found in the entity list.
// This is used for the "kill enemy every three pickups" upgrade.
void killFirstEnemy(vector<entity> &elist) {
    for (int i = 0; i < (int)elist.size(); i++) {
        if (elist[i].type != "player" && elist[i].type != "gate" && elist[i].type != "coin" && elist[i].type != "projectile") {
            if (elist[i].c != -1) {
                elist[i].c = -1;
                return;
            }
        }
    }
}

// Spawns a pickup on the map.
// pickupType can be "time_stop", "kill_pickup", or "swap_pickup".
void spawnPickup(vector<entity> &elist, int n, int m, string pickupType, char symbol) {
    int x = rand() % n;
    int y = rand() % m;

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

// Applies the upgrade chosen by the player.
// Upgrade numbers follow the proposal list.
void applyUpgrade(int upgradeNumber, UpgradeState &upgrades, int &health, int &maxHealth, vector<entity> &elist, int n, int m) {
    if (upgradeNumber == 1) {
        // Increase max health and heal to full.
        maxHealth += 1;
        health = maxHealth;
    }
    else if (upgradeNumber == 2) {
        // If you get hit, kill the enemy that hit you.
        upgrades.revengeKill = true;
    }
    else if (upgradeNumber == 3) {
        // Heal 1 health at the end of every stage.
        upgrades.healOneEndStage = true;
    }
    else if (upgradeNumber == 4) {
        // Heal back to half health at the end of every stage.
        upgrades.healHalfEndStage = true;
    }
    else if (upgradeNumber == 5) {
        // Spawn 2 time stop pickups.
        spawnPickup(elist, n, m, "time_stop", 'T');
        spawnPickup(elist, n, m, "time_stop", 'T');
    }
    else if (upgradeNumber == 6) {
        // Spawn 2 kill enemy pickups.
        spawnPickup(elist, n, m, "kill_pickup", 'K');
        spawnPickup(elist, n, m, "kill_pickup", 'K');
    }
    else if (upgradeNumber == 7) {
        // Spawn 2 swap pickups.
        spawnPickup(elist, n, m, "swap_pickup", 'S');
        spawnPickup(elist, n, m, "swap_pickup", 'S');
    }
    else if (upgradeNumber == 8) {
        // Kill an enemy every time you pick up three items.
        upgrades.killEveryThreePickups = true;
    }
    else if (upgradeNumber == 9) {
        // Gain 1 hp every three pickups.
        upgrades.healEveryThreePickups = true;
    }
    else if (upgradeNumber == 10) {
        // Loop around the map once.
        upgrades.loopAroundMap = true;
        upgrades.loopCharges = 1;
    }
    else if (upgradeNumber == 11) {
        // Every fifth turn, be immune to damage.
        upgrades.fifthTurnImmune = true;
    }
    else if (upgradeNumber == 12) {
        // When walking into a stationary enemy, swap places with them instead.
        upgrades.swapStationaryEnemy = true;
    }
    else if (upgradeNumber == 13) {
        // Block one projectile per floor.
        upgrades.blockOneProjectile = true;
        upgrades.projectileBlocks = 1;
    }
    else if (upgradeNumber == 14) {
        // Limit 5 projectiles.
        upgrades.limitProjectiles = true;
        upgrades.projectileLimit = 5;
    }
    else if (upgradeNumber == 15) {
        // Slow projectiles.
        upgrades.slowProjectiles = true;
    }
}

// Call this at the end of every stage.
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

// Call this whenever the player picks up a coin or item.
void onPickupCollected(UpgradeState &upgrades, int &health, int maxHealth, vector<entity> &elist) {
    upgrades.pickupCount++;

    if (upgrades.pickupCount % 3 == 0) {
        if (upgrades.killEveryThreePickups == true) {
            killFirstEnemy(elist);
        }

        if (upgrades.healEveryThreePickups == true) {
            healPlayer(health, maxHealth, 1);
        }
    }
}

// Returns true if the player should ignore damage this turn.
bool isPlayerImmune(UpgradeState &upgrades) {
    if (upgrades.fifthTurnImmune == true && upgrades.turnCount % 5 == 0) {
        return true;
    }

    return false;
}

// Call this when an enemy damages the player.
// enemyIndex is the index of the enemy in elist that damaged the player.
void onPlayerHit(UpgradeState &upgrades, int &health, vector<entity> &elist, int enemyIndex) {
    if (isPlayerImmune(upgrades) == true) {
        return;
    }

    health--;

    if (upgrades.revengeKill == true) {
        if (enemyIndex >= 0 && enemyIndex < (int)elist.size()) {
            elist[enemyIndex].c = -1;
        }
    }
}

// Allows the player to loop around the map one time.
// Call this after moving the player.
void checkLoopAroundMap(UpgradeState &upgrades, entity &player, int n, int m) {
    if (upgrades.loopAroundMap == false || upgrades.loopCharges <= 0) {
        return;
    }

    bool used = false;

    if (player.x < 0) {
        player.x = n - 1;
        used = true;
    }
    else if (player.x >= n) {
        player.x = 0;
        used = true;
    }

    if (player.y < 0) {
        player.y = m - 1;
        used = true;
    }
    else if (player.y >= m) {
        player.y = 0;
        used = true;
    }

    if (used == true) {
        upgrades.loopCharges--;
    }
}

// Checks if a projectile should be blocked.
// Returns true if the projectile was blocked.
bool tryBlockProjectile(UpgradeState &upgrades) {
    if (upgrades.blockOneProjectile == true && upgrades.projectileBlocks > 0) {
        upgrades.projectileBlocks--;
        return true;
    }

    return false;
}

// Checks if a new projectile is allowed to spawn.
bool canSpawnProjectile(UpgradeState &upgrades, vector<entity> &elist) {
    if (upgrades.limitProjectiles == false) {
        return true;
    }

    if (countProjectiles(elist) < upgrades.projectileLimit) {
        return true;
    }

    return false;
}

// Moves projectiles slower if the slow projectile upgrade is active.
// A projectile with slow mode moves only every other turn.
bool shouldProjectileMove(UpgradeState &upgrades) {
    if (upgrades.slowProjectiles == false) {
        return true;
    }

    if (upgrades.turnCount % 2 == 0) {
        return true;
    }

    return false;
}

// Use this at the end of every player turn.
void nextUpgradeTurn(UpgradeState &upgrades) {
    upgrades.turnCount++;
}

// Simple text used when showing upgrade choices.
string getUpgradeName(int upgradeNumber) {
    if (upgradeNumber == 1) return "Increase max health and heal to full";
    if (upgradeNumber == 2) return "Kill the enemy that damages you";
    if (upgradeNumber == 3) return "Heal 1 HP after every stage";
    if (upgradeNumber == 4) return "Heal back to half HP after every stage";
    if (upgradeNumber == 5) return "Spawn 2 time stop pickups";
    if (upgradeNumber == 6) return "Spawn 2 kill enemy pickups";
    if (upgradeNumber == 7) return "Spawn 2 swap pickups";
    if (upgradeNumber == 8) return "Kill an enemy every 3 pickups";
    if (upgradeNumber == 9) return "Gain 1 HP every 3 pickups";
    if (upgradeNumber == 10) return "Loop around the map once";
    if (upgradeNumber == 11) return "Immune every fifth turn";
    if (upgradeNumber == 12) return "Swap with stationary enemies";
    if (upgradeNumber == 13) return "Block one projectile per floor";
    if (upgradeNumber == 14) return "Limit projectiles to 5";
    if (upgradeNumber == 15) return "Slow projectiles";

    return "Unknown upgrade";
}
