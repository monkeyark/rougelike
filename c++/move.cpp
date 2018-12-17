#include <cstring>

#include "dungeon.h"
#include "move.h"
#include "path.h"
#include "io.h"


/*
npc_move_func[c->npc->characteristics & 0x0000000f](d, c, next);

void (*npc_move_func[])(NPC *c) =
{
	npc_next_pos_00,
	npc_next_pos_01,
	npc_next_pos_02,
	npc_next_pos_03,
	npc_next_pos_04,
	npc_next_pos_05,
	npc_next_pos_06,
	npc_next_pos_07,
	npc_next_pos_08,
	npc_next_pos_09,
	npc_next_pos_0a,
	npc_next_pos_0b,
	npc_next_pos_0c,
	npc_next_pos_0d,
	npc_next_pos_0e,
	npc_next_pos_0f,
};
*/

void npc_next_pos_00(NPC *npc);
void npc_next_pos_01(NPC *npc);
void npc_next_pos_02(NPC *npc);
void npc_next_pos_03(NPC *npc);
void npc_next_pos_04(NPC *npc);

void npc_next_pos_05(NPC *npc)
{
	memset(npc->dist, 0, sizeof (npc->dist));

	dijkstra_nontunneling(npc);

	int rowMove[8] = {-1,  -1,  -1,   0,  +1,  +1,  +1,   0};
	int colMove[8] = {-1,   0,  +1,  +1,  +1,   0,  -1,  -1};

	int current_v = npc->col + npc->row * COL;
	int min_v = current_v;
	int min_dist = npc->dist[min_v];

	int next_row = npc->row; int next_col = npc->col;
	for (int i = 0; i < 8; i++)
	{
		int next_v = current_v + rowMove[i] * COL + colMove[i];
		if (next_v < 0 || next_v > ROW * COL) continue;
		if (next_v / 80 == 0 || next_v % 80 == 0 ) continue;
		int next_dist = npc->dist[next_v];

		if (next_dist == -1) continue;

		if (next_dist < min_dist)
		{
			min_v = next_v;
			min_dist = npc->dist[min_v];
			next_row = npc->row + rowMove[i];
			next_col = npc->col + colMove[i];
		}
	}

	if (dungeon.pc->row == next_row && dungeon.pc->col == next_col)
	{
		int damage = npc->damage.roll();
		if (dungeon.pc->hitpoints <= damage)
		{
			dungeon.pc->hitpoints = 0;
			dungeon.pc->dead = true;
			dungeon.pc->row = -1;
			dungeon.pc->col = -1;
		}
		else
		{
			dungeon.pc->hitpoints -= damage;
		}
	}
	else
	{
		dungeon.map[npc->row][npc->col].space = dungeon.map[npc->row][npc->col].terrain;
		npc->row = next_row;
		npc->col = next_col;
		dungeon.map[npc->row][npc->col].space = npc->symbol;
		dungeon.map[npc->row][npc->col].hardness = 0;
	}

	//check is npc in next terrain will be dead
	for (int i = 0; i < dungeon.num_mon; i++)
	{
		NPC *next_npc = &dungeon.npcs[i];
		if (next_npc->birth == npc->birth) continue;

		if (next_npc->row == next_row && next_npc->col == next_col)
		{
			//swap location of two npc if collide
			next_npc->row = npc->row;
			next_npc->col = npc->col;
		}
		else
		{
			dungeon.map[npc->row][npc->col].space = dungeon.map[npc->row][npc->col].terrain;
		}
	}


	npc->row = next_row;
	npc->col = next_col;
	dungeon.map[npc->row][npc->col].space = npc->symbol;
	dungeon.map[npc->row][npc->col].hardness = 0;
}

void npc_next_pos_06(NPC *npc){}

void npc_next_pos_07(NPC *npc)
{
	memset(npc->dist, 0, sizeof (npc->dist));

	dijkstra_tunneling(npc);

	int rowMove[8] = {-1,  -1,  -1,   0,  +1,  +1,  +1,   0};
	int colMove[8] = {-1,   0,  +1,  +1,  +1,   0,  -1,  -1};

	int current_v = npc->col + npc->row * COL;
	int min_v = current_v;
	int min_dist = npc->dist[min_v];

	int next_row = npc->row; int next_col = npc->col;
	for (int i = 0; i < 8; i++)
	{
		int next_v = current_v + rowMove[i] * COL + colMove[i];
		if (next_v < 0 || next_v > ROW * COL) continue;
		if (next_v / 80 == 0 || next_v % 80 == 0 ) continue;
		int next_dist = npc->dist[next_v];

		if (next_dist == -1) continue;

		if (next_dist < min_dist)
		{
			min_v = next_v;
			min_dist = npc->dist[min_v];
			next_row = npc->row + rowMove[i];
			next_col = npc->col + colMove[i];
		}
	}

	if (dungeon.map[next_row][next_col].hardness < 85)
	{
		if (dungeon.map[next_row][next_col].hardness != 0)
		{
			dungeon.map[next_row][next_col].hardness = 0;
		}
		npc->row = next_row;
		npc->col = next_col;
		if (dungeon.map[npc->row][npc->col].hardness == 0 &&
				dungeon.map[npc->row][npc->col].terrain == ROCK)
		{
			dungeon.map[npc->row][npc->col].terrain = CORRIDOR;
            dungeon.map[npc->row][npc->col].space = CORRIDOR;
		}
	}
	else
	{
		dungeon.map[next_row][next_col].hardness -= 85;
	}

	//check if next terrain is npc
	for (int i = 0; i < dungeon.num_mon; i++)
	{
		NPC *next_npc = &dungeon.npcs[i];
		if (next_npc->birth == npc->birth) continue;

		if (next_npc->row == next_row && next_npc->col == next_col)
		{
			next_npc->dead = true;
			next_npc->row = -1;
			next_npc->col = -1;
		}
		else if (dungeon.pc->row == next_row && dungeon.pc->col == next_col)
		{
			dungeon.pc->dead = true;
			dungeon.pc->row = -1;
			dungeon.pc->col = -1;
		}
	}
}

void npc_next_pos_08(NPC *npc);
void npc_next_pos_09(NPC *npc);
void npc_next_pos_0a(NPC *npc);
void npc_next_pos_0b(NPC *npc);
void npc_next_pos_0c(NPC *npc);
void npc_next_pos_0d(NPC *npc);
void npc_next_pos_0e(NPC *npc);
void npc_next_pos_0f(NPC *npc);

void move_npc()
{
	if (!dungeon.pc->dead)
	{
		int i;
		for (i = 0; i < dungeon.num_mon; i++)
		{
			NPC *npc = &dungeon.npcs[i];
			if (!npc->dead)
			{
				npc_next_pos_05(npc);
			}
		}
	}
}

const char *move_pc(int row_move, int col_move)
{
	const char *message;

	if (row_move == 0 && col_move == 0)
	{
		dungeon.pc->hitpoints = MIN(dungeon.pc->hitpoints + dungeon.pc->regen, PC_FULL_HP);
		move_npc();
		message = "PC is resting!";
	}
	else if ((dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == WATER) &&
			dungeon.pc->equipment_open[CLOAK])
	{
		message = "PC cannot swim in water without cloak!";
	}
	else if ((dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == LAVA) &&
			dungeon.pc->equipment_open[BOOTS])
	{
		message = "PC cannot walk on lava without boots!";
	}
	else if (is_inside(dungeon.pc->row + row_move, dungeon.pc->col + col_move) &&
			(dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == ROOM ||
			dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == CORRIDOR ||
			dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == STAIR_UP ||
			dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == STAIR_DOWN ||
			dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == WATER ||
			dungeon.map[dungeon.pc->row + row_move][dungeon.pc->col + col_move].terrain == LAVA))
	{
		int item_index = is_item(dungeon.pc->row, dungeon.pc->col);
		int npc_index = is_monster(dungeon.pc->row + row_move, dungeon.pc->col + col_move);
		if (item_index >= 0)
		{
			//iventory has space to pick up new item
			if (is_inventory_open())
			{
				//add item in current location to inventory
				dungeon.pc->inventory[dungeon.pc->inventory_size] = dungeon.item[item_index];
				dungeon.pc->inventory[dungeon.pc->inventory_size].name = dungeon.item[item_index].name;
				dungeon.item[item_index].row = -1;
				dungeon.item[item_index].col = -1;
				dungeon.pc->inventory_size++;
				message = "You picked up one item";
			}
			else
			{
				message = "Iventory is full, no space to pickup";
				dungeon.map[dungeon.pc->row][dungeon.pc->col].space = dungeon.item[item_index].symbol;
			}
		}
		else if (npc_index >= 0)
		{
			int d = dungeon.pc->damage.roll();

			if (dungeon.npcs[npc_index].hitpoints <= d)
			{
				dungeon.npcs[npc_index].dead = true;
				dungeon.npcs[npc_index].row = -1;
				dungeon.npcs[npc_index].col = -1;
				message = "You kill monster - ";
				dungeon.pc->row += row_move;
				dungeon.pc->col += col_move;
				dungeon.map[dungeon.pc->row][dungeon.pc->col].space = PLAYER;
				dungeon.map[dungeon.pc->row][dungeon.pc->col].hardness = 0;
			}
			else
			{
				dungeon.npcs[npc_index].hitpoints -= d;
				message = "PC is attacking";
			}
		}
		else
		{
			message = "";
			dungeon.map[dungeon.pc->row][dungeon.pc->col].space = dungeon.map[dungeon.pc->row][dungeon.pc->col].terrain;
			dungeon.pc->row += row_move;
			dungeon.pc->col += col_move;
			dungeon.map[dungeon.pc->row][dungeon.pc->col].space = PLAYER;
			dungeon.map[dungeon.pc->row][dungeon.pc->col].hardness = 0;
		}

		if (dungeon.map[dungeon.pc->row][dungeon.pc->col].terrain == WATER)
		{
			dungeon.pc->hitpoints = MIN(dungeon.pc->hitpoints + 5*dungeon.pc->regen, PC_FULL_HP);
		}
		else if (dungeon.map[dungeon.pc->row][dungeon.pc->col].terrain == LAVA)
		{
			dungeon.pc->hitpoints = MAX(dungeon.pc->hitpoints - LAVA_DAMAGE, 0);
		}
		else
		{
			dungeon.pc->hitpoints = MIN(dungeon.pc->hitpoints + dungeon.pc->regen, PC_FULL_HP);
		}

		move_npc();
		remember_map_PC();
	}
	else
	{
		message = "There's wall in the way!";
	}

	return message;
}

void move_character()
{
	dungeon_ncurses();
	//move_npc();
}


