#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>

#include "queue.h"
#include "dice.h"

#define ROW 21
#define COL 80
#define TERMINAL_ROW 24
#define TERMINAL_COL 80
#define ROCK ' '
#define ROOM '.'
#define WATER '~'
#define LAVA '^'
#define CORRIDOR '#'
#define PLAYER '@'
#define STAIR_UP '<'
#define STAIR_DOWN '>'
#define LAVA_DAMAGE 50
#define ROCK_H 255
#define ROOM_H 0
#define CORRIDOR_H 0
#define PC_H 0
#define PC_BASE_VISION_RADIUS 2
#define PC_INVENTORY 10
#define PC_FULL_HP 100000

#define MIN(a, b)						\
	({									\
	 __typeof__ (a) _a = (a);			\
	 __typeof__ (b) _b = (b);			\
	 _a < _b ? _a : _b; })

#define MAX(a, b)						\
	({									\
	 __typeof__ (a) _a = (a);			\
	 __typeof__ (b) _b = (b);			\
	 _a > _b ? _a : _b; })


#define NPC_SMART          0x00000001
#define NPC_TELEPATH       0x00000002
#define NPC_TUNNEL         0x00000004
#define NPC_ERRATIC        0x00000008
#define NPC_PASS_WALL      0x00000010
#define NPC_DESTROY_OBJ    0x00000020
#define NPC_PICKUP_OBJ     0x00000040
#define NPC_UNIQ           0x00000080
#define NPC_BOSS           0x00000100
#define NPC_BIT09          0x00000200
#define NPC_BIT10          0x00000400
#define NPC_BIT11          0x00000800
#define NPC_BIT12          0x00001000
#define NPC_BIT13          0x00002000
#define NPC_BIT14          0x00004000
#define NPC_BIT15          0x00008000
#define NPC_BIT16          0x00010000
#define NPC_BIT17          0x00020000
#define NPC_BIT18          0x00040000
#define NPC_BIT19          0x00080000
#define NPC_BIT20          0x00100000
#define NPC_BIT21          0x00200000
#define NPC_BIT22          0x00400000
#define NPC_BIT23          0x00800000
#define NPC_BIT24          0x01000000
#define NPC_BIT25          0x02000000
#define NPC_BIT26          0x04000000
#define NPC_BIT27          0x08000000
#define NPC_BIT28          0x10000000
#define NPC_BIT29          0x20000000
#define NPC_BIT30          0x40000000
#define NPC_BIT31          0x80000000

#define COLORS_BLACK       0x00000001
#define COLORS_RED         0x00000002
#define COLORS_GREEN       0x00000004
#define COLORS_YELLOW      0x00000008
#define COLORS_BLUE        0x00000010
#define COLORS_MAGENTA     0x00000020
#define COLORS_CYAN        0x00000040
#define COLORS_WHITE       0x00000080

enum
{
	WEAPON,
	OFFHAND,
	RANGED,
	ARMOR,
	HELMET,
	CLOAK,
	GLOVES,
	BOOTS,
	AMULET,
	LIGHT,
	RING,
	RING_SECONDARY,
	NUM_EQUIPMENT,
};

enum
{
	ITEM_IN_INVENTORY,
	ITEM_IN_EQUIPMENT,
	ITEM_ON_FLOOR,
	ITEM_IS_EXPUNGED,
	NUM_ITEM_STATUS,
};

class Tile
{
	public:
		char terrain;
		char space;
		char fog;
		int hardness;
};

class Terrain
{
	public:
		int row;
		int col;
		int width;
		int height;
};

class Room : public Terrain
{};

class Water : public Terrain
{};

class Lava : public Terrain
{};

class Item
{
	public:
		int row;
		int col;
		char symbol;
		char *name;
		char *description;
		unsigned int color;
		int color_display;
		dice damage;
		int damage_bonus;
		int vision_bonus;
		int hit;
		int dodge;
		int defence;
		int weight;
		int speed;
		int attribute;
		int value;
		bool artifact;
		int rarity;
		int type;
		char *type_string;

		int birth;
		int status;
};

class Character
{
	public:
		char symbol;
		int row;
		int col;
		int birth;
		int dist[ROW * COL];
		int speed;
		int dead;

		int hitpoints;
		dice damage;
		int regen;
};

class NPC : public Character
{
	public:
		unsigned int ability;
		int pc_row;
		int pc_col;
		unsigned int color;
		int color_display;
		const char *name;
		const char *description;
};

class PC : public Character
{
	public:
		int vision_range;
		int vision_map[ROW][COL];
		int damage_bonus;
		std::vector<Item> inventory;
		std::vector<Item> equipment;
		bool is_equiped[NUM_EQUIPMENT];
		bool superman;
};

class Monster
{
	public:
		std::string name;
		std::string description;
		unsigned int color;
		int color_display;
		std::string color_string;
		dice damage;
		dice hitpoints;
		dice speed;
		unsigned int ability;
		std::string ability_string;
		int rarity;
		char symbol;
		bool seen;
};

class Object
{
	public:
		std::string name;
		std::string description;
		unsigned int color;
		int color_display;
		std::string color_string;
		dice hit;
		dice damage;
		dice dodge;
		dice defence;
		dice weight;
		dice speed;
		dice attribute;
		dice value;
		int vision_bonus;
		bool artifact;
		int rarity;
		int type;
		std::string type_string;
		char symbol;
		bool seen;
};

class Dungeon
{
	public:
		int seed;
		int num_mon;
		int num_item;
		int num_room;
		int num_water;
		int num_lava;
		int version;
		std::vector<Room> rooms;
		std::vector<Water> waters;
		std::vector<Lava> lavas;
		std::vector<NPC> npcs;
		std::vector<Item> items;
		PC *pc;
		int cursor_row;
		int cursor_col;
		Tile map[ROW][COL];
		Queue pq_tunel;
		Queue pq_nontunel;
		std::vector<Monster> monster_desc;
		std::vector<Object> object_desc;
};

extern Dungeon dungeon;

void init_dungeon();
void generate_dungeon();
void generate_dungeon();
void print_dungeon();
void load_dungeon();
void save_dungeon();
void delete_dungeon();
void delete_dungeon_desc();
void move_dungeon();

int get_random(int modulus, int min);
int is_visible_terrain(int i, int j);
int is_inside(int row, int col);
int is_monster(int row, int col);
int is_item(int row, int col);
bool is_inventory_full();
bool is_on_floor_item(int row, int col);
void remember_map_PC();

#endif
