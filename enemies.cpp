#include "declar.h"
#include "enemies.h"
#include <cmath>

extern int n;
extern int m;
extern int open;

extern unsigned seed;
extern mt19937 engine;
extern uniform_int_distribution<int> dist;
#define rando dist(engine) 

bool isWall(vector<pair<int,int>> &w, int x, int y) {
  for (auto tile : w) {
    if (tile.first == x && tile.second == y) {
      return true;
    }
  }

  return false;
}

bool isBlocked(vector<pair<int,int>> &w, int x, int y) {
    if (x < 0 || x >= n || y < 0 || y >= m) {
        return true;
    }

    return isWall(w, x, y);
}

// BOUNCER
// Moves diagonally.
// Bounces off borders, walls, and wall corners.
// It checks both adjacent side tiles before allowing diagonal movement.
void bBouncer(entity &e, vector<pair<int,int>> &w) {
    int dx = e.ax;
    int dy = e.ay;

    // Safety: bouncer should always have a direction.
    if (dx == 0) dx = 1;
    if (dy == 0) dy = 1;

    // Check the side tiles separately.
    bool blockedVerticalSide = isBlocked(w, e.x + dx, e.y);
    bool blockedHorizontalSide = isBlocked(w, e.x, e.y + dy);

    // If the vertical side is blocked, reverse vertical direction.
    if (blockedVerticalSide) {
        dx *= -1;
    }

    // If the horizontal side is blocked, reverse horizontal direction.
    if (blockedHorizontalSide) {
        dy *= -1;
    }

    int nextX = e.x + dx;
    int nextY = e.y + dy;

    // If the diagonal landing tile itself is blocked,
    // reverse both directions.
    if (isBlocked(w, nextX, nextY)) {
        dx *= -1;
        dy *= -1;

        nextX = e.x + dx;
        nextY = e.y + dy;
    }

    // Final safety check.
    // Only move if the final tile and both side paths are valid.
    if (
        !isBlocked(w, nextX, nextY) &&
        !isBlocked(w, e.x + dx, e.y) &&
        !isBlocked(w, e.x, e.y + dy)
    ) {
        e.x = nextX;
        e.y = nextY;
    }

    e.ax = dx;
    e.ay = dy;
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
    if (e.t == 1) {
        if (abs(e.x - p.x)!= 0 && abs(e.y - p.y)!=0) {
            if (rando%2 == 0) {
                if (p.x > e.x) e.x++;
                else e.x--;
            } else {
                if (p.y > e.y) e.y++;
                else e.y--;
            }
        } else {
            if (abs(e.x - p.x) > 0) {
                if (p.x > e.x) e.x++;
                else e.x--;
            } else if (abs(e.y - p.y) > 0) {
                if (p.y > e.y) e.y++;
                else e.y--;
            }
        }
        e.t = 0;
    } else e.t++;
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
// It spawns all valid adjacent projectiles at the same time.
void bTurret(entity &e, vector<entity> &elist, vector<pair<int,int>> &w) {
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
        int dx = dirs[i][0];
        int dy = dirs[i][1];

        int spawnX = e.x + dx;
        int spawnY = e.y + dy;

        // Only block the projectile if the actual spawn tile is blocked.
        // Do NOT check side tiles here, because the projectile is being created,
        // not moving diagonally from a previous tile yet.
        if (isBlocked(w, spawnX, spawnY)) {
            continue;
        }

        entity projectile{
            spawnX,
            spawnY,
            "projectile",
            '*',
            2,
            dx,
            dy,
            0
        };

        elist.push_back(projectile);
    }
}
int signNum(int value) {
    if (value > 0) return 1;
    if (value < 0) return -1;
    return 0;
}

int distanceSquared(entity e, player p) {
    int dx = e.x - p.x;
    int dy = e.y - p.y;
    return dx * dx + dy * dy;
}

bool canSpawnProjectile(vector<pair<int,int>> &w, int x, int y, int ax, int ay) {
    if (isBlocked(w, x, y)) {
        return false;
    }

    // Diagonal projectile cannot spawn by cutting through a wall corner.
    if (ax != 0 && ay != 0) {
        if (isBlocked(w, x - ax, y) || isBlocked(w, x, y - ay)) {
            return false;
        }
    }

    return true;
}
// PROJECTILE SHOOTER
// Keeps roughly 5-6 tiles away from the player.
// Every few turns, shoots a three-wide volley toward the player.
void bShooter(entity &e, player p, vector<entity> &elist, vector<pair<int,int>> &w) {
    int dxToPlayer = p.x - e.x;
    int dyToPlayer = p.y - e.y;

    int absDx = abs(dxToPlayer);
    int absDy = abs(dyToPlayer);

    int distSq = distanceSquared(e, p);

    // Shoot every 4 turns.
    // e.t works as the cooldown/charge timer.
    if (e.t >= 4) {
        e.t = 0;

        int ax = 0;
        int ay = 0;

        // Choose the main direction toward the player.
        // If the player is mostly vertical, shoot vertically.
        // If mostly horizontal, shoot horizontally.
        // If diagonal-ish, shoot diagonally.
        if (absDx > absDy + 1) {
            ax = signNum(dxToPlayer);
            ay = 0;
        }
        else if (absDy > absDx + 1) {
            ax = 0;
            ay = signNum(dyToPlayer);
        }
        else {
            ax = signNum(dxToPlayer);
            ay = signNum(dyToPlayer);
        }

        if (ax == 0 && ay == 0) {
            return;
        }

        // Three-wide volley offsets.
        // If shooting vertically, spread left/right.
        // If shooting horizontally, spread up/down.
        // If shooting diagonally, use a small perpendicular spread.
        int offsets[3][2];

        if (ax != 0 && ay == 0) {
            offsets[0][0] = 0;  offsets[0][1] = -1;
            offsets[1][0] = 0;  offsets[1][1] = 0;
            offsets[2][0] = 0;  offsets[2][1] = 1;
        }
        else if (ax == 0 && ay != 0) {
            offsets[0][0] = -1; offsets[0][1] = 0;
            offsets[1][0] = 0;  offsets[1][1] = 0;
            offsets[2][0] = 1;  offsets[2][1] = 0;
        }
        else {
            // Perpendicular to diagonal direction.
            offsets[0][0] = -ax; offsets[0][1] = ay;
            offsets[1][0] = 0;   offsets[1][1] = 0;
            offsets[2][0] = ax;  offsets[2][1] = -ay;
        }

        for (int i = 0; i < 3; i++) {
            int spawnX = e.x + ax + offsets[i][0];
            int spawnY = e.y + ay + offsets[i][1];

            if (!canSpawnProjectile(w, spawnX, spawnY, ax, ay)) {
                continue;
            }

            entity projectile{
                spawnX,
                spawnY,
                "projectile",
                '*',
                2,
                ax,
                ay,
                0
            };

            elist.push_back(projectile);
        }

        return;
    }

    // Movement phase:
    // If too close, move away.
    // If too far, move closer.
    // If at good distance, stay still and charge.
    int moveX = 0;
    int moveY = 0;

    if (distSq < 25) {
        // Too close: move away from player.
        if (absDx > absDy) {
            moveX = -signNum(dxToPlayer);
        } else {
            moveY = -signNum(dyToPlayer);
        }
    }
    else if (distSq > 36) {
        // Too far: move toward player.
        if (absDx > absDy) {
            moveX = signNum(dxToPlayer);
        } else {
            moveY = signNum(dyToPlayer);
        }
    }
    else {
        // Good range: pause/charge.
        e.t++;
        return;
    }

    int nextX = e.x + moveX;
    int nextY = e.y + moveY;

    if (!isBlocked(w, nextX, nextY)) {
        e.x = nextX;
        e.y = nextY;
    }

    // The shooter charges slowly even while repositioning.
    e.t++;
}
// PROJECTILE
// Moves in its direction.
// Dies when it hits a wall or leaves the map.
void bProjectile(entity &e, vector<pair<int,int>> &w) {
    int nextX = e.x + e.ax;
    int nextY = e.y + e.ay;

    // Cardinal projectile movement:
    // only needs to check the landing tile.
    if (e.ax == 0 || e.ay == 0) {
        if (isBlocked(w, nextX, nextY)) {
            e.c = -1;
            return;
        }

        e.x = nextX;
        e.y = nextY;
        return;
    }

    // Diagonal projectile movement:
    // must check both side tiles and the landing tile.
    bool verticalSideBlocked = isBlocked(w, e.x + e.ax, e.y);
    bool horizontalSideBlocked = isBlocked(w, e.x, e.y + e.ay);
    bool landingBlocked = isBlocked(w, nextX, nextY);

    if (verticalSideBlocked || horizontalSideBlocked || landingBlocked) {
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
      bTurret(e[i], e, w);
    }
    else if (e[i].type== "shooter") {
      bShooter(e[i], p, e, w);
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
    e.type == "shooter" ||
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
