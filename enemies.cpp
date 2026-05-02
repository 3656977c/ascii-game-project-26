#include "enemies.h"
#include <cstdlib>
#include <cmath>
extern int n;
extern int m;
extern int open;

// BOUNCER
// Moves diagonally and bounces off the map borders.
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
