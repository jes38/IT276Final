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
extern struct transform BloonArray[];
extern int PAUSED;
extern int EDITMODE;
extern int PLACE;

extern SDL_Surface *message;
extern TTF_Font *font;
extern SDL_Color textColor;

void Move_Ent(Entity *thisEnt, double xAmnt, double yAmnt)  //move a specific entity by a certain ammount
{
	thisEnt -> x += xAmnt;
	thisEnt -> y += yAmnt;
}

void Draw_All()
{
	int i;
	for(i =0; i < maxEnts; i++){
		if(entList[i].inuse){
			Draw_Ent(&entList[i]);
		}
	}
}

void Draw_Ent (Entity *thatEnt){
	if(thatEnt->sprite != NULL) 
		DrawSprite(thatEnt->sprite, screen, 
			thatEnt->x - thatEnt->size, 
			thatEnt->y - thatEnt->size, 
		0);
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

//used for leveling a tower
void LevelTower (int towerNum){
	int i = 0;
	while (i < maxEnts)
	{
		if(entList[i].inuse == 1 && entList[i].type >= 1 && entList[i].type <= 10 && entList[i].order == towerNum)
		{
			Entity *tempEnt;
			tempEnt = &entList[i];

			tempEnt->health += 1;
		}
		i++;
	}
}

void editMode() {
	if(EDITMODE%2 == 1 && PLACE == 1){
 		int tempPos = EDITMODE - 1;

		BloonArray[tempPos].y = MOUSEY;

		if(EDITMODE == 1){BloonArray[tempPos].x = MOUSEX;}
		else {BloonArray[tempPos].x = BloonArray[tempPos - 1].x;}

		if(EDITMODE != 1){
			if(BloonArray[tempPos].y > BloonArray[tempPos - 1].y){
				BloonArray[tempPos - 1].yVel = 1;
			}
			else {
				BloonArray[tempPos - 1].yVel = -1;
			}
		}

		BloonArray[tempPos].yVel = 0;

		EDITMODE += 1;
	}
	else if (EDITMODE%2 == 0 && PLACE == 1){
		int tempPos = EDITMODE - 1;

		BloonArray[tempPos].x = MOUSEX;
		
		BloonArray[tempPos].y = BloonArray[tempPos - 1].y;

		BloonArray[tempPos].xVel = 0;

		if(BloonArray[tempPos].x > BloonArray[tempPos - 1].x){
			BloonArray[tempPos - 1].xVel = 1;
		}
		else {BloonArray[tempPos - 1].xVel = -1;}

		EDITMODE += 1;
	}

	PLACE = 0;
}

//main update function
void update()
{
	int i = 0;
	readyForWave = 1;

	if(EDITMODE > 0){editMode();}

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
			double X;
			double Y;

			tempEnt = &entList[i];
			X = tempEnt -> x;
			Y = tempEnt -> y;

			if (tempEnt->type == 50){readyForWave = 0;}

			//////////////////////////////////////
			//detect if bullet collides with bloon
			//////////////////////////////////////
			if (tempEnt->type == 40 && tempEnt->health == 0){Free_Ent(tempEnt);} //first check to see if it's alive.
			if (tempEnt->type == 40) //if the entity is a bullet
			{
				int q = 0;
				while (q < maxEnts)
				{
					if(entList[q].inuse==1 && entList[q].type==50) //if the enemy is a bloon
					{
						Entity *enemy;
						double Xdist;
						double Ydist;
						double tempDist;
						enemy = &entList[q];

						Xdist = (tempEnt->x) - (enemy->x);
						Ydist = (tempEnt->y) - (enemy->y);
						if (Xdist < 0){Xdist = Xdist * -1;}
						if (Ydist < 0){Ydist = Ydist * -1;}

						tempDist = (Xdist * Xdist) + (Ydist * Ydist);
						if (tempDist <= 64){   //if colliding (is it 64 or 256?)
							tempEnt->health = 0;
							enemy->health -= 1;
							ECON++;

							if(tempEnt->health == 0){
								LevelTower(tempEnt->order);
								Free_Ent(tempEnt);
							}
							if(enemy->health == 0){
								Free_Ent(enemy);
							}
						}
					}
					q++;
				}
			}

			//can't place towers on top of one another
			if (tempEnt->type <= 4) //if the entity is a tower
			{
				double Xdist;
				double Ydist;
				double tempDist;

				Xdist = (tempEnt->x) - MOUSEX;
				Ydist = (tempEnt->y) - MOUSEY;
				if (Xdist < 0){Xdist = Xdist * -1;}
				if (Ydist < 0){Ydist = Ydist * -1;}

				tempDist = (Xdist * Xdist) + (Ydist * Ydist);
				if (tempDist <= 1024){   //if colliding
					CANPLACE = 0;
				}
			}

			if(PAUSED == 0){Move_Ent(tempEnt, tempEnt->xVel, tempEnt->yVel);}//moves all entities
			else{Move_Ent(tempEnt, 0, 0);}

			//out of bounds detection
			if(X < -35 || X > 1059){
				if(tempEnt->type == 50){LIVES--;}
				Free_Ent(tempEnt);
			}
			if(Y < -35 || Y > 803){
				if(tempEnt->type == 50){LIVES--;}
				Free_Ent(tempEnt);
			}
		}
		i++;
	}

	if(PAUSED == 0){TIME++;} //level time

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
