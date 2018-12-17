#include "path.h"


int get_hardness_cost(int hardness)
{
	if (hardness == 255)
		return 3;
	if (hardness == 0)
		return 1;
	return 1 + (hardness / 85);
}

void print_dijkstra_path(int dist[ROW * COL])
{
	for (int i = 0; i < COL; i++)
	{
		int row = (i - i % 10) / 10;
		if (i % 10 == 0)
		{
			printf("%d", row);
		}
		else
		{
			putchar(' ');
		}
	}
	printf("\n");
	for (int i = 0; i < COL; i++)
	{
		printf("%d", i % 10);
	}
	printf("\n");
	//TODO above
	//	putchar('\n');
	int i, j;
	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			if (dungeon.pc->row == i && dungeon.pc->col == j)
			{
				printf("%c", PLAYER);
			}
			else if (dist[i * COL + j] != -1)
			{
				int n = dist[i * COL + j] % 10;
				printf("%d", n);
			}
			else
			{
				printf("%c", ROCK);
			}
		}
		printf("\n");
	}
	putchar('\n');
}

void dijkstra_tunneling(Character *npc)
{
	int rowMove[8] = {-1, -1, -1, 0, +1, +1, +1, 0};
	int colMove[8] = {-1, 0, +1, +1, +1, 0, -1, -1};
	int i, j;
	
	Node *node = node_new(dungeon.pc->row * COL + dungeon.pc->col);

	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			if (!is_inside(i, j))
			{
				npc->dist[i * COL + j] = -1;
			}
			else if (dungeon.map[i][j].terrain != PLAYER)
			{
				npc->dist[i * COL + j] = ROW * COL + 1;
				pq_insert(dungeon.pq_tunel, &node, i * COL + j, npc->dist);
			}
		}
	}
	npc->dist[dungeon.pc->row * COL + dungeon.pc->col] = 0;

	while (!pq_isEmpty(dungeon.pq_tunel, &node))
	{
		int u = pq_pop(dungeon.pq_tunel, &node);
		for (i = 0; i < 8; i++)
		{
			int alt = 0;
			int v = u + rowMove[i] + colMove[i] * COL;
			if (0 > v || v > ROW * COL || npc->dist[v] == -1)
				continue;

			if (npc->dist[v] >= 0)
			{
				alt = npc->dist[u] + get_hardness_cost(dungeon.map[u / COL][u % COL].hardness);
				if (alt < npc->dist[v])
				{
					npc->dist[v] = alt;
					pq_insert(dungeon.pq_tunel, &node, v, npc->dist);
				}
			}
		}
	}
}

void dijkstra_nontunneling(Character *npc)
{
	int rowMove[8] = {-1, -1, -1, 0, +1, +1, +1, 0};
	int colMove[8] = {-1, 0, +1, +1, +1, 0, -1, -1};
	int i, j;
	
	Node *node = node_new(dungeon.pc->row * COL + dungeon.pc->col);

	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			if (dungeon.map[i][j].terrain == ROOM ||
				dungeon.map[i][j].terrain == CORRIDOR ||
				dungeon.map[i][j].terrain == WATER ||
				dungeon.map[i][j].terrain == LAVA ||
				dungeon.map[i][j].terrain == STAIR_UP ||
				dungeon.map[i][j].terrain == STAIR_DOWN)
			{
				npc->dist[i * COL + j] = ROW * COL + 1;
				pq_insert(dungeon.pq_nontunel, &node, i * COL + j, npc->dist);
			}
			else if (dungeon.map[i][j].terrain == ROCK)
			{
				npc->dist[i * COL + j] = -1;
			}
		}
	}
	npc->dist[dungeon.pc->row * COL + dungeon.pc->col] = 0;

	while (!pq_isEmpty(dungeon.pq_nontunel, &node))
	{
		int u = pq_pop(dungeon.pq_nontunel, &node);
		for (i = 0; i < 8; i++)
		{
			int alt = 0;
			int v = u + rowMove[i] + colMove[i] * COL;
			if (0 > v || v > ROW * COL)
				continue;

			if (npc->dist[v] >= 0)
			{
				alt = npc->dist[u] + 1;
				if (alt < npc->dist[v])
				{
					npc->dist[v] = alt;
					pq_insert(dungeon.pq_nontunel, &node, v, npc->dist);
				}
			}
		}
	}
}