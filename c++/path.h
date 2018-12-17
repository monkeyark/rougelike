#ifndef PATH_H
# define PATH_H

#include "dungeon.h"

void print_dijkstra_path(int dist[ROW * COL]);
void dijkstra_tunneling(Character *npc);
void dijkstra_nontunneling(Character *npc);

#endif