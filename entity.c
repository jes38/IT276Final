//   2/24/15 commit

#include <string.h>
#include <stdlib.h>

#include "graphics.h"
#include "sprite.h"
#include "entity.h"

extern SDL_Surface *screen;
extern int ECON;
extern int LIVES;
extern int LEVEL;
extern int ROTATION;
extern int MOUSEX;
extern int MOUSEY;
extern int CANPLACE;
extern int BloonArray[];

extern SDL_Surface *message;
extern TTF_Font *font;
extern SDL_Color textColor;

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

void Move_Ent(Entity *thisEnt, int xAmnt, int yAmnt)  //move a specific entity by a certain ammount
{
	int offX;
	int offY;

	thisEnt -> x += xAmnt;
	thisEnt -> y += yAmnt;

	offX = thisEnt->x - thisEnt->size;
	offY = thisEnt->y - thisEnt->size;
	if(thisEnt->sprite != NULL) DrawSprite(thisEnt->sprite, screen, offX, offY, 0);
}

Entity *Spawn_Ent(int spawnX, int spawnY, int xVel, int yVel, int dir, Sprite *sprite, int health, int type)  //spawn new entity at a location
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
	if (type==1 || type==2 || type ==3 || type==4){entPointer -> size = 16;}
	else {entPointer -> size = 8;}
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
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite1, 1, 50);
		bloon->think = bloonThink;
	}
	else if (type == 2)
	{
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite2, 2, 50);
		bloon->think = bloonThink;
	}
	else if (type == 3)
	{
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite2, 3, 50);
		bloon->think = bloonThink;
	}
	else if (type == 4)
	{
		bloon = Spawn_Ent(715, 0, 0, 1, 0, bloonSprite2, 4, 50);
		bloon->think = bloonThink;
	}
	else
	{
		fprintf(stdout, "invalid bloon type\n");
	}

	
}

void spBullet(int towerX, int towerY, int dir, int type) //spawn a bullet at tower's location
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
	bullet = Spawn_Ent(towerX, towerY, xVel, yVel, dir, bSprite, 1, 40);
}

//spawn dummy bullet once for UI
void spDumb()
{
	Entity *dumbBullet;
	Sprite *dumbSprite = LoadSprite("images/bullet.png",16,16);
	dumbBullet = Spawn_Ent(958, 22, 0, 0, 0, dumbSprite, -1, 100);

	dumbBullet->think = dumbThink;
}


//spawn a tower of type 1, 2, 3, or 4
void spTower(int towerX, int towerY, int dir, int type)
{
	int cost;
	if(type == 1){cost = 20;}
	if(type == 2){cost = 30;}
	if(type == 3){cost = 30;}
	if(type == 4){cost = 40;}
	
	if(ECON >= cost){	
		Entity *tower;
		Sprite *tSprite = LoadSprite("images/tower.png",32,32);
		dir = ROTATION;

		tower = Spawn_Ent(towerX, towerY, 0, 0, dir, tSprite, -1, type);
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
	
	if (type == 1){
		if( (TIME/30) * 30 == TIME){spBullet(towerX, towerY, dir, 1);}
	}
	else if (type == 2){
		if( (TIME/20) * 20 == TIME){spBullet(towerX, towerY, dir, 1);}
	}
	else if (type == 3){
		if( (TIME/30) * 30 == TIME){spBullet(towerX, towerY, dir, 2);}
	}
	else if (type == 4){
		if( (TIME/20) * 20 == TIME){spBullet(towerX, towerY, dir, 2);}
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
		LIVES--;
		Free_Ent(thatEnt);
	}
}

//start a wave based on LEVEL variable
void startWave(int SpawnRate/*a bigger number is a slower rate, recoomended 15*/, int lvl2mix, int lvl3mix, int lvl4mix, int numBloons)  
{
	spr = SpawnRate;
	l2m = lvl2mix - 1; //for a 0 chance, the variable must be -1
	l3m = lvl3mix - 1;
	l4m = lvl4mix - 1;
	nbs = numBloons;
	bloonsSpawned = 0;
	waitTime = SpawnRate + (rand()%4) - 2;
	
	waveInProg = 1;
}

void DrawUI() //self explanatory
{
	char lvl[40];
	char econ[45];
	char lives[10];
	SDL_Rect R;

	sprintf(lvl, "Level: %d", LEVEL);
	sprintf(econ, "Resources: %d", ECON);
	sprintf(lives, "Lives: %d", LIVES);

	
	R.x = 0;
	R.y = 0;
	R.w = 0;
	R.h = 0;

	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, lvl, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);
	
	R.y = 30;
	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, econ, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);
	
	R.y = 60;
	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, lives, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);
}

//main update function
void update()
{
	int i = 0;
	while (i < maxEnts)
	{
		//do think function
		if(entList[i].inuse == 1 && entList[i].think != NULL)
		{
			(*entList[i].think)(&entList[i]);
		}

		//scrolls through entities in use
		if(entList[i].inuse == 1)
		{
			Entity *tempEnt;
			int X;
			int Y;

			tempEnt = &entList[i];
			X = tempEnt -> x;
			Y = tempEnt -> y;

			//detect if bullet collides with bloon
			if (tempEnt->type == 40) //if the entity is a bullet
			{
				int q = 0;
				while (q < maxEnts)
				{
					if(entList[q].inuse==1 && entList[q].type==50) //if the enemy is a bloon
					{
						Entity *enemy;
						int Xdist;
						int Ydist;
						int tempDist;
						enemy = &entList[q];

						Xdist = (tempEnt->x) - (enemy->x);
						Ydist = (tempEnt->y) - (enemy->y);
						if (Xdist < 0){Xdist = Xdist * -1;}
						if (Ydist < 0){Ydist = Ydist * -1;}

						tempDist = (Xdist * Xdist) + (Ydist * Ydist);
						if (tempDist <= 256){   //if colliding
							tempEnt->health -= 1;
							enemy->health -= 1;
							ECON++;

							if(tempEnt->health == 0){Free_Ent(tempEnt);}
							if(enemy->health == 0){Free_Ent(enemy);}
						}
					}
					q++;
				}
			}

			//can't place towers on top of one another
			if (tempEnt->type <= 4) //if the entity is a tower
			{
				int Xdist;
				int Ydist;
				int tempDist;

				Xdist = (tempEnt->x) - MOUSEX;
				Ydist = (tempEnt->y) - MOUSEY;
				if (Xdist < 0){Xdist = Xdist * -1;}
				if (Ydist < 0){Ydist = Ydist * -1;}

				tempDist = (Xdist * Xdist) + (Ydist * Ydist);
				if (tempDist <= 1024){   //if colliding
					CANPLACE = 0;
				}
			}

			Move_Ent(tempEnt, tempEnt->xVel, tempEnt->yVel); //moves all entities

			//out of bounds detection
			if(X < -35 || X > 1059) Free_Ent(tempEnt);
			if(Y < -35 || Y > 803)	Free_Ent(tempEnt);
		}
		i++;
	}

	TIME++; //level time

	//controls waves
	if (waveInProg == 1)
	{
		if (bloonsSpawned < nbs)
		{
			if( (TIME/waitTime) * waitTime == TIME)
			{
				int randNum1 = rand()%10;
				int randNum2 = rand()%10;
				int randNum3 = rand()%10;
			
				if (randNum1 <= l4m){spBloon(4);}
				else if (randNum2 <= l3m){spBloon(3);}
				else if (randNum3 <= l2m){spBloon(2);}
				else{spBloon(1);}

				bloonsSpawned++;
				waitTime = spr + (rand()%4) - 2;
			}
		}
		else{waveInProg = 0; LEVEL++;}
	}
	
}
