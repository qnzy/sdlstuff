#include <stdio.h>
#include <math.h>

#include "SDL/SDL.h"

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
 int t0,t;
 float indexfactor;
 int colorx[WIDTH],colory[HEIGHT];
  /* factors for cosine function */
 float a,b,c,d,e,f,g,h;
 /* precalculated products */
 float ax,cx,ey,gy;
 float bt,dt,ft,ht;
    
 for (t=0;t<256;t++)
  cosTable[t]=(int)(32.0+32.0*cos(t*2.0*3.141592/256.0));

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
    
    speed=0.5;
    size=1.0;
    
    a=size*0.5930;  b=-0.206*speed;   
    c=size*2.0180;  d=0.130*speed;    
    e=size*0.2200;  f=0.360*speed;
    g=size*1.0820;  h=-0.106*speed;
    
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
 
 while( !quit && t<30000)
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
  
  t=SDL_GetTicks()-t0;
  
  if (t<10000)
  {
      indexfactor=(float)t/10000.0;
  }
  else
  {      
      if (t>20000)
          indexfactor=1-(float)(t-20000.0)/10000.0;
      else
          indexfactor=1.0;
  }  
  /* Paint something */
  
  bt=b*t;dt=d*t;ft=f*t;ht=h*t;

  ax=0;cx=0;
  for (x=0;x<screen->w;x++)
  {
    colorx[x]=cosTable[(int)(ax +bt)&255]+
              cosTable[(int)(cx +dt)&255];
    ax+=a;cx+=c;
  }
  
  ey=0;gy=0;
  for (y=0;y<screen->h;y++)
  {
    colory[y]=cosTable[(int)(ey+ft)&255]+
              cosTable[(int)(gy+ht)&255]; 
    ey+=e;gy+=g;
  }

  for (x=0;x<screen->w;x++)
  {
      for (y=0;y<screen->h;y++)
      {
         Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x;
          *p=(Uint8)(indexfactor*(colorx[x]+colory[y]));
      }
  }
  
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

