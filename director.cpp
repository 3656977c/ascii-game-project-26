#include "declar.h"
#include "director.h"

//rand val - declared in main.cpp
extern unsigned seed;
extern mt19937 engine;
extern uniform_int_distribution<int> dist;
#define rand dist(engine) 

//global vals
vector<int> difficulty = {20, 25, 30, 35, 40, 55}; //vector<int> difficulty = {30, 40, 50, 60, 70, 80};
vector<vector<pair<int,int>>> layout = { 
    {},{},{},{}, //empty type layouts are 4 times as likely to spawn
    {{1, 1}, {1, 7}, {3, 7}},
    {{3, 1}, {1, 7}},
    {{1, 1}, {3, 1}},
    {{1, 8}, {2, 5}, {2, 6}, {2, 7}, {2, 8}, {3, 8}},
    {{1, 0}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {3, 0}},
    {{1, 4}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 4}},
    {{1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}, {3, 3}, {3, 4}, {3, 5}},
    {{0, 1}, {0, 2}, {0, 5}, {0, 6}, {0, 7}, {4, 1}, {4, 2}, {4, 3}, {4, 6}, {4, 7}}
};
vector<vector<pair<int,int>>> playout = {
    {},
    {{1, 2},{1, 6},{2, 1},{2, 2},{2, 6},{2, 7},{3, 2},{3, 6}},
    {{1, 1},{1, 2},{1, 7},{3, 1},{3, 6},{3, 7}}
};
entity turret = {0, 0, "turret", '#', 2, -1, -1, 0};
entity leaper = {0, 0, "leaper", '^', 2, -1, -1, 0};
entity mushroom = {0, 0, "mushroom", '!', 2, -1, -1, 0};
entity grappler = {0, 0, "grappler", '$', 2, -1, -1, 0};
entity ttur = {0, 0, "+tur", 'x', 2, -1, -1, 0};
entity xtur = {0, 0, "xtur", '+', 2, -1, -1, 0};
vector<pair<int, entity>> enemies = {
    {5, turret},
    {4, leaper},
    {5, mushroom},
    {5, grappler},
    {3, ttur},
    {3, xtur}
};


//checks if an area is occupied by an entity, wall, or player
bool occupied(int x, int y, player p, gamestate g) {
    if (p.x == x && p.y == y) return false;
    for (entity i: g.elist) {
        if (i.x == x && i.y == y) return false;
    }
    for (auto i: g.wlist) {
        if (i.first == x && i.second == y) return false;
    }
    return true;
}

//generate a list for the the director follows in spawning enemies
vector<entity> listgen(int pts, vector<pair<int, entity>> epool) {
    if (epool.size() == 0) return {};
    int temp = rand%(epool.size());
    vector<entity> list;

    while (pts - epool[temp].first > 0) {
        pts -= epool[temp].first;
        list.push_back(epool[temp].second);
        temp = rand%(epool.size());

        if (epool[temp].second.t > 0) list.back().t = rand%3;
    }
    return list;
}

//try to force a spawn on a free tile
bool forcespawn(player p, gamestate &g, entity e, int bgx, int bgy) { 
    int x = 0, y = 0;
    for (int i = 0; i < 100; i++) { //try a spawn 100 times
        x = rand%5 + bgx;
        y = rand%9 + bgy;
        if (occupied(x, y, p, g)) {
            entity n = e;
            n.x = x; n.y = y;
            g.elist.push_back(n);
            return true;
        }
    }
    return false;
}


void director(gamestate& g, player& p) {
    //if level is greater than one, add a new enemy to the loop
    int newen = rand%enemies.size();
    if (g.level > 1) g.epool.push_back(enemies[newen]);
    enemies.erase(enemies.begin() + newen);

    //generate list of enemies to spawn
    vector<entity> enemylineup = listgen(difficulty[g.level - 1], g.epool);

    //build a list of sections and a rand number
    vector<int> section = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    //take random sections, assign them specific areas (coins, gates, player), spawn enemies
    for (int i = 9; i > 0; i--) {
        int temp = rand%i;

        //player spawning
        if (i == 9) {
            //init seciton for player
            int playersection = section[temp];
            section.erase(section.begin() + temp);
            int bpx = 5 *((playersection - 1)%3);
            int bpy = 9 *((playersection - 1)/3);

            //spawn player
            p.x = bpx + 2 + rand%3 - 1;
            p.y = bpy + 4 + rand%3 - 1;

            //choose pre-defined layout
            temp = rand%3;
            for (auto &i : playout[temp]) {
                g.wlist.emplace_back(bpx + i.first, bpy + i.second);
            }
        } else { //everything else
            //choose section
            int gsection = section[temp];
            section.erase(section.begin() + temp);

            //init section borders
            int bgx = 5 *((gsection - 1)%3);
            int bgy = 9 *((gsection - 1)/3);

            //choose layout
            temp = rand%12;
            for (auto &it : layout[temp]) {
                g.wlist.emplace_back(bgx + it.first, bgy + it.second);
            }

            //spawn either coins or gates
            if (i <= 8 && i > 5) {
                //gates
                entity gate = {1, 2, "gate", 'G', 3, -1, -1, -1};
                if (!forcespawn(p, g, gate, bgx, bgy)) g.elist.push_back(gate);
            } else if (i <= 5 && i > 3) {
                entity coin = {1, 2, "coin", 'c', 3, -1, -1, -1};
                if (!forcespawn(p, g, coin, bgx, bgy)) g.elist.push_back(coin);
            }

            //spawn enemies
            int spawnnumb = (enemylineup.size() + (i-1))/i; //how many i should spawn in 1 section
            for (int it = 0; it < spawnnumb; it++) {
                forcespawn(p, g, enemylineup.back(), bgx, bgy);
                enemylineup.pop_back();
            }
        }
    }

}

void updater(int &val, pair<int, entity> &target, gamestate &g, player p) {
    if (val > 25 + 3*target.first) {
        int x = 0, y = 0;
        for (int i = 0; i < 100; i++) {
            x = rand%15; y = rand%27;
            if (occupied(x, y, p, g)) {
                g.elist.push_back({x, y, "spawner", 'X', 5, 0, 0, 6,
                {target.second.s, target.second.c}, target.second.type, {target.second.ax, target.second.ay, target.second.t}
                });
                break;
            }
        }
        val = -4;
        target = g.epool[rand%g.epool.size()];
    } else val++;
}