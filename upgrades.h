#ifndef UPGRADES_H
#define UPGRADES_H

#include "declar.h"

class UpgradeState {
    public:
        bool revengeKill;
        bool healOneEndStage;
        bool healHalfEndStage;
        bool killEveryTwoPickups;
        bool healEveryTwoPickups;
        bool loopAroundMap;
        bool fifthTurnImmune;
        bool swapStationaryEnemy;
        bool blockOneProjectile;
        bool limitProjectiles;
        bool slowProjectiles;
        bool spawnTimeStopPickups;
        bool spawnKillPickups;
        bool spawnSwapPickups;

        int pickupCount;
        int timeStopPickupCount;
        int timeStopTurns;
        int turnCount;
        int loopCharges;
        int projectileBlocks;
        int projectileLimit;

        UpgradeState();
};

void healPlayer(int &health, int maxHealth, int amount);
int countProjectiles(vector<entity> &elist);
void killFirstEnemy(vector<entity> &elist);
void spawnPickup(vector<entity> &elist, vector<pair<int, int>> &wlist, int n, int m, string pickupType, char symbol);
void applyUpgrade(int upgradeNumber, UpgradeState &upgrades, int &health, int &maxHealth, vector<entity> &elist, vector<pair<int, int>> &wlist, int n, int m);
void onStageClear(UpgradeState &upgrades, int &health, int maxHealth);
void onPickupCollected(UpgradeState &upgrades, int &health, int maxHealth, vector<entity> &elist);
bool isPlayerImmune(UpgradeState &upgrades);
void onPlayerHit(UpgradeState &upgrades, int &health, vector<entity> &elist, int enemyIndex);
bool tryBlockProjectile(UpgradeState &upgrades);
bool canSpawnMoreProjectiles(UpgradeState &upgrades, vector<entity> &elist);
bool shouldProjectileMove(UpgradeState &upgrades);
void nextUpgradeTurn(UpgradeState &upgrades);
string getUpgradeName(int upgradeNumber);

#endif
