//   2/24/15 commit

#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "graphics.h"
#include "sprite.h"
#include "entity.h"

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect Camera;

void Init_All();

int getImagePathFromFile(char *filepath,char * filename);
int getCoordinatesFromFile(int *x, int *y,char * filename);
void addCoordinateToFile(char *filepath,int x, int y);
void getTurns(char * filename);

int ECON;
int LIVES;
int LEVEL;
int ROTATION;
int BloonArray[41];
int MOUSEX;
int MOUSEY;
int CANPLACE;

/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
int main(int argc, char *argv[])
{
  int doOnce = 0;
  
  SDL_Surface *temp = NULL;
  SDL_Surface *bg;
  Sprite *tile;

  Sprite *towerDumb;

  int done;
  int keyn;
  int i;
  int mx,my;
  int tx = 0,ty = 0;
  Uint8 *keys;
  char imagepath[512];
  Init_All();
  if (getImagePathFromFile(imagepath,"config.ini") == 0)
  {
    temp = IMG_Load(imagepath);/*notice that the path is part of the filename*/
  }
  if(temp != NULL)						/*ALWAYS check your pointers before you use them*/
    bg = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  if(bg != NULL)
    SDL_BlitSurface(bg,NULL,buffer,NULL);
  tile = LoadSprite("images/32_32_16_2sprite.png",32,32);
  getCoordinatesFromFile(&tx, &ty,"config.ini");
  fprintf(stdout,"x and y: (%i, %i)\n",tx,ty);

  //getTurns("config.ini");

  //addCoordinateToFile("config.ini",7,11);
  /*
  if(tile != NULL)
  {
        for(i = 0;i < 12;i++)
        {
            DrawSprite(tile,buffer,(i * tile->w) + tx,ty,0);
        }
  }
  */

  //dummy tower for UI
  towerDumb = LoadSprite("images/tower.png",32,32);
  DrawSprite(towerDumb,buffer,942,22,0);

  //spawn the dummy bullet
  spDumb();

  ECON = 30; //players resources
  LIVES = 50; //lives before game over
  LEVEL = 1; //what level the player is on
  ROTATION = 0; //rotation that next tower placed will have

  done = 0;
  do
  {
    ResetBuffer ();
	update();
    DrawMouse();
	DrawUI();
    NextFrame();
    SDL_PumpEvents();
    keys = SDL_GetKeyState(&keyn);

	if(doOnce == 0)
	{
		if(SDL_GetMouseState(&mx,&my))
		{
			
		}
		if(keys[SDLK_SPACE])
		{
			//start a wave
			int bloonNum;
			if(waveInProg == 0)
			{
				bloonNum = LEVEL * 20;
				startWave(15,3,0,0,bloonNum);
			}
			doOnce = 1;
		}

		//spawn a tower
		if(CANPLACE == 1){
			if(keys[SDLK_1]){spTower(mx,my,0,1); doOnce = 1;}
			if(keys[SDLK_2]){spTower(mx,my,0,2); doOnce = 1;}
			if(keys[SDLK_3]){spTower(mx,my,0,3); doOnce = 1;}
			if(keys[SDLK_4]){spTower(mx,my,0,4); doOnce = 1;}
		}
		CANPLACE = 1;

		//start a wave
		if(keys[SDLK_TAB])
		{
			ROTATION = ROTATION + 45;
			if (ROTATION == 360){ROTATION = 0;}
			doOnce = 1;
		}
	}
	if(keys[SDLK_1]==0 && keys[SDLK_2]==0 && keys[SDLK_3]==0 && keys[SDLK_4]==0 && keys[SDLK_SPACE]==0 && keys[SDLK_TAB]==0){doOnce = 0;}

	if(LIVES == 0)done = 1;
    if(keys[SDLK_ESCAPE])done = 1;
  }while(!done);
  exit(0);		/*technically this will end the program, but the compiler likes all functions that can return a value TO return a value*/
  return 0;
}

void CleanUpAll()
{
	freeAllEnts();
	CloseSprites();
  /*any other cleanup functions can be added here*/ 
}

void Init_All()
{
  Init_Graphics();

  InitMouse();
  atexit(CleanUpAll);
}

int getImagePathFromFile(char *filepath,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int returnValue = -1;
    if (!filepath)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    if (fscanf(fileptr,"%s",buf))
    {
        if (strcmp(buf,"image:")==0)
        {
            fscanf(fileptr,"%s",filepath);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    return returnValue;
}

void addCoordinateToFile(char *filepath,int x, int y)
{
    FILE *fileptr = NULL;
    if (!filepath)
    {
        fprintf(stdout,"addCoordinateToFile: warning, no input file path provided\n");
        return;
    }
    fileptr = fopen(filepath,"a");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filepath);
        return;
    }
    fprintf(fileptr,"newcoordinate: %i %i\n",x,y);
    fclose(fileptr);
}

int getCoordinatesFromFile(int *x, int *y,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int tx,ty;
    int returnValue = -1;
    if ((!x)&&(!y))
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    while (fscanf(fileptr,"%s",buf) != EOF)
    {
        fprintf(stdout,"buf is: %s\n",buf);
        if (strcmp(buf,"position:")==0)
        {
            fscanf(fileptr,"%i %i",&tx,&ty);
            fprintf(stdout,"as read: %i, %i\n",tx,ty);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    if (x)*x = tx;
    if (y)*y = ty;
    return returnValue;
}

void getTurns(char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];

    fileptr = fopen(filename,"r");

    while (fscanf(fileptr,"%s",buf) != EOF)
    {
        if (strcmp(buf,"turnCoord1:")==0)
        {
            fscanf(fileptr,"%i %i %i %i", BloonArray[1], BloonArray[2], BloonArray[3], BloonArray[4]);
        }
		if (strcmp(buf,"turnCoord2:")==0)
        {
            fscanf(fileptr,"%i %i %i %i", BloonArray[5], BloonArray[6], BloonArray[7], BloonArray[8]);
        }
		if (strcmp(buf,"turnCoord3:")==0)
        {
            fscanf(fileptr,"%i %i %i %i", BloonArray[9], BloonArray[10], BloonArray[11], BloonArray[12]);
        }
    }

    fclose(fileptr);
}

