#include "enemies.h"
#include <cstdlib>
#include <cmath>

extern int n;
extern int m;
extern int open;

bool isWall(vector<pair<int,int>> &w, int x, int y) {
  for (auto tile : w) {
    if (tile.first == x && tile.second == y) {
      return true;
    }
  }

  return false;
}

// BOUNCER
// Moves diagonally and bounces off both borders and walls.
void bBouncer(entity &e, vector<pair<int,int>> &w) {
  int nextX = e.x + e.ax;
  int nextY = e.y + e.ay;

  if (nextX < 0 || nextX >= n) {
    e.ax *= -1;
  }

  if (nextY < 0 || nextY >= m) {
    e.ay *= -1;
  }

  nextX = e.x + e.ax;
  nextY = e.y + e.ay;

  if (isWall(w, nextX, nextY)) {
    e.ax *= -1;
    e.ay *= -1;

    nextX = e.x + e.ax;
    nextY = e.y + e.ay;
  }

  if (
    nextX >= 0 && nextX < n &&
    nextY >= 0 && nextY < m &&
    !isWall(w, nextX, nextY)
  ) {
    e.x = nextX;
    e.y = nextY;
  }
}

// FOLLOWER
// Moves one tile toward the player every other turn.
// Cannot pass through walls.
void bFollow(entity &e, player p, vector<pair<int,int>> &w) {
  if (e.t == 0) {
    e.t++;
    return;
  }

  int nextX = e.x;
  int nextY = e.y;

  if (abs(e.x - p.x) > abs(e.y - p.y)) {
    if (p.x > e.x) nextX++;
    else if (p.x < e.x) nextX--;
  } else {
    if (p.y > e.y) nextY++;
    else if (p.y < e.y) nextY--;
  }

  if (
    nextX >= 0 && nextX < n &&
    nextY >= 0 && nextY < m &&
    !isWall(w, nextX, nextY)
  ) {
    e.x = nextX;
    e.y = nextY;
  }

  e.t = 0;
}

// GHOST
// Follows the player but ignores walls.
void bGhost(entity &e, player p) {
  if (e.t == 0) {
    e.t++;
    return;
  }

  if (abs(e.x - p.x) > abs(e.y - p.y)) {
    if (p.x > e.x && e.x < n - 1) e.x++;
    else if (p.x < e.x && e.x > 0) e.x--;
  } else {
    if (p.y > e.y && e.y < m - 1) e.y++;
    else if (p.y < e.y && e.y > 0) e.y--;
  }

  e.t = 0;
}

// CHARGER
// Moves 2 tiles toward the player.
// Cannot land inside a wall.
void bCharger(entity &e, player p, vector<pair<int,int>> &w) {
  if (e.t == 0) {
    e.t++;
    return;
  }

  int dx = 0;
  int dy = 0;

  if (abs(e.x - p.x) > abs(e.y - p.y)) {
    if (p.x > e.x) dx = 1;
    else if (p.x < e.x) dx = -1;
  } else {
    if (p.y > e.y) dy = 1;
    else if (p.y < e.y) dy = -1;
  }

  for (int step = 0; step < 2; step++) {
    int nextX = e.x + dx;
    int nextY = e.y + dy;

    if (
      nextX >= 0 && nextX < n &&
      nextY >= 0 && nextY < m &&
      !isWall(w, nextX, nextY)
    ) {
      e.x = nextX;
      e.y = nextY;
    }
  }

  e.t = 0;
}

// KNIGHT
// Jumps exactly 2 tiles in one cardinal direction.
// It skips the middle tile.
// It cannot land on a wall.
void bKnight(entity &e, vector<pair<int,int>> &w) {
  if (e.t == 0) {
    e.t++;
    return;
  }

  int directions[4][2] = {
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1}
  };

  for (int tries = 0; tries < 8; tries++) {
    int choice = std::rand() % 4;

    int dx = directions[choice][0];
    int dy = directions[choice][1];

    int landX = e.x + 2 * dx;
    int landY = e.y + 2 * dy;

    if (
      landX >= 0 && landX < n &&
      landY >= 0 && landY < m &&
      !isWall(w, landX, landY)
    ) {
      e.x = landX;
      e.y = landY;
      break;
    }
  }

  e.t = 0;
}

// MUSHROOM
// Stationary enemy.
// Damages player inside a radius 2 circle.
bool bMushroom(entity mushroom, player p) {
  if (mushroom.c == -1 || mushroom.type != "mushroom") {
    return false;
  }

  int dx = mushroom.x - p.x;
  int dy = mushroom.y - p.y;

  return dx * dx + dy * dy <= 4;
}

// TURRET
// Shoots projectiles in 8 directions every 3 turns.
void bTurret(entity &e, vector<entity> &elist) {
  e.t++;

  if (e.t < 3) {
    return;
  }

  e.t = 0;

  int dirs[8][2] = {
    {-1, 0}, {1, 0},
    {0, -1}, {0, 1},
    {-1, -1}, {-1, 1},
    {1, -1}, {1, 1}
  };

  for (int i = 0; i < 8; i++) {
    entity projectile{
      e.x,
      e.y,
      "projectile",
      '*',
      2,
      dirs[i][0],
      dirs[i][1],
      0
    };

      elist.push_back(projectile);
  }
}

// PROJECTILE
// Moves in its direction.
// Dies when it hits a wall or leaves the map.
void bProjectile(entity &e, vector<pair<int,int>> &w) {
  int nextX = e.x + e.ax;
  int nextY = e.y + e.ay;

  if (
    nextX < 0 || nextX >= n ||
    nextY < 0 || nextY >= m ||
    isWall(w, nextX, nextY)
  ) {
    e.c = -1;
    return;
  }

  e.x = nextX;
  e.y = nextY;
}

// GATE
void bGate(entity &e, player p) {
  if (e.x == p.x && e.y == p.y && e.t == -1) {
    open++;
    e.t = 0;
    e.c = 1;
  }
}

// COIN
void bCoin(entity &e, player p) {
  if (e.x == p.x && e.y == p.y && e.c > -1) {
    e.t = 0;
    e.c = -1;
  }
}

// Updates all active entities.
void updateentities(vector<entity> &e, player p, vector<pair<int,int>> &w) {
  int originalSize = e.size();

  for (int i = 0; i < originalSize; i++) {
    if (e[i].c == -1) continue;

    if (e[i].type == "bouncer") {
      bBouncer(e[i], w);
    }
    else if (e[i].type == "follow") {
      bFollow(e[i], p, w);
    }
    else if (e[i].type == "ghost") {
      bGhost(e[i], p);
    }
    else if (e[i].type == "charger") {
      bCharger(e[i], p, w);
    }
    else if (e[i].type == "knight") {
      bKnight(e[i], w);
    }
    else if (e[i].type == "turret") {
      bTurret(e[i], e);
    }
    else if (e[i].type == "projectile") {
      bProjectile(e[i], w);
    }
    else if (e[i].type == "gate") {
      bGate(e[i], p);
    }
    else if (e[i].type == "coin") {
      bCoin(e[i], p);
    }
  }
}

bool isDamagingEnemy(entity e) {
  return e.c != -1 && (
    e.type == "bouncer" ||
    e.type == "follow" ||
    e.type == "ghost" ||
    e.type == "charger" ||
    e.type == "knight" ||
    e.type == "projectile"
  );
}

int getPlayerHitIndex(vector<entity> e, player p) {
  for (int i = 0; i < (int)e.size(); i++) {
    if (e[i].c == -1) {
      continue;
    }

    if (isDamagingEnemy(e[i]) && e[i].x == p.x && e[i].y == p.y) {
      return i;
    }

    if (bMushroom(e[i], p)) {
      return i;
    }
  }

  return -1;
}
