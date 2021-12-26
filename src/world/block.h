#ifndef BLOCK_H
#define BLOCK_H

#include <cglm/cglm.h>
#include "../state.h"

#define TEXCOORD_STEP 1 / 16

enum block_type
{
	BLOCK_SOLID,
	BLOCK_TRANSPARENT,
	BLOCK_INVISIBLE,
};

enum block_id
{
	BLOCK_AIR,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_PLANK,
	BLOCK_STONE,
	BLOCK_BEDROCK,
	BLOCK_LOG,
	BLOCK_LEAVES,
	BLOCK_SAND,
	BLOCK_COBBLESTONE,
	BLOCK_COAL_ORE,
	BLOCK_IRON_ORE,
	BLOCK_GOLD_ORE,
	BLOCK_DIAMOND_ORE,
	BLOCK_WATER,
	BLOCK_LAVA,
};

struct block
{
	int id;
	int texture_location[6];
	char *name;
	
	enum block_type type;
};

unsigned int
block_create(int id,
			 int t_index,
			 char *name,
			 enum block_type type);

void
block_get_texcoord(int block,
				   float *x,
				   float *y,
				   int direction);

int
block_get_type(int id);

void
block_init();

extern struct block BLOCKS[256];

#endif //BLOCK_H
