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

int signNum(int value);

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

  int dx = 0;
  int dy = 0;

  if (abs(e.x - p.x) > abs(e.y - p.y)) {
    if (p.x > e.x) dx = 1;
    else if (p.x < e.x) dx = -1;
  } else {
    if (p.y > e.y) dy = 1;
    else if (p.y < e.y) dy = -1;
  }

  int nextX = e.x + dx;
  int nextY = e.y + dy;

  if (!isBlocked(w, nextX, nextY)) {
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

// // LEAPER
// Merges charger + knight.
// It jumps exactly 2 tiles toward the player.
// It skips the middle tile and only checks the landing tile.
void bLeaper(entity &e, player p, vector<pair<int,int>> &w) {
    if (e.t == 0) {
        e.t++;
        return;
    }

    int dx = 0;
    int dy = 0;

    int diffX = p.x - e.x;
    int diffY = p.y - e.y;

    if (abs(diffX) > abs(diffY)) {
        dx = signNum(diffX);
    } else {
        dy = signNum(diffY);
    }

    int landX = e.x + 2 * dx;
    int landY = e.y + 2 * dy;

    if (!isBlocked(w, landX, landY)) {
        e.x = landX;
        e.y = landY;
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
void bTurret(entity &e, vector<entity> &elist, vector<pair<int,int>> &w, UpgradeState &upgrades) {
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

        if (!canSpawnMoreProjectiles(upgrades, elist)) {
            return;
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
bool isShooterBlocked(vector<entity> &elist, vector<pair<int,int>> &w, int x, int y, int selfIndex) {
    if (isBlocked(w, x, y)) {
        return true;
    }

    for (int i = 0; i < (int)elist.size(); i++) {
        if (i == selfIndex) {
            continue;
        }

        if (elist[i].c == -1 || elist[i].type != "shooter") {
            continue;
        }

        int dx = abs(elist[i].x - x);
        int dy = abs(elist[i].y - y);

        // A shooter cannot move onto or next to another shooter.
        // This makes nearby shooters act like walls.
        if (dx <= 1 && dy <= 1) {
            return true;
        }
    }

    return false;
}
// PROJECTILE SHOOTER
// Keeps roughly 5-6 tiles away from the player.
// Every few turns, shoots a three-wide volley toward the player.
void bShooter(
    entity &e,
    player p,
    vector<entity> &elist,
    vector<pair<int,int>> &w,
    int selfIndex,
    UpgradeState &upgrades
) {
    // Cooldown after shooting.
    // During cooldown, the shooter does not move or charge.
    if (e.t < 0) {
        e.t++;
        return;
    }

    int dxToPlayer = p.x - e.x;
    int dyToPlayer = p.y - e.y;

    int absDx = abs(dxToPlayer);
    int absDy = abs(dyToPlayer);

    int distSq = distanceSquared(e, p);

    // Shoot when fully charged.
    if (e.t >= 4) {
        int ax = 0;
        int ay = 0;

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
            e.t = -3;
            return;
        }

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
            offsets[0][0] = -ax; offsets[0][1] = ay;
            offsets[1][0] = 0;   offsets[1][1] = 0;
            offsets[2][0] = ax;  offsets[2][1] = -ay;
        }
        
        // Wait 3 turns after shooting.
        e.t = -3;

        for (int i = 0; i < 3; i++) {
            int spawnX = e.x + ax + offsets[i][0];
            int spawnY = e.y + ay + offsets[i][1];

            if (!canSpawnProjectile(w, spawnX, spawnY, ax, ay)) {
                continue;
            }

            if (!canSpawnMoreProjectiles(upgrades, elist)) {
                return;
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

    int moveX = 0;
    int moveY = 0;

    // Too close: move away.
    if (distSq < 25) {
        if (absDx > absDy) {
            moveX = -signNum(dxToPlayer);
        } else {
            moveY = -signNum(dyToPlayer);
        }
    }
    // Too far: move closer.
    else if (distSq > 36) {
        if (absDx > absDy) {
            moveX = signNum(dxToPlayer);
        } else {
            moveY = signNum(dyToPlayer);
        }
    }
    // Good range: stay still and charge.
    else {
        e.t++;
        return;
    }

    int nextX = e.x + moveX;
    int nextY = e.y + moveY;

    if (!isShooterBlocked(elist, w, nextX, nextY, selfIndex)) {
        e.x = nextX;
        e.y = nextY;
    }

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
// GRAPPLER
// If the player is in the same row or column, pulls the player one tile toward it.
// It also moves slowly every 3 turns.
void bGrappler(entity &e, player &p, vector<pair<int,int>> &w) {
    // Pull player if aligned.
    if (p.x == e.x) {
        int pullY = p.y + signNum(e.y - p.y);

        if (!isBlocked(w, p.x, pullY)) {
            p.y = pullY;
        }
    }
    else if (p.y == e.y) {
        int pullX = p.x + signNum(e.x - p.x);

        if (!isBlocked(w, pullX, p.y)) {
            p.x = pullX;
        }
    }

    // Move only once every 3 turns.
    e.t++;

    if (e.t < 3) {
        return;
    }

    e.t = 0;

    int dirs[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    for (int tries = 0; tries < 6; tries++) {
        int choice = rando % 4;

        int nextX = e.x + dirs[choice][0];
        int nextY = e.y + dirs[choice][1];

        if (!isBlocked(w, nextX, nextY)) {
            e.x = nextX;
            e.y = nextY;
            break;
        }
    }
}
// +TUR
// Moving turret that shoots cardinal projectiles.
void bPlusTurret(entity &e, vector<entity> &elist, vector<pair<int,int>> &w, UpgradeState &upgrades) {
    e.t++;

    // Random movement every turn.
    int dirsMove[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    int choice = rando % 4;
    int nextX = e.x + dirsMove[choice][0];
    int nextY = e.y + dirsMove[choice][1];

    if (!isBlocked(w, nextX, nextY)) {
        e.x = nextX;
        e.y = nextY;
    }

    // Shoot every 3 turns.
    if (e.t < 3) {
        return;
    }

    e.t = 0;

    int dirsShoot[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    for (int i = 0; i < 4; i++) {
        int dx = dirsShoot[i][0];
        int dy = dirsShoot[i][1];

        int spawnX = e.x + dx;
        int spawnY = e.y + dy;

        if (isBlocked(w, spawnX, spawnY)) {
            continue;
        }

        if (!canSpawnMoreProjectiles(upgrades, elist)) {
            return;
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
// XTUR
// Moving turret that shoots diagonal projectiles.
void bXTurret(entity &e, vector<entity> &elist, vector<pair<int,int>> &w, UpgradeState &upgrades) {
    e.t++;

    // Random movement every turn.
    int dirsMove[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    int choice = rando % 4;
    int nextX = e.x + dirsMove[choice][0];
    int nextY = e.y + dirsMove[choice][1];

    if (!isBlocked(w, nextX, nextY)) {
        e.x = nextX;
        e.y = nextY;
    }

    // Shoot every 3 turns.
    if (e.t < 3) {
        return;
    }

    e.t = 0;

    int dirsShoot[4][2] = {
        {-1, -1},
        {-1, 1},
        {1, -1},
        {1, 1}
    };

    for (int i = 0; i < 4; i++) {
        int dx = dirsShoot[i][0];
        int dy = dirsShoot[i][1];

        int spawnX = e.x + dx;
        int spawnY = e.y + dy;

        if (isBlocked(w, spawnX, spawnY)) {
            continue;
        }

        if (!canSpawnMoreProjectiles(upgrades, elist)) {
            return;
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
// Updates all active entities.
void updateentities(vector<entity> &e, player &p, vector<pair<int,int>> &w, UpgradeState &upgrades) {
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
    else if (e[i].type == "leaper") {
    bLeaper(e[i], p, w);
    }
    else if (e[i].type == "turret") {
      bTurret(e[i], e, w, upgrades);
    }
    else if (e[i].type == "shooter") {
      bShooter(e[i], p, e, w, i, upgrades);
    }
    else if (e[i].type == "projectile") {
      if (shouldProjectileMove(upgrades)) {
        bProjectile(e[i], w);
      }
    }
    else if (e[i].type == "gate") {
      bGate(e[i], p);
    }
    else if (e[i].type == "coin") {
      bCoin(e[i], p);
    }
    else if (e[i].type == "grappler") {
    bGrappler(e[i], p, w);
    }
    else if (e[i].type == "+tur") {
    bPlusTurret(e[i], e, w, upgrades);
    }
    else if (e[i].type == "xtur") {
    bXTurret(e[i], e, w, upgrades);
    }
  }
}

bool isDamagingEnemy(entity e) {
    return e.c != -1 && (
        e.type == "bouncer" ||
        e.type == "follow" ||
        e.type == "ghost" ||
        e.type == "leaper" ||
        e.type == "shooter" ||
        e.type == "grappler" ||
        e.type == "+tur" ||
        e.type == "xtur" ||
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
