#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "declar.h"

void director(vector<entity> &elist, vector<pair<int,int>> &wall, player &p, int danval);
bool occupied(int x, int y, player p, vector<entity> elist, vector<pair<int,int>> wall);
#endif
