#include <stdio.h>
#include <math.h>
#include "SDL/SDL.h"

#include "yekgfx.h"

#define WIDTH 600
#define HEIGHT 600

#define RAUP 10000
#define UP   20000
#define RADOWN 10000

#define VMFLAGS SDL_SWSURFACE|SDL_DOUBLEBUF//|SDL_FULLSCREEN


int main(int argc, char *argv[])
{
 SDL_Surface *screen;
 int quit = 0;
 int x, y;
 float speed,size;
 /* palette */
 SDL_Color myPalette[256];
 int cosTable[256];
 int t0,t,told;
 float indexfactor;
 int colorx[WIDTH],colory[HEIGHT];
  /* factors for cosine function */
 float coeffs1[8];
 float coeffs2[8];
 float pos1x,pos1y,delta1x,delta1y;
 float pos2x,pos2y,delta2x,delta2y;
 float scale;    

 srand(0);
    
 for (t=0;t<256;t++)
  cosTable[t]=(int)(32.0+32.0*cos(t*2.0*3.141592/256.0));

 pos1x=WIDTH/4*rand()/(float)RAND_MAX;
 pos1y=HEIGHT/4*rand()/(float)RAND_MAX;
 delta1x=(rand()/(float)RAND_MAX-0.5);
 delta1y=(rand()/(float)RAND_MAX-0.5);
 pos2x=WIDTH/4*rand()/(float)RAND_MAX+3*WIDTH/4-1;
 pos2y=HEIGHT/4*rand()/(float)RAND_MAX+3*HEIGHT/4-1;
 delta2x=(rand()/(float)RAND_MAX-0.5);
 delta2y=(rand()/(float)RAND_MAX-0.5);
 scale=0.05;
 delta1x=delta1x*scale/(delta1x*delta1x+delta1y*delta1y);
 delta1y=delta1y*scale/(delta1x*delta1x+delta1y*delta1y);
 delta2x=delta2x*scale/(delta2x*delta2x+delta2y*delta2y);
 delta2y=delta2y*scale/(delta2x*delta2x+delta2y*delta2y);

 sinpalette(myPalette,1,1,1,128,64,32);
 for (t=0;t<64;t++)
 {
  myPalette[t].r=0;
  myPalette[t].g=0;
  myPalette[t].b=t;

  myPalette[t+64].r=t;
  myPalette[t+64].g=0;
  myPalette[t+64].b=63-t;

  myPalette[t+128].r=63-t;
  myPalette[t+128].g=t;
  myPalette[t+128].b=0;
  
  myPalette[t+192].r=0;
  myPalette[t+192].g=63-t;
  myPalette[t+192].b=0;
 }     


    for (t=0;t<8;t++)
    {   
        if (t%2==0)
            scale=2.5;  /*size*/
        else
            scale=0.4; /*speed*/
        coeffs1[t]=scale*((float)rand()/RAND_MAX-0.5);
        coeffs2[t]=scale*((float)rand()/RAND_MAX-0.5);
    }
    t0=SDL_GetTicks();


 /* Initialize defaults, Video and Audio */
 if((SDL_Init(SDL_INIT_VIDEO)==-1))
 {
  printf("Could not initialize SDL: %s.\n", SDL_GetError());
  return -1;
 }

 screen = SDL_SetVideoMode(WIDTH, HEIGHT, 8, VMFLAGS);
 if ( screen == NULL )
 {
  fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
  return -2;
 }
 SDL_ShowCursor(SDL_DISABLE);

 
  /* set palette */
  SDL_SetColors(screen,myPalette,0,256);
 t=0;
 while( !quit )//&& t<(RAUP+UP+RADOWN))
 {
  eventcheck(&quit);
  /* Lock the screen for direct access to the pixels */
  if ( SDL_MUSTLOCK(screen) )
  {
   if ( SDL_LockSurface(screen) < 0 )
   {
    fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
    return -3;
   }
  }
  told=t;
  t=SDL_GetTicks()-t0;
  indexfactor=1.0;//getindexfactor(t,RAUP,UP,RADOWN);
  /* the actual drawing */

  moveval(&pos1x,&delta1x,t-told,0,WIDTH-1);
  moveval(&pos1y,&delta1y,t-told,0,HEIGHT-1);
  moveval(&pos2x,&delta2x,t-told,0,WIDTH-1);
  moveval(&pos2y,&delta2y,t-told,0,HEIGHT-1);
  
  plasma8_1(screen,0,0,WIDTH-1,HEIGHT-1,t,coeffs1,indexfactor,cosTable,colorx,colory);
  plasma8_1(screen,round(pos1x>pos2x?pos2x:pos1x),round(pos1y>pos2y?pos2y:pos1y),
    round(pos1x>pos2x?pos1x:pos2x),round(pos1y>pos2y?pos1y:pos2y),
    t,coeffs2,indexfactor,cosTable,colorx,colory);
  
 // printf("fps=%f\n",1000.0/(t-told));
  
  /* Unlock Surface if necessary */
  if ( SDL_MUSTLOCK(screen) )
  {
   SDL_UnlockSurface(screen);
  }

  SDL_Flip(screen);
 }

 SDL_Quit();
 
 return 0;
}
