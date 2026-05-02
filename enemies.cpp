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
