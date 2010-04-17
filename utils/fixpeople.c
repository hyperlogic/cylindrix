/* Reads in the cooked binary people.dat file and converts it to
   a text file.
   Note: the reason this exists is so cylindrix will run on 64 bit and big endian platforms.
*/
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_CHARACTER_SOUNDS 7 /* Number of things a character can say */
typedef char string[80];

typedef enum { ATTACK, GET_ENERGY, PANIC, BEZERK,
               HIDE, GROUPUP, GET_PYLONS, PROTECT,
               KILL_RADAR_BASE, PROTECT_RADAR_BASE, EVADE } state_type;

struct character_t
{
	char            name[40];
	char            filename[15]; /* Name of the pcx file */
	string          sample_filenames[NUMBER_CHARACTER_SOUNDS];
	unsigned int    hack_padding[NUMBER_CHARACTER_SOUNDS];  /* NOTE: was SYS_SOUNDHANDLE */
	state_type      state;

	unsigned char   passive_aggressive;     /* 1-100...1-50 is passive 51-100 is aggressive */
	unsigned char   bravery_cowardice;      /* 1-100...same scheme */
	unsigned char   aerial_ground;          /* 1-100...same scheme */
	unsigned char   obedience_disobedience; /* 1-100...same scheme */
	unsigned char   pylon_grab;             /* 1-50 hates pylons...50-100 likes em */
	unsigned char   radar_kill;             /* 1-50 wont kill, 50-100 loves to kill */
	unsigned char   radar_protect;          /* 1-50 protects, 50-100 doesn't care */
	unsigned char   skill_level;            /* Skill level 1-5  */
	unsigned char   preferred_vehicle;      /* See vehicletype */
};

int s_count;

#define DUMP_ITEM(fmt, item, comment)							\
	do															\
    {															\
		s_count = fprintf(file_out, #fmt , item);				\
		while (s_count < 45)									\
		{														\
			fprintf(file_out, " ");								\
			s_count++;											\
		}														\
		fprintf(file_out, "%s%s\n", "# ", #comment );			\
	} while(0)													\

void main(int argc, char* argv[])
{
	FILE* file_in = fopen(argv[1], "rb");
	if (!file_in)
	{
		fprintf(stderr, "failed to open file \"%s\"\n", argv[1]);
		exit(1);
	}

	FILE* file_out = fopen("new_people.dat", "w");
	if (!file_out)
	{
		fprintf(stderr, "failed to open file \"new_people.dat\"\n", argv[1]);
		exit(1);
	}

	struct character_t characters[100];
	struct character_t* c = characters;
	while (fread(c, sizeof(struct character_t), 1, file_in) > 0)
	{
		DUMP_ITEM(%s, c->name, Name);
		DUMP_ITEM(%s, c->filename, Pcx file);

		DUMP_ITEM(%s, c->sample_filenames[0], Greeting);
		DUMP_ITEM(%s, c->sample_filenames[0], Affirmation);
		DUMP_ITEM(%s, c->sample_filenames[0], Negation);
		DUMP_ITEM(%s, c->sample_filenames[0], Gloat);
		DUMP_ITEM(%s, c->sample_filenames[0], Despair);
		DUMP_ITEM(%s, c->sample_filenames[0], Death);
		DUMP_ITEM(%s, c->sample_filenames[0], Victory);

		DUMP_ITEM(%hhd, c->passive_aggressive, Passive Agressive);

		DUMP_ITEM(%hhd, c->bravery_cowardice, Bravery Cowardice);
		DUMP_ITEM(%hhd, c->aerial_ground, Aerial Ground);
		DUMP_ITEM(%hhd, c->obedience_disobedience, Obedience Disobedience);
		DUMP_ITEM(%hhd, c->pylon_grab, Pylon Grab);
		DUMP_ITEM(%hhd, c->radar_kill, Radar Kill);
		DUMP_ITEM(%hhd, c->radar_protect, Radar Protect);
		DUMP_ITEM(%hhd, c->skill_level, Skill Level);
		DUMP_ITEM(%hhd, c->preferred_vehicle, Preferred Vehicle);

		fprintf(file_out, "\n");
		
		c++;
	}
}
