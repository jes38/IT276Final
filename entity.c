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
extern int CANPLACE;
extern int BORD;
extern int TORD;
extern int MOUSEX;
extern int MOUSEY;
extern struct transform BloonArray[];

int FIRE;

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

Entity *Spawn_Ent(double spawnX, double spawnY, double xVel, double yVel, int dir, Sprite *sprite, int health, int type, int ord)  //spawn new entity at a location
{
	Entity *entPointer = initEnt();
	double offX;
	double offY;

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
	else if (type == 50 && health > 5){entPointer -> size = 16;}
	else {entPointer -> size = 8;}
	entPointer -> think = NULL;
	offX = spawnX - entPointer->size;
	offY = spawnY - entPointer->size;

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

void spBloon(int type) //spawn a bloon of type 1 - 5
{
	Entity *bloon;

	Sprite *bloonSprite;
	
	if (type == 1)
	{
		BORD += 1;
		bloonSprite = LoadSprite("images/rbloon.png",16,16);
		bloon = Spawn_Ent(BloonArray[0].x, 0, 0, 1, 0, bloonSprite, 1, 50, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 2)
	{
		BORD += 1;
		bloonSprite = LoadSprite("images/bbloon.png",16,16);
		bloon = Spawn_Ent(BloonArray[0].x, 0, 0, 1, 0, bloonSprite, 2, 50, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 3)
	{
		BORD += 1;
		bloonSprite = LoadSprite("images/gbloon.png",16,16);
		bloon = Spawn_Ent(BloonArray[0].x, 0, 0, 1, 0, bloonSprite, 3, 50, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 4)
	{
		BORD += 1;
		bloonSprite = LoadSprite("images/shieldBloon.png",16,16);
		bloon = Spawn_Ent(BloonArray[0].x, 0, 0, 1, 0, bloonSprite, 1, 51, BORD);
		bloon->think = bloonThink;
	}
	else if (type == 5)
	{
		BORD += 1;
		bloonSprite = LoadSprite("images/boss.png",32,32);
		bloon = Spawn_Ent(BloonArray[0].x, 0, 0, 1, 0, bloonSprite, 15, 50, BORD);
		bloon->think = bloonThink;
	}
	else
	{
		fprintf(stdout, "invalid bloon type\n");
	}

}

void spBullet(double towerX, double towerY, double xVel, double yVel, int type, int towerNum) //spawn a bullet at tower's location
{
	Entity *bullet;
	Sprite *bSprite = LoadSprite("images/bullet.png",16,16);
	
	//add velocity to base direction
	if (type == 1){
		xVel *= 2;
		yVel *= 2;
	}
	if (type == 2){
		xVel *= 4;
		yVel *= 4;
	}
	bullet = Spawn_Ent(towerX, towerY, xVel, yVel, 0, bSprite, 100, 40, towerNum);
	bullet->think = bulletThink;
}

//spawn a tower of type 1, 2, 3, 4, or 5
void spTower(double towerX, double towerY, int dir, int type)
{
	int cost;
	if(type == 1){cost = 20;}
	if(type == 3){cost = 30;}
	if(type == 5){cost = 30;}
	if(type == 7){cost = 50;}
	if(type == 9){cost = 100;}
	
	if(ECON >= cost){	
		Entity *tower;
		Sprite *tSprite;

		if(type == 1){
			tSprite = LoadSprite("images/tower1a.png",32,32);
		}
		else if(type == 3){
			tSprite = LoadSprite("images/tower2a.png",32,32);
		}
		else if(type == 5){
			tSprite = LoadSprite("images/tower3a.png",32,32);
		}
		else if(type == 7){
			tSprite = LoadSprite("images/tower4a.png",32,32);
		}
		else if(type == 9){
			tSprite = LoadSprite("images/tower5.png",32,32);
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

void towerThink(Entity *thatEnt) //fire bullets and target enemies
{
	double targXVel;
	double targYVel;
	
	int targ = 1500;
	int q = 0;

	double towerX = thatEnt -> x;
	double towerY = thatEnt -> y;
	int dir = thatEnt -> dir;
	int type = thatEnt -> type;
	int towerNum = thatEnt -> order;
	
	//upgrade tower if health (xp) is greater than 10
	if (type == 1 && thatEnt->health == 10){
		Sprite *tSprite;
		tSprite = LoadSprite("images/tower1b.png",32,32);
		thatEnt->type = 2;
		thatEnt->sprite = tSprite;
		thatEnt->health = 11;
	}
	if (type == 3 && thatEnt->health == 10){
		Sprite *tSprite;
		tSprite = LoadSprite("images/tower2b.png",32,32);
		thatEnt->type = 4;
		thatEnt->sprite = tSprite;
		thatEnt->health = 11;
	}
	if (type == 5 && thatEnt->health == 10){
		Sprite *tSprite;
		tSprite = LoadSprite("images/tower3b.png",32,32);
		thatEnt->type = 6;
		thatEnt->sprite = tSprite;
		thatEnt->health = 11;
	}
	if (type == 7 && thatEnt->health == 10){
		Sprite *tSprite;
		tSprite = LoadSprite("images/tower4b.png",32,32);
		thatEnt->type = 8;
		thatEnt->sprite = tSprite;
		thatEnt->health = 11;
	}
	
	//////////////
	//Targeting AI
	//////////////
	if (thatEnt->type != 9){
		while (q < maxEnts)
		{
			if(entList[q].inuse==1 && (entList[q].type==50 || entList[q].type==51)) //if the enemy is a bloon
			{
				Entity *enemy;
				double Xdist;
				double Ydist;
				double tempDist;
				enemy = &entList[q];

				Xdist = (thatEnt->x) - (enemy->x);
				Ydist = (thatEnt->y) - (enemy->y);

				tempDist = (Xdist * Xdist) + (Ydist * Ydist);
				if (tempDist <= 16384 && enemy->order < targ){ //if a bloon is in range
					if (Xdist < 0){targXVel = (Xdist * Xdist) / tempDist;}
					else {targXVel = (Xdist * Xdist) / tempDist * -1;}

					if (Ydist < 0){targYVel = (Ydist * Ydist) / tempDist;}
					else {targYVel = (Ydist * Ydist) / tempDist * -1;}
				
					targ = enemy->order;
					FIRE = 1;
				}
			}
			q++;
		}

		if (FIRE == 1){    //BRANDON FOUND THIS: had to add this or tower would fire all the time
			fireBull(towerX, towerY, targXVel, targYVel, type, towerNum);
			FIRE = 0;
		}
	}
	else if (thatEnt->type == 9) {  //player controlled tower doesn't need AI
		double Xdist;
		double Ydist;
		double tempDist;

		Xdist = (thatEnt->x) - MOUSEX;
		Ydist = (thatEnt->y) - MOUSEY;

		tempDist = (Xdist * Xdist) + (Ydist * Ydist);
		
		if (Xdist < 0){targXVel = (Xdist * Xdist) / tempDist;}
		else {targXVel = (Xdist * Xdist) / tempDist * -1;}

		if (Ydist < 0){targYVel = (Ydist * Ydist) / tempDist;}
		else {targYVel = (Ydist * Ydist) / tempDist * -1;}
				
		fireBull(towerX, towerY, targXVel, targYVel, type, towerNum);
		
	}
}

//spawn bullets based on tower type
void fireBull(double towerX, double towerY, double targXVel, double targYVel, int type, int towerNum)
{
	if (type == 1){
		if( (TIME/30) * 30 == TIME){spBullet(towerX, towerY, targXVel, targYVel, 1, towerNum);}
	}
	else if (type == 2){
		if( (TIME/30) * 30 == TIME){spBullet(towerX, towerY, targXVel, targYVel, 2, towerNum);}
	}
	else if (type == 3){
		if( (TIME/20) * 20 == TIME){spBullet(towerX, towerY, targXVel, targYVel, 1, towerNum);}
	}
	else if (type == 4){
		if( (TIME/20) * 20 == TIME){spBullet(towerX, towerY, targXVel, targYVel, 2, towerNum);}
	}
	else if (type == 5){
		if( (TIME/30) * 30 == TIME){
			spBullet(towerX, towerY, 0, -1, 1, towerNum);
			spBullet(towerX, towerY, 0, 1, 1, towerNum);
			spBullet(towerX, towerY, 1, 0, 1, towerNum);
			spBullet(towerX, towerY, -1, 0, 1, towerNum);
		}
	}
	else if (type == 6){
		if( (TIME/30) * 30 == TIME){
			spBullet(towerX, towerY, 0, -1, 1, towerNum);
			spBullet(towerX, towerY, 0, 1, 1, towerNum);
			spBullet(towerX, towerY, 1, 0, 1, towerNum);
			spBullet(towerX, towerY, -1, 0, 1, towerNum);

			spBullet(towerX, towerY, 0, 0.707, 1, towerNum);
			spBullet(towerX, towerY, 0, -0.707, 1, towerNum);
			spBullet(towerX, towerY, 0.707, 0, 1, towerNum);
			spBullet(towerX, towerY, -0.707, 0, 1, towerNum);
		}
	}
	else if (type == 7){
		if( (TIME/30) * 30 == TIME){
			int randNum1 = rand()%10;
			int randNum2 = rand()%10;
			randNum1 - 5;
			randNum2 - 5;

			spBullet(towerX + randNum1, towerY + randNum2, 0, 0, 1, towerNum);
		}
	}
	else if (type == 8){
		if( (TIME/20) * 20 == TIME){
			int randNum1 = rand()%10;
			int randNum2 = rand()%10;
			randNum1 - 5;
			randNum2 - 5;

			spBullet(towerX + randNum1, towerY + randNum2, 0, 0, 1, towerNum);
		}
	}
	else if (type == 9){
		if( (TIME/20) * 20 == TIME){spBullet(towerX, towerY, targXVel, targYVel, 2, towerNum);}
	}
}

void bloonThink(Entity *thatEnt) //pathfinding
{
	double bloonX = thatEnt -> x;
	double bloonY = thatEnt -> y;

	int cnt = 0;
	while(cnt <= 9) {
		if(bloonX == BloonArray[cnt].x && bloonY == BloonArray[cnt].y ) {
			thatEnt->xVel = BloonArray[cnt].xVel;
			thatEnt->yVel = BloonArray[cnt].yVel;
		}
		cnt++;
	}
}

//bullets lose health over time and die after a few seconds (if they don't hit anything)
void bulletThink(Entity *thatEnt){
	thatEnt->health -= 1;
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