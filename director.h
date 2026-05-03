#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "declar.h"

vector<entity> listgen(int pts, vector<pair<int, entity>> epool);
void director(gamestate &g, player &p);
bool occupied(int x, int y, player p, gamestate g);
bool forcespawn(player p, gamestate& g, entity e, int bgx, int bgy);
void updater(int &val, pair<int, entity> &target, gamestate &g, player p);
#endif
