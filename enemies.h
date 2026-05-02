#ifndef ENEMIES_H
#define ENEMIES_H

#include "declar.h"

void bBouncer(entity &e);
void bFollow(entity &e, player p);
void bGhost(entity &e, player p);
void bCharger(entity &e, player p);
void bKnight(entity &e);
void bMushroom(entity &e);
void bTurret(entity &e, vector<entity> &elist);
void bProjectile(entity &e);

void bGate(entity &e, player p);
void bCoin(entity &e, player p);

void updateentities(vector<entity> &e,player p);
bool isDamagingEnemy(entity e);
bool isAreaDamageEnemy(entity e, player p);
int getPlayerHitIndex(vector<entity> e, player p);




#endif 
