//   2/24/15 commit

#ifndef _SPRITE_
#define _SPRITE_

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "graphics.h"

typedef struct Sprite_T
{
  SDL_Surface *image;		/*pointer to the actual image in memory*/
  char filename[20];			/*the name of the file, keep them short*/
  int w, h;					/*the width and height of the frames of the sprites, not the file itself*/
  int framesperline;			/*default is 16*/
  int color1,color2,color3;		/*if the file was palette swapped these are the colors that were changed*/
  int used;					/*used by the maintanenc functions to keep track of how many times a single sprite is being used*/
}Sprite;


void SwapSprite(SDL_Surface *sprite,int color1,int color2,int color3);
void InitSpriteList();
void FreeSprite(Sprite *img);
Sprite *LoadSprite(char *filename,int sizex, int sizey);		/*simplest file to load*/
Sprite *LoadSwappedSprite(char *filename,int sizex, int sizey, int c1, int c2, int c3);
/*drawsprite and drawspritepart work the same except that Drawsprite will render an entire sprite on the desired srface
  and drawspritepart can have the size of the sprite specified.  Know your sprite when using DSP since you can in theory
  read too far, not that SDL will allow that.*/
void DrawSprite(Sprite *sprite,SDL_Surface *surface,int sx,int sy, int frame);
void CloseSprites();		/*call this before you exit to make sure all the memory that your program used is given back to your OS*/


#endif
