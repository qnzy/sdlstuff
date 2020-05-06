#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL/SDL.h"

#include "yekgfx.h"

#define WIDTH 1920
#define HEIGHT 1080

#define TEXWIDTH 256

#define RAUP 10000
#define UP   20000
#define RADOWN 10000

#define VMFLAGS SDL_SWSURFACE|SDL_DOUBLEBUF//|SDL_FULLSCREEN
#define VMFLAGS SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN
int main(int argc, char *argv[])
{
  SDL_Surface *screen,*texture;
  int quit = 0;
  SDL_Event event;
  int x, y,u,v;
  float speed,size;
  int ratio;
  Uint8 *p1,*p2;
  int fract1,fract2;
  /* palette */
  SDL_Color myPalette[256];
  int cosTable[256];
  int t0,t,told,print_t;
  float indexfactor;
  int colorx[TEXWIDTH],colory[TEXWIDTH];
  /* factors for cosine function */
  float coeffs1[8];
  float coeffs2[8];
  float *angleTable;
  float *distTable;    
  float scale,dist;    
  int index;
  float camx,camy,deltax,deltay;
  int c;
  long  int countframes, printframes;
  deltax=0.1;deltay=0.05;
  if ((angleTable=(float*)malloc(2*WIDTH*2*HEIGHT*sizeof(float)))==NULL)
    {
      printf("Not enough memory.\n");
      return -1;
    }
  if ((distTable=(float*)malloc(2*WIDTH*2*HEIGHT*sizeof(float)))==NULL)
    {
      printf("Not enough memory.\n");
      return -1;
    }

    
  srand(SDL_GetTicks());
    
  for (t=0;t<256;t++)
    cosTable[t]=(int)(32.0+32.0*cos(t*2.0*3.141592/256.0));

 


  for (t=0;t<8;t++)
    {   
      if (t%2==0)
	scale=2.5;  /*size*/
      else
	scale=0.4; /*speed*/
      coeffs1[t]=scale*((float)rand()/RAND_MAX-0.5);
      coeffs2[t]=scale*((float)rand()/RAND_MAX-0.5);
    }


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

  texture=SDL_CreateRGBSurface(SDL_SWSURFACE,TEXWIDTH,TEXWIDTH,8,0,0,0,1);
  //xortexture8(texture); 
  sinpalette(myPalette,0,1,0,64,64,32);   
   //sinpalette(myPalette,1,1,1,128,64,32);   
  ratio=32;
  for (x=0;x<2*WIDTH;x++)
    {
      for (y=0;y<2*HEIGHT;y++)
	{
	  if ((x-WIDTH)!=0)
            angleTable[x+y*2*WIDTH]=0.5*TEXWIDTH*
	      atan2((float)y-HEIGHT,(float)x-WIDTH)/3.141592;
	  else
	    {   
	      if (y<HEIGHT)
		angleTable[x+y*2*WIDTH]=-(TEXWIDTH/4.0);
	      else
		angleTable[x+y*2*WIDTH]=+(TEXWIDTH/4.0);
	    }
	  dist=sqrt((x-WIDTH)*(x-WIDTH)+(y-HEIGHT)*(y-HEIGHT));
	  if (dist<20)
	    distTable[x+y*2*WIDTH]=(ratio*TEXWIDTH)/20.0+0.8*20*20-0.8*
	      dist*dist;
	  else
	    distTable[x+y*2*WIDTH]=(ratio*TEXWIDTH/dist);
	}
    }
 
  /* set palette */
  SDL_SetColors(screen,myPalette,0,256);

  t=0;
  countframes=0;
  printframes = countframes;
  print_t = t;
  t0=SDL_GetTicks();

  while( !quit)
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
      if ( SDL_MUSTLOCK(texture) )
	{
	  if ( SDL_LockSurface(texture) < 0 )
	    {
	      fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
	      return -3;
	    }
	}
      told=t;
      t=SDL_GetTicks()-t0;

      /* the actual drawing */

      moveval(&camx,&deltax,t-told,WIDTH*0.25,WIDTH*0.75);
      moveval(&camy,&deltay,t-told,HEIGHT*0.25,HEIGHT*0.75);
      //  camx=WIDTH/2.0+(int)(0.5*WIDTH/2.0*sin(0.2*t/1000.0));
      //  camy=HEIGHT/2.0+(int)(0.5*HEIGHT/2.0*sin(0.2*t/1000.0*2.0));
      fract2=fmod(t/5.0,texture->h);
      fract1=100*sin(t/1000.0)+256*sin(t/2000.0);
      //fract2=0;
      //fract1=100*sin(0/1000.0)+256*sin(0/2000.0);
      plasma8_2(texture,0,0,TEXWIDTH/2-1,TEXWIDTH/2-1,t/10,coeffs1,1,
		cosTable,colorx,colory);
      plasma8_2(texture,0,TEXWIDTH/2,TEXWIDTH/2-1,TEXWIDTH-1,t/10,coeffs1,1,
		cosTable,colorx,colory);
      plasma8_2(texture,TEXWIDTH/2,0,TEXWIDTH-1,TEXWIDTH/2-1,t/10,coeffs1,1,
		cosTable,colorx,colory);
      plasma8_2(texture,TEXWIDTH/2,TEXWIDTH/2,TEXWIDTH-1,TEXWIDTH-1,t/10,
		coeffs1,1,cosTable,colorx,colory);

      tablemap8(screen, texture, 
		angleTable,distTable,
		2*WIDTH,
		camx, camy,
		fract1, fract2);
  
      /* Unlock Surface if necessary */
      if ( SDL_MUSTLOCK(screen) )
	{
	  SDL_UnlockSurface(screen);
	}
      if ( SDL_MUSTLOCK(texture) )
	{
	  SDL_UnlockSurface(texture);
	}

      SDL_Flip(screen);
      countframes++;
      if (countframes - printframes > 100) {
          //printf("fps: %f\n", 1000.0*(countframes-printframes)/(t-print_t));
          print_t = t;
          printframes = countframes;
      }

    }
  t=SDL_GetTicks()-t0;
  printf("Number Of Frames: %ld\nTotal Time: %i\nAverage Frames Per Second: %f\n",
	 countframes,t-t0,1000.0*countframes/(t-t0));
  SDL_Quit();
  return 0;
}

