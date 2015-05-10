//   2/24/15 commit

#ifndef _ENTITY_
#define _ENTITY_

#define maxEnts		1024

//create entity structure with common variables for: inuse, positioning
typedef struct ENTITY_T
{
	int inuse;
	double x;
	double y;
	double xVel;
	double yVel;
	int dir;
	int health;
	int type;
	int colliding;
	double size;
	int order;

	Sprite *sprite;

	void (*think)(struct ENTITY_T *thisEnt);
} Entity;



//list all entities in game
Entity entList[maxEnts];
int numEnts;

//list function used in entity.c
Entity *initEnt(void);
void Free_Ent(Entity *thisEnt);
void freeAllEnts();
Entity *Spawn_Ent(double spawnX, double spawnY, double xVel, double yVel, int dir, Sprite *sprite, int health, int type, int ord);

//spawn specific entity types
void spBloon(int type);
void spBullet(double towerX, double towerY, double xVel, double yVel, int type, int towerNum);
void spTower(double towerX, double towerY, int dir, int type);
void spDumb();

//entity think functions
void towerThink(Entity *thatEnt);
void bulletThink(Entity *thatEnt);
void bloonThink(Entity *thatEnt);
void dumbThink(Entity *thatEnt);

//wave stuff
void startWave(int SpawnRate, int lvl2mix, int lvl3mix, int lvl4mix, int numBloons);
int spr;
int l2m;
int l3m;
int l4m;
int nbs;
int bloonsSpawned;
int waitTime;
int waveInProg;
int readyForWave;

#endif
