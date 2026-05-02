#ifndef ENEMIES_H
#define ENEMIES_H

#include "declar.h"
#include "upgrades.h"

bool isWall(vector<pair<int,int>> &w, int x, int y);

void bBouncer(entity &e, vector<pair<int,int>> &w);
void bFollow(entity &e, player p, vector<pair<int,int>> &w);
void bGhost(entity &e, player p);
void bLeaper(entity &e, player p, vector<pair<int,int>> &w);
bool bMushroom(entity mushroom, player p);

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

void updateentities(vector<entity> &e, player p, vector<pair<int,int>> &w, UpgradeState &upgrades);

bool isDamagingEnemy(entity e);
int getPlayerHitIndex(vector<entity> e, player p);

#endif
