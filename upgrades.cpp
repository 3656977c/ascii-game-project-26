#include "upgrades.h"

// Upgrade state and small helpers used by the main game loop.

UpgradeState::UpgradeState() {
    revengeKill = false;
    healOneEndStage = false;
    healHalfEndStage = false;
    killEveryTwoPickups = false;
    healEveryTwoPickups = false;
    loopAroundMap = false;
    fifthTurnImmune = false;
    swapStationaryEnemy = false;
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

// Only projectiles that are still visible count.
int countProjectiles(vector<entity> &elist) {
    int total = 0;

    for (int i = 0; i < (int)elist.size(); i++) {
        if (elist[i].type == "projectile" && elist[i].c != -1) {
            total++;
        }
    }

    return total;
}

bool isEnemyType(string type) {
    return type == "bouncer" ||
           type == "follow" ||
           type == "ghost" ||
           type == "charger" ||
           type == "knight" ||
           type == "mushroom" ||
           type == "turret" ||
           type == "shooter";
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

// Put one pickup somewhere random on the map.
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

// Turn on the chosen upgrade, and spawn anything it gives immediately.
void applyUpgrade(int upgradeNumber, UpgradeState &upgrades, int &health, int &maxHealth, vector<entity> &elist, int n, int m) {
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
        spawnPickup(elist, n, m, "time_stop", 'T');
        spawnPickup(elist, n, m, "time_stop", 'T');
    }
    else if (upgradeNumber == 6) {
        // Add kill pickups to this and later stages.
        upgrades.spawnKillPickups = true;
        spawnPickup(elist, n, m, "kill_pickup", 'K');
        spawnPickup(elist, n, m, "kill_pickup", 'K');
    }
    else if (upgradeNumber == 7) {
        // Add swap pickups to this and later stages.
        upgrades.spawnSwapPickups = true;
        spawnPickup(elist, n, m, "swap_pickup", 'S');
        spawnPickup(elist, n, m, "swap_pickup", 'S');
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
        // Reserved for stationary-enemy swap behavior.
        upgrades.swapStationaryEnemy = true;
    }
    else if (upgradeNumber == 13) {
        // One projectile block per stage.
        upgrades.blockOneProjectile = true;
        upgrades.projectileBlocks = 1;
    }
    else if (upgradeNumber == 14) {
        // Keep the projectile count capped.
        upgrades.limitProjectiles = true;
        upgrades.projectileLimit = 5;
    }
    else if (upgradeNumber == 15) {
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

    health--;

    if (upgrades.revengeKill == true) {
        if (enemyIndex >= 0 && enemyIndex < (int)elist.size()) {
            elist[enemyIndex].c = -1;
        }
    }
}

// Older loop-around helper kept for code that uses entity as the player.
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

// Spend a projectile block if one is available.
bool tryBlockProjectile(UpgradeState &upgrades) {
    if (upgrades.blockOneProjectile == true && upgrades.projectileBlocks > 0) {
        upgrades.projectileBlocks--;
        return true;
    }

    return false;
}

// Projectile spawn cap.
bool canSpawnProjectile(UpgradeState &upgrades, vector<entity> &elist) {
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
    if (upgradeNumber == 12) return "Swap with stationary enemies";
    if (upgradeNumber == 13) return "Block one projectile per floor";
    if (upgradeNumber == 14) return "Limit projectiles to 5";
    if (upgradeNumber == 15) return "Slow projectiles";

    return "Unknown upgrade";
}
