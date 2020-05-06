#include <stdio.h>
#include <math.h>

#include "SDL/SDL.h"

#define WIDTH 800
#define HEIGHT 600

#define VMFLAGS SDL_SWSURFACE|SDL_DOUBLEBUF /* | SDL_FULLSCREEN */

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

int main(int argc, char *argv[])
{
 SDL_Surface *screen;
 int quit = 0;
 SDL_Event event;
 int x, y;
 float speed,size;
 int cosTable[256];
 int t0,t;
 int redx[WIDTH],greenx[WIDTH],bluex[WIDTH];
 int redy[HEIGHT],greeny[HEIGHT],bluey[HEIGHT];
  /* factors for cosine function */
 float ar,br,cr,dr,er,fr,gr,hr;
 float ag,bg,cg,dg,eg,fg,gg,hg;
 float ab,bb,cb,db,eb,fb,gb,hb;    
 /* precalculated products */
 float arx,crx,agx,cgx,abx,cbx;
 float ery,gry,egy,ggy,eby,gby;
 float brt,drt,frt,hrt;
 float bgt,dgt,fgt,hgt;
 float bbt,dbt,fbt,hbt;
    
    
    for (t=0;t<256;t++)
        cosTable[t]=(int)(32.0+32.0*cos(t*2.0*3.141592/256.0));
    
    speed=2.5;
    size=0.4;
    
    ar=size*0.5930;  br=-0.006*speed;   
    cr=size*2.0180;  dr=0.130*speed;    
    er=size*0.2200;  fr=0.060*speed;
    gr=size*1.0820;  hr=-0.106*speed;

    ag=size*0.293;  bg=0.0156*speed;
    cg=size*1.044;  dg=-0.083*speed;
    eg=size*0.847;  fg=-0.0021*speed;
    gg=size*2.041;  hg=0.048*speed;
    
    ab=size*1.240;  bb=-0.002*speed;
    cb=size*1.844;  db=0.066*speed;
    eb=size*2.120;  fb=-0.014*speed;  
    gb=size*0.122;  hb=0.136*speed;

    
    t0=SDL_GetTicks();


 /* Initialize defaults, Video and Audio */
 if((SDL_Init(SDL_INIT_VIDEO)==-1))
 {
  printf("Could not initialize SDL: %s.\n", SDL_GetError());
  return -1;
 }

 screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, VMFLAGS);
 if ( screen == NULL )
 {
  fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
  return -2;
 }
 SDL_ShowCursor(SDL_DISABLE);

 while( !quit )
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
  
  /* Paint something */
  
  brt=br*t;drt=dr*t;frt=fr*t;hrt=hr*t;
  bgt=bg*t;dgt=dg*t;fgt=fg*t;hgt=hg*t;
  bbt=bb*t;dbt=db*t;fbt=fb*t;hbt=hb*t;

  arx=0;crx=0;agx=0;cgx=0;abx=0;cbx=0;
  for (x=0;x<screen->w;x++)
  {
    redx[x]=cosTable[(int)(arx +brt)&255]+
            cosTable[(int)(crx +drt)&255];
    greenx[x]=cosTable[(int)(agx +bgt)&255]+
              cosTable[(int)(cgx +dgt)&255];
    bluex[x]=cosTable[(int)(abx +bbt)&255]+
             cosTable[(int)(cbx +dbt)&255];
    arx+=ar;crx+=cr;agx+=ag;cgx+=cg;abx+=ab;cbx+=cb;
  }
  
  ery=0;gry=0;egy=0;ggy=0;eby=0;gby=0;
  for (y=0;y<screen->h;y++)
  {
    redy[y]=cosTable[(int)(ery+frt)&255]+
            cosTable[(int)(gry+hrt)&255]; 
    greeny[y]=cosTable[(int)(egy+fgt)&255]+
              cosTable[(int)(ggy+hgt)&255];
    bluey[y]=cosTable[(int)(eby+fbt)&255]+
             cosTable[(int)(gby+hbt)&255];
    ery+=er;gry+=gr;egy+=eg;ggy+=gg;eby+=eb;gby+=gb;
  }

  for (x=0;x<screen->w;x++)
  {
      for (y=0;y<screen->h;y++)
      {
        putpixel(screen, x, y, SDL_MapRGB(
        screen->format, 
        redx[x]+redy[y], 
        greenx[x]+greeny[y], 
        bluex[x]+bluey[y])
        );
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

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
 int bpp = surface->format->BytesPerPixel;
 /* Here p is the address to the pixel we want to set */
 Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

 switch(bpp)
 {
  case 1:
   *p = pixel;
   break;
  case 2:
   *(Uint16 *)p = pixel;
   break;
  case 3:
   if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
   {
    p[0] = (pixel >> 16) & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = pixel & 0xff;
   } else {
    p[0] = pixel & 0xff;
    p[1] = (pixel >> 8) & 0xff;
    p[2] = (pixel >> 16) & 0xff;
   }
   break;
  case 4:
   *(Uint32 *)p = pixel;
   break;
 }
}
