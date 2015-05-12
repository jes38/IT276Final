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
extern int DELETE;

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
	int frame = 0;
	if(thatEnt->health <= 292 && thatEnt->health >= 250){frame = 1;}
	if(thatEnt->sprite != NULL) 
		DrawSprite(thatEnt->sprite, screen, 
			thatEnt->x - thatEnt->size, 
			thatEnt->y - thatEnt->size, 
		frame);
}

void DrawUI() //self explanatory
{
	char lvl[40];
	char econ[45];
	char lives[10];

	char cost1[3];
	char cost2[3];
	char cost3[3];
	char cost4[3];
	char cost5[3];

	SDL_Rect R;

	sprintf(lvl, "Level: %d", LEVEL);
	sprintf(econ, "Resources: %d", ECON);
	sprintf(lives, "Lives: %d", LIVES);

	sprintf(cost1, "20");
	sprintf(cost2, "30");
	sprintf(cost3, "30");
	sprintf(cost4, "50");
	sprintf(cost5, "100");
	
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

	R.x = 980;
	R.y = 12;
	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, cost1, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);
	
	R.y = 54;
	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, cost2, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);
	
	R.y = 96;
	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, cost3, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);

	R.y = 138;
	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, cost4, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);

	R.y = 180;
	if (message != NULL)
	{
		SDL_FreeSurface(message);
	}
	message = TTF_RenderText_Solid( font, cost5, textColor );
	SDL_BlitSurface(message, NULL, screen, &R);
}

//used for leveling a tower
void LevelTower (int towerNum){
	int i = 0;
	while (i < maxEnts)
	{
		if(entList[i].inuse == 1 && entList[i].type >= 1 && entList[i].type <= 10 && entList[i].order == towerNum && entList[i].health <= 10)
		{
			entList[i].health += 1;
		}
		i++;
	}
}

//launch editor mode. press e to enter and exit, press space to place turn.
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

//can't place towers on path
void pathCol()
{
	int i;
	for(i = 0; i < 10; i++)
	{
		if (i%2 == 1 && MOUSEY < (BloonArray[i].y + 32) && MOUSEY > (BloonArray[i].y - 32)){
			if (BloonArray[i - 1].xVel < 0){
				if (MOUSEX < BloonArray[i - 1].x && MOUSEX > BloonArray[i].x){
					CANPLACE = 0;
				}
			}
			else {
				if (MOUSEX > BloonArray[i - 1].x && MOUSEX < BloonArray[i].x){
					CANPLACE = 0;
				}
			}
		}
	
		if (i%2 == 0 && MOUSEX < (BloonArray[i].x + 32) && MOUSEX > (BloonArray[i].x - 32)){
			if (BloonArray[i - 1].yVel < 0){
				if (MOUSEY < BloonArray[i - 1].y && MOUSEY > BloonArray[i].y){
					CANPLACE = 0;
				}
			}
			else {
				if (MOUSEY > BloonArray[i - 1].y && MOUSEY < BloonArray[i].y){
					CANPLACE = 0;
				}
			}
		}
	}
}

//controls waves
void doWave ()
{
	if (waveInProg == 1)
	{
		if (bloonsSpawned < nbs)
		{
			if( (TIME/waitTime) * waitTime == TIME)
			{
				int randNum1 = rand()%10;
				int randNum2 = rand()%10;
				int randNum3 = rand()%10;
			
				if (l4m == -1 && l3m == -1 && l2m == -1){
					spBloon(5);
				}
				else if (randNum1 <= l4m){spBloon(4);}
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

//////////////////////////////////////
//detect if bullet collides with bloon
//////////////////////////////////////
void bulletToBloonCol (Entity *tempEnt)
{
			if (tempEnt->type == 40 && tempEnt->health == 0){Free_Ent(tempEnt);} //first check to see if it's alive.
			if (tempEnt->type == 40) //if the entity is a bullet
			{
				int q = 0;
				while (q < maxEnts)
				{
					if(entList[q].inuse==1 && (entList[q].type==50 || entList[q].type==51)) //if the enemy is a bloon
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
}

//can't place towers on top of one another
void towerToTowerCol (Entity *tempEnt)
{
	if (tempEnt->type <= 10) //if the entity is a tower
			{
				double Xdist;
				double Ydist;
				double tempDist;

				Xdist = (tempEnt->x) - MOUSEX;
				Ydist = (tempEnt->y) - MOUSEY;
				if (Xdist < 0){Xdist = Xdist * -1;}
				if (Ydist < 0){Ydist = Ydist * -1;}

				tempDist = (Xdist * Xdist) + (Ydist * Ydist);
				if (tempDist <= 1024){   //if other tower is colliding
					CANPLACE = 2;
				}
				if (tempDist <= 256 && DELETE){   //if mouse is colliding and d is being clicked
					ECON += 10;
					Free_Ent(tempEnt);
				}
			}
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

			if (tempEnt->type == 50 || tempEnt->type == 51){readyForWave = 0;}

			if (tempEnt->type == 51) //turn shield bloon's shields on and off
			{
				if( (TIME/30) * 30 == TIME && tempEnt->health == 292){
					tempEnt->health = 1;
				}
				else if((TIME/30) * 30 == TIME) {tempEnt->health = 292;}
			}

			bulletToBloonCol (tempEnt);  //BRANDON: suggested update function be less crowded.

			towerToTowerCol(tempEnt);

			if(PAUSED == 0){Move_Ent(tempEnt, tempEnt->xVel, tempEnt->yVel);}//moves all entities
			else{Move_Ent(tempEnt, 0, 0);}

			//out of bounds detection
			if(X < -35 || X > 1059){
				if(tempEnt->type == 50 || tempEnt->type == 51){LIVES--;}
				Free_Ent(tempEnt);
			}
			if(Y < -35 || Y > 803){
				if(tempEnt->type == 50 || tempEnt->type == 51){LIVES--;}
				Free_Ent(tempEnt);
			}
		}
		i++;
	}

	if(PAUSED == 0){TIME++;} //level time
	DELETE = 0;

	doWave();
}
