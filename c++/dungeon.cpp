#include <math.h>
#include <string.h>
#include <sys/stat.h>

#include <vector>

#include "dungeon.h"
#include "move.h"
#include "path.h"
#include "parser.h"

void init_dungeon()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			dungeon.map[i][j].terrain = ROCK;
			dungeon.map[i][j].space = ROCK;
			dungeon.map[i][j].fog = ROCK;
			dungeon.map[i][j].hardness = ROCK_H;
		}
	}
}

int get_random(int modulus, int min)
{
	return rand() % modulus + min;
}

int is_monster(int row, int col)
{
	for (int i = 0; i < dungeon.num_mon; i++)
	{
		if (row == dungeon.npcs[i].row &&
			col == dungeon.npcs[i].col &&
			!dungeon.npcs[i].dead)
		{
			return i;
		}
	}
	return -1;
}

int is_item(int row, int col)
{
	for (int i = 0; i < dungeon.num_item; i++)
	{
		if (row == dungeon.items[i].row &&
			col == dungeon.items[i].col)
		{
			return i;
		}
	}
	return -1;
}

bool is_on_floor_item(int row, int col)
{
	int item_index = is_item(row, col);
	if (item_index >= 0)
	{
		if (dungeon.items[item_index].status == ITEM_ON_FLOOR)
		{
			return true;
		}
	}
	return false;
}

void print_dungeon()
{
	printf("   ");
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

	printf("   ");
	for (int i = 0; i < COL; i++)
	{
		printf("%d", i % 10);
	}
	printf("\n");

	printf("  ");
	for (int i = 0; i < COL + 2; i++)
	{
		printf("-");
	}
	printf("\n");

	for (int i = 0; i < ROW; i++)
	{
		printf("%2d|", i);
		for (int j = 0; j < COL; j++)
		{
			printf("%c", dungeon.map[i][j].space);
		}
		printf("|\n");
	}
	printf("  ");
	for (int i = 0; i < COL + 2; i++)
	{
		printf("-");
	}
	printf("\n");
}

int is_inside(int row, int col)
{
	//is room not on edge or outside of dungeon or cross dungeon
	return row > 0 && col > 0 && row < ROW - 1 && col < COL - 1;
}

int is_room(int row, int col)
{
	return dungeon.map[row][col].terrain == ROOM;
}

int is_water_lava(int row, int col)
{
	return dungeon.map[row][col].terrain == WATER ||
		dungeon.map[row][col].terrain == LAVA;
}

int is_room_corridor_stair(int row, int col)
{
	return dungeon.map[row][col].terrain == ROOM ||
		dungeon.map[row][col].terrain == CORRIDOR ||
		dungeon.map[row][col].terrain == STAIR_UP ||
		dungeon.map[row][col].terrain == STAIR_DOWN;
}

int is_valid_room(int row, int col, int width, int height)
{
	//is current terrain free
	if (dungeon.map[row][col].terrain != ROCK)
		return 0;
	if (!is_inside(row, col) || !is_inside(row + height, col + width))
		return 0;
	//touch or overlap another room
	for (int i = row - 1; i < row + height + 2; i++)
	{
		if (dungeon.map[i][col - 1].terrain != ROCK)
			return 0; //left touch
		if (dungeon.map[i][col].terrain != ROCK)
			return 0; //left overlap
		if (dungeon.map[i][col + width + 1].terrain != ROCK)
			return 0; //right touch
		if (dungeon.map[i][col + width].terrain != ROCK)
			return 0; //right overlap
	}
	for (int j = col - 1; j < col + width + 2; j++)
	{
		if (dungeon.map[row - 1][j].terrain != ROCK)
			return 0; //top touch
		if (dungeon.map[row][j].terrain != ROCK)
			return 0; //top overlap
		if (dungeon.map[row + height + 1][j].terrain != ROCK)
			return 0; //bottom touch
		if (dungeon.map[row + height][j].terrain != ROCK)
			return 0; //bottom overlap
	}

	return 1;
}

Water new_water()
{
	Water r;
	r.row = get_random(ROW, 0);
	r.col = get_random(COL, 0);
	r.width = get_random(3, 3);
	r.height = get_random(3, 3);

	int validRoom = is_valid_room(r.row, r.col, r.width, r.height);

	if (validRoom)
	{
		for (int i = r.row; i < r.row + r.height; i++)
		{
			for (int j = r.col; j < r.col + r.width; j++)
			{
				dungeon.map[i][j].terrain = WATER;
				dungeon.map[i][j].space = WATER;
				dungeon.map[i][j].hardness = 0;
			}
		}
	}
	else
	{
		return new_water();
	}

	return r;
}

Lava new_lava()
{
	Lava r;
	r.row = get_random(ROW, 0);
	r.col = get_random(COL, 0);
	r.width = get_random(3, 3);
	r.height = get_random(3, 3);

	int validRoom = is_valid_room(r.row, r.col, r.width, r.height);

	if (validRoom)
	{
		for (int i = r.row; i < r.row + r.height; i++)
		{
			for (int j = r.col; j < r.col + r.width; j++)
			{
				dungeon.map[i][j].terrain = LAVA;
				dungeon.map[i][j].space = LAVA;
				dungeon.map[i][j].hardness = 0;
			}
		}
	}
	else
	{
		return new_lava();
	}

	return r;
}

Room new_room()
{
	Room r;
	r.row = get_random(ROW, 0);
	r.col = get_random(COL, 0);
	r.width = get_random(7, 3);
	r.height = get_random(6, 2);

	int validRoom = is_valid_room(r.row, r.col, r.width, r.height);

	if (validRoom)
	{
		for (int i = r.row; i < r.row + r.height; i++)
		{
			for (int j = r.col; j < r.col + r.width; j++)
			{
				dungeon.map[i][j].terrain = ROOM;
				dungeon.map[i][j].space = ROOM;
				dungeon.map[i][j].hardness = ROOM_H;
			}
		}
	}
	else
	{
		return new_room();
	}

	return r;
}

Room set_room(int row, int col, int width, int height)
{
	Room r;
	r.row = row;
	r.col = col;
	r.width = width;
	r.height = height;

	for (int i = row; i < row + height; i++)
	{
		for (int j = col; j < col + width; j++)
		{
			dungeon.map[i][j].terrain = ROOM;
			dungeon.map[i][j].space = ROOM;
			dungeon.map[i][j].hardness = ROOM_H;
		}
	}

	return r;
}

int distance(int aRow, int aCol, int bRow, int bCol)
{
	int row = abs(aRow - bRow);
	int col = abs(aCol - bCol);

	return row * row + col * col;
}

int is_connected(int row, int col)
{
	return dungeon.map[row - 1][col].terrain == CORRIDOR ||
		dungeon.map[row + 1][col].terrain == CORRIDOR ||
		dungeon.map[row][col - 1].terrain == CORRIDOR ||
		dungeon.map[row][col + 1].terrain == CORRIDOR;
}

void new_corridor(int aRow, int aCol, int bRow, int bCol)
{
	if (distance(aRow, aCol, bRow, bCol) == 0)
		return;
	if (dungeon.map[aRow][aCol].terrain == CORRIDOR && is_connected(bRow, bCol))
		return;
	if (dungeon.map[aRow][aCol].terrain == ROCK)
	{
		dungeon.map[aRow][aCol].terrain = CORRIDOR;
		dungeon.map[aRow][aCol].space = CORRIDOR;
	}

	int max = distance(0, 0, ROW, COL);
	int min = max;
	int top = max;
	int down = max;
	int left = max;
	int right = max;

	if (is_inside(aRow - 1, aCol))
	{
		top = distance(aRow - 1, aCol, bRow, bCol);
		min = MIN(min, top);
	}
	if (is_inside(aRow + 1, aCol))
	{
		down = distance(aRow + 1, aCol, bRow, bCol);
		min = MIN(min, down);
	}
	if (is_inside(aRow, aCol - 1))
	{
		left = distance(aRow, aCol - 1, bRow, bCol);
		min = MIN(min, left);
	}
	if (is_inside(aRow, aCol + 1))
	{
		right = distance(aRow, aCol + 1, bRow, bCol);
		min = MIN(min, right);
	}

	if (min == top)
	{
		new_corridor(aRow - 1, aCol, bRow, bCol);
	}
	else if (min == down)
	{
		new_corridor(aRow + 1, aCol, bRow, bCol);
	}
	else if (min == left)
	{
		new_corridor(aRow, aCol - 1, bRow, bCol);
	}
	else if (min == right)
	{
		new_corridor(aRow, aCol + 1, bRow, bCol);
	}
}

void new_stair()
{
	int stair_up_row = get_random(ROW, 0);
	int stair_up_col = get_random(COL, 0);
	int stair_down_row = get_random(ROW, 0);
	int stair_down_col = get_random(COL, 0);

	if (is_room(stair_up_row, stair_up_col) &&
			is_room(stair_down_row, stair_down_col))
	{
		dungeon.map[stair_up_row][stair_up_col].terrain = STAIR_UP;
		dungeon.map[stair_up_row][stair_up_col].space = STAIR_UP;
		dungeon.map[stair_up_row][stair_up_col].hardness = 0;
		dungeon.map[stair_down_row][stair_down_col].terrain = STAIR_DOWN;
		dungeon.map[stair_down_row][stair_down_col].space = STAIR_DOWN;
		dungeon.map[stair_down_row][stair_down_col].hardness = 0;
	}
	else
	{
		new_stair();
	}
}

int is_visible_terrain(int i, int j, PC *pc)
{
	
	return i >= pc->row - pc->vision_range &&
		i <= pc->row + pc->vision_range &&
		j >= pc->col - pc->vision_range &&
		j <= pc->col + pc->vision_range;
}

void map_memorize(PC *pc)
{
	int row, col;
	for (row = pc->row - pc->vision_range; row < pc->row + pc->vision_range + 1; row++)
	{
		for (col = pc->col - pc->vision_range; col < pc->col + pc->vision_range + 1; col++)
		{
			if (row >= 0 && col >= 0 && row < ROW && col < COL)
			{
				pc->vision_map[row][col] = 1;
				//if (is_monster(row, col) && is_visible_terrain(row, col))
				if (is_visible_terrain(row, col, pc))
				{
					dungeon.map[row][col].fog = dungeon.map[row][col].space;
				}
				else
				{
					dungeon.map[row][col].fog = dungeon.map[row][col].terrain;
				}
			}
		}
	}
}

PC new_PC()
{
	PC pc;

	pc.birth = -1;
	pc.dead = 0;
	pc.row = dungeon.rooms[0].row;
	pc.col = dungeon.rooms[0].col;
	pc.speed = 10;
	pc.vision_range = PC_BASE_VISION_RADIUS;
	pc.hitpoints = PC_FULL_HP;
	pc.regen = 7;
	pc.damage = dice(0, 1, 4);
	pc.superman = false;

	for (int i = 0; i < NUM_EQUIPMENT; i++)
	{
		pc.is_equiped[i] = false;
	}

	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			pc.vision_map[i][j] = 0;
		}
	}
	map_memorize(&pc);
	dungeon.map[pc.row][pc.col].space = PLAYER;

	return pc;
}

NPC new_NPC(int birth)
{
	NPC npc;
	int row, col;
	//add random npc postion
	do
	{
		row = get_random(ROW, 0);
		col = get_random(COL, 0);
	} while (dungeon.map[row][col].space != ROOM && dungeon.map[row][col].space != CORRIDOR);

	int index, rarity;
	bool is_unique_generated;
	Monster mons;
	do
	{
		index = get_random(dungeon.monster_desc.size(), 0);
		rarity = get_random(99, 0);
		mons = dungeon.monster_desc.at(index);
		is_unique_generated = mons.seen && (mons.ability & NPC_UNIQ);
		if (rarity >= mons.rarity)
		{
			continue;
		}
		dungeon.monster_desc.at(index).seen = true;
	} while ((rarity >= mons.rarity) || is_unique_generated);

	npc.row = row;
	npc.col = col;
	npc.birth = birth;
	npc.dead = 0;

	//creating NPC with all four ability having 1/2 probability, clean unused bits
	npc.ability = mons.ability;
	npc.damage = mons.damage;
	npc.symbol = mons.symbol;
	npc.color = mons.color;
	npc.color_display = mons.color_display;
	npc.hitpoints = mons.hitpoints.roll();
	npc.speed = mons.speed.roll();
	npc.name = mons.name.c_str();
	npc.description = mons.description.c_str();
	if (npc.ability & NPC_TELEPATH) //monster is telepath
	{
		npc.pc_row = dungeon.pc->row;
		npc.pc_col = dungeon.pc->col;
	}

	dungeon.map[npc.row][npc.col].space = npc.symbol;

	return npc;
}

bool is_inventory_full()
{
	return dungeon.pc->inventory.size() == PC_INVENTORY;
}

Item new_item(int birth)
{
	Item item;
	int row, col;
	//add random object postion
	do
	{
		row = get_random(ROW, 0);
		col = get_random(COL, 0);
	} while (dungeon.map[row][col].space != ROOM && dungeon.map[row][col].space != CORRIDOR);

	int index, rarity;
	bool is_artifact_generated;
	Object obj;
	do
	{
		//get random index of object from description
		index = get_random(dungeon.object_desc.size(), 0);
		rarity = get_random(99, 0);
		obj = dungeon.object_desc.at(index);
		is_artifact_generated = dungeon.object_desc.at(index).seen && dungeon.object_desc.at(index).artifact;
		if (rarity >= obj.rarity)
		{
			continue;
		}
		dungeon.object_desc.at(index).seen = true;
	} while ((rarity >= obj.rarity) || is_artifact_generated);

	item.row = row;
	item.col = col;
	item.birth = birth;

	//item.name = obj.name.c_str();
	item.name = new char[obj.name.length() +1];
	strcpy(item.name, obj.name.c_str());
	//std::cout << "name of monster :" << item.name << std::endl;
	//std::cout << "name of the coming object : "<<obj.name.c_str() << std::endl<<std::endl;
	//item.description = obj.description.c_str();
	item.description = new char [obj.description.length()+1];
	strcpy(item.description, obj.description.c_str());
	item.color = obj.color;
	item.color_display = obj.color_display;
	item.damage = obj.damage;
	item.damage_bonus = obj.damage.roll();
	item.hit = obj.hit.roll();
	item.dodge = obj.dodge.roll();
	item.defence = obj.defence.roll();
	item.weight = obj.weight.roll();
	item.speed = obj.speed.roll();
	item.attribute = obj.attribute.roll();
	item.value = obj.value.roll();
	item.artifact = obj.artifact;
	item.rarity = obj.rarity;
	item.type = obj.type;
	item.status = ITEM_ON_FLOOR;
	if (obj.type == LIGHT)//TODO
	{
		item.vision_bonus = 5;
	}
	
	//item.type_string = obj.type_string.c_str();
	item.type_string = new char [obj.type_string.length()+1];
	strcpy(item.type_string,obj.type_string.c_str());
	item.symbol = obj.symbol;

	dungeon.map[item.row][item.col].space = item.symbol;

	return item;
}

void generate_dungeon()
{
	std::string path_monster = "./description/monster_desc.txt";
	std::string path_object = "./description/object_desc.txt";
	load_monster_desc(path_monster);
	load_object_desc(path_object);

	//initialize dungeon
	init_dungeon();

	//generate random number of water
	dungeon.num_water = get_random(4, 2);
	//generate random number of lava
	dungeon.num_lava = get_random(4, 2);
	//generate random number of rooms
	dungeon.num_room = get_random(7, 5);
	//generate random number of monster
	if (!dungeon.num_mon)
	{
		dungeon.num_mon = get_random(5, 8);
	}
	//dungeon.num_mon = 1;//DEBUG
	dungeon.num_item = get_random(5, 11);

	int i;
	//add lava
	for (i = 0; i < dungeon.num_lava; i++)
	{
		//dungeon.lavas[i] = new_lava();
		dungeon.lavas.push_back(new_lava());
	}

	//add water
	for (i = 0; i < dungeon.num_water; i++)
	{
		//dungeon.waters[i] = new_water();
		dungeon.waters.push_back(new_water());
	}
	
	//add rooms
	for (i = 0; i < dungeon.num_room; i++)
	{
		//dungeon.rooms[i] = new_room();
		dungeon.rooms.push_back(new_room());
	}

	int row_from, col_from, row_to, col_to;
	//add corridors between rooms
	for (i = 0; i < dungeon.num_room - 1; i++)
	{
		row_from = dungeon.rooms[i].row;
		col_from = dungeon.rooms[i].col;
		row_to = dungeon.rooms[i + 1].row;
		col_to = dungeon.rooms[i + 1].col;
		new_corridor(row_from, col_from, row_to, col_to);
	}
	
	//add corridors between rooms and waters
	row_from = dungeon.rooms[dungeon.num_room - 1].row;
	col_from = dungeon.rooms[dungeon.num_room - 1].col;
	row_to = dungeon.waters[0].row;
	col_to = dungeon.waters[0].col;
	new_corridor(row_from, col_from, row_to, col_to);

	//add corridors between waters
	for (i = 0; i < dungeon.num_water - 1; i++)
	{
		row_from = dungeon.waters[i].row;
		col_from = dungeon.waters[i].col;
		row_to = dungeon.waters[i + 1].row;
		col_to = dungeon.waters[i + 1].col;
		new_corridor(row_from, col_from, row_to, col_to);
	}

	//add corridors between waters and lavas
	row_from = dungeon.waters[dungeon.num_water - 1].row;
	col_from = dungeon.waters[dungeon.num_water - 1].col;
	row_to = dungeon.lavas[0].row;
	col_to = dungeon.lavas[0].col;
	new_corridor(row_from, col_from, row_to, col_to);

	//add corridors between lavas
	for (i = 0; i < dungeon.num_lava - 1; i++)
	{
		row_from = dungeon.lavas[i].row;
		col_from = dungeon.lavas[i].col;
		row_to = dungeon.lavas[i + 1].row;
		col_to = dungeon.lavas[i + 1].col;
		new_corridor(row_from, col_from, row_to, col_to);
	}


	//add stair
	new_stair();

	//add pc
	PC player = new_PC();
	dungeon.pc = &player;

	//add npc
	for (i = 0; i < dungeon.num_mon; i++)
	{
		//dungeon.npc[i] = new_NPC(i);
		dungeon.npcs.push_back(new_NPC(i));
	}

	//add object
	for (i = 0; i < dungeon.num_item; i++)
	{
		dungeon.items.push_back(new_item(i));
	}
}

void delete_dungeon()
{
	dungeon.pc->hitpoints = PC_FULL_HP;
	dungeon.pc->damage_bonus = 0;
	dungeon.pc->speed = 10;
}

void move_dungeon()
{
	move_character();
	//move_character_turn();
}

void load_dungeon()
{
	std::string path_dungeon = "./dungeonfile/dungeon";
	char *path = new char[path_dungeon.size() + 1];
    std::copy(path_dungeon.begin(), path_dungeon.end(), path);
    path[path_dungeon.size()] = '\0';
	FILE *f = fopen(path, "r");
	delete[] path;

	if (!f)
	{
		mkdir(path, 0777);
		fprintf(stderr, "Failed to open file when loading dungeon\n");
		return;
	}

	init_dungeon();

	char marker[12];
	fread(&marker, 1, 12, f);

	uint32_t ver;
	fread(&ver, 4, 1, f);
	dungeon.version = be32toh(ver);

	uint32_t file_size;
	fread(&file_size, 4, 1, f);
	int filesize = be32toh(file_size);

	uint8_t pc_col;
	fread(&pc_col, 1, 1, f);
	dungeon.pc->col = pc_col;
	uint8_t pc_row;
	fread(&pc_row, 1, 1, f);
	dungeon.pc->row = pc_row;

	uint8_t hard[1680];
	fread(hard, 1, 1680, f);

	for (int row = 0; row < ROW; row++)
	{
		for (int col = 0; col < COL; col++)
		{
			int index = COL * row + col;
			int h = hard[index];
			dungeon.map[row][col].hardness = h;
			if (h == 0)
			{
				dungeon.map[row][col].terrain = CORRIDOR;
			}
			else
			{
				dungeon.map[row][col].terrain = ROCK;
			}
		}
	}

	dungeon.num_room = (filesize - 1702) / 4;
	//dungeon.rooms = (Room *)malloc(dungeon.num_room * sizeof(Room));
	//TODO
	//need to rewrite room construction with vector

	uint8_t roomRead[filesize - 1702];
	fread(roomRead, 1, filesize - 1702, f);

	int n = 0;
	for (int i = 0; i < dungeon.num_room; i++)
	{
		dungeon.rooms[i].col = roomRead[n++];
		dungeon.rooms[i].row = roomRead[n++];
		dungeon.rooms[i].width = roomRead[n++];
		dungeon.rooms[i].height = roomRead[n++];
		
		set_room(dungeon.rooms[i].row, dungeon.rooms[i].col, dungeon.rooms[i].width, dungeon.rooms[i].height);
	}

	//add PC
	dungeon.map[dungeon.pc->row][dungeon.pc->col].terrain = PLAYER;
	dungeon.map[dungeon.pc->row][dungeon.pc->col].hardness = PC_H;

	fclose(f);
}

void save_dungeon()
{
	std::string path_dungeon = "./dungeonfile/dungeon";
	char *path = new char[path_dungeon.size() + 1];
    std::copy(path_dungeon.begin(), path_dungeon.end(), path);
    path[path_dungeon.size()] = '\0';
	FILE *f = fopen(path, "w");
	delete[] path;

	if (!f)
	{
		fprintf(stderr, "Failed to open file when saving dungeon\n");
		return;
	}

	//char *marker = "RLG327-F2018";
	const char *marker = "RLG327-F2018";
	fwrite(marker, 1, 12, f);

	int ver = htobe32(dungeon.version);
	fwrite(&ver, 4, 1, f);

	int filesize = 1702 + 4 * dungeon.num_room;
	filesize = htobe32(filesize);
	fwrite(&filesize, 4, 1, f);

	int pc_x = dungeon.pc->col;
	fwrite(&pc_x, 1, 1, f);
	int pc_y = dungeon.pc->row;
	fwrite(&pc_y, 1, 1, f);

	char *hard = (char *)malloc(1680);
	//write hardness
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			hard[COL * i + j] = (char)dungeon.map[i][j].hardness;
		}
	}
	fwrite(hard, 1, 1680, f);

	//write room
	unsigned char *loc = (unsigned char *)malloc(4 * dungeon.num_room);
	int n = 0;
	for (int i = 0; i < dungeon.num_room; i++)
	{
		loc[n++] = (unsigned char)dungeon.rooms[i].col;
		loc[n++] = (unsigned char)dungeon.rooms[i].row;
		loc[n++] = (unsigned char)dungeon.rooms[i].width;
		loc[n++] = (unsigned char)dungeon.rooms[i].height;
	}
	fwrite(loc, 1, 4 * dungeon.num_room, f);

	free(hard);
	free(loc);
	fclose(f);
}
