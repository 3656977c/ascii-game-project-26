#include "declar.h"

//displays a n*n grid
void display(vector<char> &gmap, int n, int x, int y, int ex, int ey) {
    for (int i = 0; i < n*n; i++) {
        if (i%n == 0) cout << "\n";
        if (i == x+y*n) {
            cout << c_green << gmap[i] << " ";
            continue;
        }
        if (i == ex+ey*n) {
            cout << c_red << gmap[i] << " ";
            continue;
        }
        cout << c_yellow << gmap[i] << " ";
    }
}

void enemy(int x, int y, int &ex, int &ey) {
    if (abs(ex-x) > abs(ey-y)) {
        if (x > ex) ex++;
        else ex--;
    } else {
        if (y > ey) ey++;
        else ey--;
    }
}

signed main() {
    int n; cin >> n;
    vector<char> gmap(n*n, '-');
    int x=0, y=0;
    int ex = n-1, ey = n-1;
    gmap[0] = 'P';
    gmap[n*n-1] = 'E';
    bool state = true;
    int co = 0;
    while (state) {
        display(gmap, n, x, y, ex, ey);
        gmap[x+y*n] = '-';
        char c; cin >> c;
        if (c == 'w' && y!=0) y--;
        else if (c == 's' && y!=n-1) y++;
        else if (c == 'a' && x!=0) x--;
        else if (c == 'd' && x!=n-1) x++;
        else if (c == 'q') state=false;
        gmap[x+y*n] = 'P';
        gmap[ex+ey*n] = '-';
        if (co%2 == 0) {
            enemy(x,y,ex,ey);
        }
        co++;
        gmap[ex+ey*n] = 'E';
        cout.flush();
    }
}