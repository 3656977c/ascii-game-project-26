#include "enemies.h"
#include <cstdlib>
#include <cmath>
extern int n;
extern int m;
extern int open;

// BOUNCER
// Moves diagonally and bounces off the map borders
void bBouncer(entity &e) {
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
    if (nextX >= 0 && nextX < n) {
        e.x = nextX;
    }
    if (nextY >= 0 && nextY < m) {
        e.y = nextY;
    }

}

// FOLLOWER
// Moves one tile toward the player every other turn.
void bFollow(entity &e, player p) {
    if (e.t == 1) {
        if (abs(e.x - p.x) != 0 && abs(e.y - p.y) != 0) {
            if (rand() % 2 == 0) {
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
    } else {
        e.t++;
    }

}
//GHOST
// Similar to follower, but should be treated as ignoring walls.
void bGhost(entity &e, player p) {
    if (e.t == 1) {
        if (abs(e.x - p.x) > abs(e.y - p.y)) {
            if (p.x > e.x) e.x++;
            else if (p.x < e.x) e.x--;
        } else {
            if (p.y > e.y) e.y++;
            else if (p.y < e.y) e.y--;
        }

        e.t = 0;
    } else {
        e.t++;
    }

}
// CHARGER
// Moves up to 2 tiles toward the player in a straight direction.
void bCharger(entity &e, player p) {
    if (e.t < 1) {
        e.t++;
        return;
    }

    for (int step = 0; step < 2; step++) {
        if (abs(e.x - p.x) > abs(e.y - p.y)) {
            if (p.x > e.x && e.x < n - 1) e.x++;
            else if (p.x < e.x && e.x > 0) e.x--;
        } else {
            if (p.y > e.y && e.y < m - 1) e.y++;
            else if (p.y < e.y && e.y > 0) e.y--;
        }
    }

    e.t = 0;
}
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
        int choice = rand % 4;

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
