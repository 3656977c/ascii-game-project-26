#ifndef ENEMIES_H
#define ENEMIES_H

#include "declar.h"
#include "upgrades.h"

bool isWall(vector<pair<int,int>> &w, int x, int y);

void bBouncer(entity &e, vector<pair<int,int>> &w);
void bGhost(entity &e, player p);
void bLeaper(entity &e, player p, vector<pair<int,int>> &w);
void bSpawner(entity &e);
void bMushroom(entity &mushroom, vector<entity> &elist, vector<pair<int,int>> &w);
void bHarmingTile(entity &e);

void bTurret(entity &e, vector<entity> &elist, vector<pair<int,int>> &w, UpgradeState &upgrades);
bool isShooterBlocked(vector<entity> &elist, vector<pair<int,int>> &w, int x, int y, int selfIndex);

void bShooter(
    entity &e,
    player p,
    vector<entity> &elist,
    vector<pair<int,int>> &w,
    int selfIndex,
    UpgradeState &upgrades
);
void bProjectile(entity &e, vector<pair<int,int>> &w);

void bGate(entity &e, player p);
void bCoin(entity &e, player p);

void bGrappler(entity &e, player &p, vector<pair<int,int>> &w);
void bPlusTurret(entity &e, vector<entity> &elist, vector<pair<int,int>> &w, UpgradeState &upgrades);
void bXTurret(entity &e, vector<entity> &elist, vector<pair<int,int>> &w, UpgradeState &upgrades);
void removeInactiveEntities(vector<entity> &elist);
void updateentities(gamestate &g, player &p, UpgradeState &upgrades);

bool isDamagingEnemy(entity e);
int getPlayerHitIndex(vector<entity> e, player p);

#endif
