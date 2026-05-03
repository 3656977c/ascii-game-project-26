#include "declar.h"
#include "director.h"

//rand val - declared in main.cpp
extern unsigned seed;
extern mt19937 engine;
extern uniform_int_distribution<int> dist;
#define rand dist(engine) 

bool occupied(int x, int y, player p, vector<entity> elist, vector<pair<int,int>> wall) {
    if (p.x == x && p.y == y) return false;
    for (entity i: elist) {
        if (i.x == x && i.y == y) return false;
    }
    for (auto i: wall) {
        if (i.first == x && i.second == y) return false;
    }

    return true;
}

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

void director(gamestate& g, player& p) {
    //elist is the list of all entities, p is player, level is the danger value
    //vector<int> difficulty = {30, 40, 50, 60, 70, 80};
    vector<int> difficulty = {20, 25, 30, 35, 40, 45};
    int pts = difficulty[g.level];
    vector<entity> enemylineup = listgen(pts, g.epool);

    vector<int> section = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int temp = rand%9;
    //add more stuff
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

    //pick a section of the map for the player.
    //the player spawns in the middle of this section
    //the director then chooses a random starting layout for this

    //PLAYER SECTION
    //choose section
        int playersection = section[temp];
        section.erase(section.begin() + temp);
    //init section borders
        int bpx = 5 *((playersection - 1)%3);
        int bpy = 9 *((playersection - 1)/3);
    //spawn player
        p.x = bpx + 2 + rand%3 - 1;
        p.y = bpy + 4 + rand%3 - 1;
    //choose layout
        vector<vector<pair<int,int>>> playout = {
            {},
            {{1, 2},{1, 6},{2, 1},{2, 2},{2, 6},{2, 7},{3, 2},{3, 6}},
            {{1, 1},{1, 2},{1, 7},{3, 1},{3, 6},{3, 7}}
        };
        temp = rand%3;
        for (auto &i : playout[temp]) {
            g.wlist.emplace_back(bpx + i.first, bpy + i.second);
        }
    //GATE SECTION
    for (int i = 8; i > 5; i--) {
        temp = rand%i;
        //choose section
            int gsection = section[temp];
            section.erase(section.begin() + temp);
        //init section borders
            int bgx = 5 *((gsection - 1)%3);
            int bgy = 9 *((gsection - 1)/3);
        //choose layout
            temp = rand%12;
            for (auto &i : layout[temp]) {
                g.wlist.emplace_back(bgx + i.first, bgy + i.second);
            }
        //spawn gate
            int x = 0, y = 0;
            for (int i = 0; i < 50; i++) {
                x = rand%5 + bgx; y = rand%9 + bgy;
                if (occupied(x, y, p, g.elist, g.wlist)) break;
                x = 1; y = 2;
            }
            entity gate = {x, y, "gate", 'G', 3, -1, -1, -1};
            g.elist.push_back(gate);
        //spawn enemy
        int spawn = (enemylineup.size() + (i-1))/i;
        for (int i = 0; i < spawn; i++) {
            int x = 0, y = 0;
            for (int i = 0; i < 100; i++) {
                x = rand%5 + bgx; y = rand%9 + bgy;
                if (occupied(x, y, p, g.elist, g.wlist)) {
                    g.elist.push_back({x, y, enemylineup.back().type, 
                        enemylineup.back().s, enemylineup.back().c, 
                        enemylineup.back().ax, enemylineup.back().ay, 
                        enemylineup.back().t});
                    enemylineup.pop_back();
                    break;
                }
            }
        }
    }

    //COIN SECTION
    for (int i = 5; i > 3; i--) {
        temp = rand%i;
    //choose section
        int gsection = section[temp];
        section.erase(section.begin() + temp);
    //init section borders
        int bgx = 5 *((gsection - 1)%3);
        int bgy = 9 *((gsection - 1)/3);
    //choose layout
        temp = rand%12;
        for (auto &i : layout[temp]) {
            g.wlist.emplace_back(bgx + i.first, bgy + i.second);
        }
    //spawn coins
        int x = 0, y = 0;
        for (int i = 0; i < 50; i++) {
            x = rand%5 + bgx; y = rand%9 + bgy;
            if (occupied(x, y, p, g.elist, g.wlist)) break;
            x = 1; y = 2;
        }
        entity coin = {x, y, "coin", 'c', 3, -1, -1, -1};
        g.elist.push_back(coin);
    //spawn enemy
        int spawn = (enemylineup.size() + (i-1))/i;
        for (int i = 0; i < spawn; i++) {
            int x = 0, y = 0;
            for (int i = 0; i < 100; i++) {
                x = rand%5 + bgx; y = rand%9 + bgy;
                if (occupied(x, y, p, g.elist, g.wlist)) {
                    g.elist.push_back({x, y, enemylineup.back().type, 
                        enemylineup.back().s, enemylineup.back().c, 
                        enemylineup.back().ax, enemylineup.back().ay, 
                        enemylineup.back().t});
                    enemylineup.pop_back();
                    break;
                }
            }
        }
    }

    //ENEMY SECTION
    for (int i = 3; i > 0; i--) {
        temp = rand%i;
    //choose section
        int gsection = section[temp];
        section.erase(section.begin() + temp);
    //init section borders
        int bgx = 5 *((gsection - 1)%3);
        int bgy = 9 *((gsection - 1)/3);
    //choose layout
        temp = rand%12;
        for (auto &i : layout[temp]) {
            g.wlist.emplace_back(bgx + i.first, bgy + i.second);
        }
    //spawn enemy
        int spawn = (enemylineup.size() + (i-1))/i;
        for (int i = 0; i < spawn; i++) {
            int x = 0, y = 0;
            for (int i = 0; i < 100; i++) {
                x = rand%5 + bgx; y = rand%9 + bgy;
                if (occupied(x, y, p, g.elist, g.wlist)) {
                    g.elist.push_back({x, y, enemylineup.back().type, 
                        enemylineup.back().s, enemylineup.back().c, 
                        enemylineup.back().ax, enemylineup.back().ay, 
                        enemylineup.back().t});
                    enemylineup.pop_back();
                    break;
                }
            }
        }
    }
}

void updater(int &val, pair<int, entity> &target, gamestate &g, player p) {
    if (val > 25 + 3*target.first) {
        int x = 0, y = 0;
        for (int i = 0; i < 100; i++) {
            x = rand%15; y = rand%27;
            if (occupied(x, y, p, g.elist, g.wlist)) {
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