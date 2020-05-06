#include <stdio.h>
#include <math.h>
#include "SDL/SDL.h"

#include "yekgfx.h"

#define WIDTH 640
#define HEIGHT 480

#define VMFLAGS SDL_SWSURFACE|SDL_DOUBLEBUF//|SDL_FULLSCREEN

int main(int argc, char *argv[])
{
 SDL_Surface *screen;
 int quit = 0;
 SDL_Event event;
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
 float posx,posy,deltax,deltay;
 float scale;    
 for (t=0;t<256;t++)
  cosTable[t]=(int)(32.0+32.0*cos(t*2.0*3.141592/256.0));
 posx=0;posy=0;deltax=0.1;deltay=0.2;
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

    srand(0);
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
 while( !quit /*&& t<30000*/)
 {
  /* Poll for events */
  while( SDL_PollEvent( &event ) )
  {
   switch( event.type )
   {
    case SDL_KEYUP:
     if(event.key.keysym.sym == SDLK_ESCAPE)
      quit = 1;
      break;
    case SDL_QUIT:
     quit = 1;
     break;
    default:
     break;
   }
  }

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
  if (t<10000)
  {
      indexfactor=(float)t/10000.0;
  }
  /*else
  {      
      if (t>20000)
          indexfactor=1-(float)(t-20000.0)/10000.0;
      else
          indexfactor=1.0;
  } */ 

  /* the actual drawing */

  //SDL_FillRect(screen,0,0);
  if (posx<=0) {posx=0;if (deltax<0) deltax=-deltax;}
  if (posx+WIDTH/2>=WIDTH) {posx=WIDTH/2;if (deltax>0) deltax=-deltax;}
  if (posy<=0) {posy=0;if (deltay<0) deltay=-deltay;}
  if (posy+HEIGHT/2>=HEIGHT) {posy=HEIGHT/2;if (deltay>0) deltay=-deltay;}
  plasma8_1(screen,0,0,WIDTH-1,HEIGHT-1,t,coeffs1,indexfactor,cosTable,colorx,colory);
  plasma8_1(screen,round(posx),round(posy),
    round(posx+WIDTH/2-1),round(posy+HEIGHT/2-1),
    t,coeffs2,indexfactor,cosTable,colorx,colory);
  if ((posx<=0 && deltax<0) ||
      (posx+WIDTH/2>=WIDTH && deltax>0))
    deltax=-deltax;
  if ((posy<=0 && deltay<0) ||
      (posy+HEIGHT/2>=HEIGHT && deltay>0))
    deltay=-deltay;
  printf("%i %f %f\n",t-told,posx,posx+deltax*(t-told));
  posx+=deltax*(t-told);
  posy+=deltay*(t-told);
  
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

