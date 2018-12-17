#include <stdio.h>
#include <stdbool.h>
#include <time.h>


#include "dungeon.h"
#include "queue.h"
#include "io.h"
#include "parser.h"


Dungeon dungeon;

int main(int argc, char *argv[])
{

    dungeon.num_mon = 0;
	//set up random seed
	dungeon.seed = time(NULL);
	dungeon.seed = 1542173115;
	srand(dungeon.seed);

	bool load = false;
	bool save = false;

	if (argc != 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "--save") != 0 &&
				strcmp(argv[i], "--load") != 0 &&
                strcmp(argv[i], "--nummon") != 0 &&
				!atoi(argv[i]))
			{
				fprintf(stderr, "Bad argument\n");
				return -1;
			}
			if (strcmp(argv[i], "--save") == 0)
			{
				save = true;
			}
			if (strcmp(argv[i], "--load") == 0)
			{
				load = true;
			}
			if (strcmp(argv[i], "--nummon") == 0)
			{
				dungeon.num_mon = atoi(argv[i+1]);
			}
		}
	}

/*
	load_monster_desc(path_monster);
	load_object_desc(path_object);
    print_monster_desc();
	print_object_desc();
*/

	if (load)
	{
		load_dungeon();
	}
	else
	{
		//generate_dungeon();
		generate_dungeon();
	}
    
	if (save)
	{
		save_dungeon();
	}


	move_dungeon();
    //print_dungeon();
    delete_dungeon();


	return 0;
}
