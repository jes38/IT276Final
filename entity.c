//   2/24/15 commit

#include <string.h>
#include <stdlib.h>

#include "graphics.h"
#include "sprite.h"
#include "entity.h"
#include "update.h"

extern SDL_Surface *screen;
extern int ECON;
extern int LIVES;
extern int LEVEL;
extern int ROTATION;
extern int MOUSEX;
extern int MOUSEY;
extern int CANPLACE;
extern int BORD;
extern int TORD;
extern int BloonArray[];

Entity *initEnt(void)  //place entity in entList
{
	int i = 0;
	Entity *entPointer;

	while (i < maxEnts)
	{
		if(entList[i].inuse == 0){
			entPointer = &entList[i];
			entPointer -> inuse = 1;
			numEnts++;
			i = maxEnts;
		}
		else if (i == maxEnts){
			exit(1);
		}
		i++;
	}
	return entPointer;
}

Entity *Spawn_Ent(int spawnX, int spawnY, int xVel, int yVel, int dir, Sprite *sprite, int health, int type, int ord)  //spawn new entity at a location
{
	Entity *entPointer = initEnt();
	int offX;
	int offY;

	entPointer -> x = spawnX;
	entPointer -> y = spawnY;
	entPointer -> xVel = xVel;
	entPointer -> yVel = yVel;
	entPointer -> dir = dir;
	entPointer -> sprite = sprite;
	entPointer -> health = health;
	entPointer -> type = type;
	entPointer -> colliding = 0;
	entPointer -> order = ord;
	if (type >= 1 && type <= 10){entPointer -> size = 16;}
	else {entPointer -> size = 8;}
	entPointer -> think = NULL;
	offX = spawnX - entPointer->size;
	offY = spawnY - entPointer->size;

	//point in dir direction
	DrawSprite(entPointer->sprite, screen, offX, offY, 0);
	return entPointer;
}

void Free_Ent(Entity *thisEnt)  //clear entity when no longer used
{
	thisEnt -> inuse = 0;
	FreeSprite(thisEnt->sprite);
}

void freeAllEnts()  //free all entities
{
	int i = 0;
	while (i < maxEnts)
	{
		if(entList[i].inuse == 1)
		{
			Free_Ent(&entList[i]);
		}
		i++;
	}
}

//
/* Spawn specific entities */
//

void spBloon(int type) //spawn a bloon of type 1 or 2
{
	Entity *bloon;

	Sprite *bloonSprite1 = LoadSprite("images/bbloon.png",16,16);
	Sprite *bloonSprite2 = LoadSprite("images/rbloon.png",16,16);
	
	if (type == 1)
	{
		BORD += 1;
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite1, 1, 50, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 2)
	{
		BORD += 1;
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite2, 2, 50, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 3)
	{
		BORD += 1;
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite2, 3, 50, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 4)
	{
		BORD += 1;
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite2, 4, 50, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 5)
	{
		BORD += 1;
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite2, 10, 50, BORD);
		bloon->think = bloonThink;
	}
	else
	{
		fprintf(stdout, "invalid bloon type\n");
	}

	
}

void spBullet(int towerX, int towerY, int dir, int type, int towerNum) //spawn a bullet at tower's location
{
	Entity *bullet;
	Sprite *bSprite = LoadSprite("images/bullet.png",16,16);
	
	//hard code x and y velocity based on dir
	int xVel;
	int yVel;
	
	if (dir == 0){xVel = 0; yVel = -2;}
	else if (dir == 45){xVel = 1; yVel = -1;}
	else if (dir == 90){xVel = 2; yVel = 0;}
	else if (dir == 135){xVel = 1; yVel = 1;}
	else if (dir == 180){xVel = 0; yVel = 2;}
	else if (dir == 225){xVel = -1; yVel = 1;}
	else if (dir == 270){xVel = -2; yVel = 0;}
	else if (dir == 315){xVel = -1; yVel = -1;}
	
	//add velocity if using bullet type 2
	if (type == 2){
		xVel *= 2;
		yVel *= 2;
	}
	bullet = Spawn_Ent(towerX, towerY, xVel, yVel, dir, bSprite, 1, 40, towerNum);
}

//spawn dummy bullet once for UI
void spDumb()
{
	Entity *dumbBullet;
	Sprite *dumbSprite = LoadSprite("images/bullet.png",16,16);
	dumbBullet = Spawn_Ent(958, 22, 0, 0, 0, dumbSprite, -1, 100, 0);

	dumbBullet->think = dumbThink;
}


//spawn a tower of type 1, 2, 3, or 4
void spTower(int towerX, int towerY, int dir, int type)
{
	int cost;
	if(type == 1){cost = 20;}
	if(type == 3){cost = 30;}
	if(type == 5){cost = 60;}
	
	if(ECON >= cost){	
		Entity *tower;
		Sprite *tSprite;

		if(type == 1){
			tSprite = LoadSprite("images/tower.png",32,32);
		}
		else if(type == 3){
			tSprite = LoadSprite("images/tower.png",32,32);
		}
		else if(type == 5){
			tSprite = LoadSprite("images/tower.png",32,32);
		}
		else {tSprite = LoadSprite("images/tower.png",32,32);}

		dir = ROTATION;
		TORD -= 1;
		tower = Spawn_Ent(towerX, towerY, 0, 0, dir, tSprite, 0, type, TORD);
		tower->think = towerThink;

		ECON = ECON - cost;
	}
}

//
/*Entity Think functions*/
//

void towerThink(Entity *thatEnt) //fire bullets
{
	int towerX = thatEnt -> x;
	int towerY = thatEnt -> y;
	int dir = thatEnt -> dir;
	int type = thatEnt -> type;
	int towerNum = thatEnt -> order;
	
	//upgrade tower if health (xp) is greater than 10
	if (type == 1 && thatEnt->health >= 10){
		thatEnt->type = 2;
	}
	if (type == 3 && thatEnt->health >= 10){
		thatEnt->type = 4;
	}
	if (type == 5 && thatEnt->health >= 10){
		thatEnt->type = 6;
	}
	
	//spawn bullets based on tower type
	if (type == 1){
		if( (TIME/30) * 30 == TIME){spBullet(towerX, towerY, dir, 1, towerNum);}
	}
	else if (type == 2){
		if( (TIME/30) * 30 == TIME){spBullet(towerX, towerY, dir, 2, towerNum);}
	}
	else if (type == 3){
		if( (TIME/20) * 20 == TIME){spBullet(towerX, towerY, dir, 1, towerNum);}
	}
	else if (type == 4){
		if( (TIME/20) * 20 == TIME){spBullet(towerX, towerY, dir, 2, towerNum);}
	}
	else if (type == 5){
		if( (TIME/30) * 30 == TIME){
			spBullet(towerX, towerY, 0, 1, towerNum);
			spBullet(towerX, towerY, 90, 1, towerNum);
			spBullet(towerX, towerY, 180, 1, towerNum);
			spBullet(towerX, towerY, 270, 1, towerNum);
		}
	}
	else if (type == 6){
		if( (TIME/30) * 30 == TIME){
			spBullet(towerX, towerY, 0, 1, towerNum);
			spBullet(towerX, towerY, 90, 1, towerNum);
			spBullet(towerX, towerY, 180, 1, towerNum);
			spBullet(towerX, towerY, 270, 1, towerNum);

			spBullet(towerX, towerY, 45, 1, towerNum);
			spBullet(towerX, towerY, 135, 1, towerNum);
			spBullet(towerX, towerY, 225, 1, towerNum);
			spBullet(towerX, towerY, 315, 1, towerNum);
		}
	}
}

void dumbThink(Entity *thatEnt) //edit position based on ROTATION variable
{
	int dumbx;
	int dumby;
	
	if (ROTATION == 0){dumbx = 958; dumby = 22;}
	if (ROTATION == 45){dumbx = 974; dumby = 22;}
	if (ROTATION == 90){dumbx = 974; dumby = 38;}
	if (ROTATION == 135){dumbx = 974; dumby = 54;}
	if (ROTATION == 180){dumbx = 958; dumby = 54;}
	if (ROTATION == 225){dumbx = 942; dumby = 54;}
	if (ROTATION == 270){dumbx = 942; dumby = 38;}
	if (ROTATION == 315){dumbx = 942; dumby = 22;}

	thatEnt->x = dumbx;
	thatEnt->y = dumby;
}

void bloonThink(Entity *thatEnt) //pathfinding
{
	int bloonX = thatEnt -> x;
	int bloonY = thatEnt -> y;
	int cnt = 1;

	/*
	while(cnt <= 40) {
		if(bloonX == BloonArray[cnt] && bloonY == BloonArray[cnt + 1] ) {
			if(BloonArray[cnt + 2] < 1000){thatEnt->xVel = BloonArray[cnt + 2];}
			if(BloonArray[cnt + 3] < 1000){thatEnt->yVel = BloonArray[cnt + 3];}
			else{
				LIVES--;
				Free_Ent(thatEnt);
			}
		}
		cnt += 4;
	}
	*/

	//script path finding here
	if(bloonX == 715 && bloonY == 400 ) {
		thatEnt->xVel = -1;
		thatEnt->yVel = 0;
	}
	if(bloonX == 345 && bloonY == 400 ) {
		thatEnt->xVel = 0;
		thatEnt->yVel = 1;
	}
	if(bloonX == 345 && bloonY == 750 ) {
		LIVES -= thatEnt->health;
		Free_Ent(thatEnt);
	}
}

//start a wave based on LEVEL variable
void startWave(int SpawnRate/*a bigger number is a slower rate, recoomended 15*/, int lvl2mix, int lvl3mix, int lvl4mix, int numBloons)  
{
	BORD = 0;
	spr = SpawnRate;
	l2m = lvl2mix - 1; //for a 0 chance, the variable must be -1
	l3m = lvl3mix - 1;
	l4m = lvl4mix - 1;
	nbs = numBloons;
	bloonsSpawned = 0;
	waitTime = SpawnRate + (rand()%4) - 2;
	
	waveInProg = 1;
}
