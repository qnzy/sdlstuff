#include "SDL/SDL.h"

/*
round the float to the nearest integer
*/
int yekround(float x)
{
    return (x-(int)x)<0.5?(int)x:(int)x+1;
}


/*
draws a plasma with the current palette on the surface s (8bpp mode). 
the plasma has the following form:

p(x,y)= indexfactor*
        ( cosTable[(c0*x+c1*t)&255]+cosTable[(c2*x+c3*t)&255]+
          cosTable[(c4*x+c5*t)&255]+cosTable[(c6*x+c7*t)&255] )

Inputs 
        s : surface to draw on
        x1,y1,x2,y2 : region to draw plasma
        t : time in ms
        coeffs : plasma coefficients (see above, c0..c7)
        indexfactor : scale the palette (see above)
        cosTable : cosine table; built like this:
                   for (t=0;t<256;t++)
                     cosTable[t]=(int)(32.0+32.0*cos(t*2.0*3.141592/256.0));
        colorx : buffer with s->w elements
        colory : buffer with s->h elements

No Outputs

note:maybe improvement when rounding while calculating colorx/y[i]...
*/
void plasma8_1(SDL_Surface* s, int x1,int y1,int x2,int y2,
             int t, float* coeffs,float indexfactor, int* cosTable,
             int*colorx,int*colory)
{
 int x, y;
 Uint8 *p;
 /* precalculated products */
 float ax,cx,ey,gy;
 float bt,dt,ft,ht;
 
  bt=coeffs[1]*t;dt=coeffs[3]*t;ft=coeffs[5]*t;ht=coeffs[7]*t;
  ax=bt;cx=dt;
  for (x=x1;x<=x2;x++)
  {
    colorx[x]=(indexfactor*(cosTable[((int)ax)&255]+
              cosTable[((int)cx)&255]));
    ax+=coeffs[0];cx+=coeffs[2];
  }
  ey=ft;gy=ht;
  for (y=y1;y<=y2;y++)
  {
    colory[y]=(indexfactor*(cosTable[((int)ey)&255]+
              cosTable[((int)gy)&255])); 
    ey+=coeffs[4];gy+=coeffs[6];
  }
  for (y=y1;y<=y2;y++)
  {
      p=(Uint8*)s->pixels + y * s->pitch + x1;
      for (x=x1;x<=x2;x++)
        *(p++)=(Uint8)(colorx[x]+colory[y]);
  }
}

/*same as plasma8_1 but mirrored to make it tile seamlessely*/
void plasma8_2(SDL_Surface* s, int x1,int y1,int x2,int y2,
             int t, float* coeffs,float indexfactor, int* cosTable,
             int*colorx,int*colory)
{
 int x, y;
 Uint8 *p;
 /* precalculated products */
 float ax,cx,ey,gy;
 float bt,dt,ft,ht;
 
  bt=coeffs[1]*t;dt=coeffs[3]*t;ft=coeffs[5]*t;ht=coeffs[7]*t;
  ax=bt;cx=dt;
  for (x=x1;x<=0.5*(x1+x2)+1;x++)
  {
    colorx[x]=(indexfactor*(cosTable[((int)ax)&255]+
              cosTable[((int)cx)&255]));
    //colorx[x2-x]=colorx[x];
    ax+=coeffs[0];cx+=coeffs[2];
  }
  for (x=0.5*(x1+x2);x<=x2;x++)
  {
      colorx[x]=colorx[(x1+x2)-x];
  }
  ey=ft;gy=ht;
  for (y=y1;y<=0.5*(y1+y2)+1;y++)
  {
    colory[y]=(indexfactor*(cosTable[((int)ey)&255]+
              cosTable[((int)gy)&255])); 
    //colory[y2-y]=colory[y];
    ey+=coeffs[4];gy+=coeffs[6];
  }
  for (y=0.5*(y1+y2);y<=y2;y++)
  {
      colory[y]=colory[(y1+y2)-y];
  }
  for (y=y1;y<=y2;y++)
  {
      p=(Uint8*)s->pixels + y * s->pitch + x1;
      for (x=x1;x<=x2;x++)
        *(p++)=(Uint8)(colorx[x]+colory[y]);
  }
}


/*
Create a Palette (see code)
*/
void sinpalette(SDL_Color *palette,float r,float g,float b,float rf,float gf,float bf)
{
    int i;
    for (i=0;i<256;i++)
    {
        palette[i].r=(int)(r*(128.0+128.0*sin(3.141592*i/rf)));
        palette[i].g=(int)(g*(128.0+128.0*sin(3.141592*i/gf)));
        palette[i].b=(int)(b*(128.0+128.0*sin(3.141592*i/bf)));
    }
}

/*
Create a Palette. 
	for i=0 to numpoints
		palette[index[i]]=points[i]
	end
	linear interpolate between points
*/
void linpalette(SDL_Color *palette,int numpoints,int *index,SDL_Color *points)
{
	//todo...
}


/*
move a value between min and max at (*velocity) pixels per time
unit. if it hits a border the sign of the velocity is inverted.
*/
void moveval(float *val,float *velocity, int deltat, float min, float max)
{
  (*val)+=(*velocity)*deltat;
  if ((*val)<=min) {(*val)=min;if ((*velocity)<0) (*velocity)=-(*velocity);}
  if ((*val)>=max) {(*val)=max;if ((*velocity)>0) (*velocity)=-(*velocity);}
}


/*
return a ramp from 0 to 1 if for 0<t<rampup.
return a 1 for rampup<t<rampup+up.
return a ramp from 1 to 0 for rampup+up<t<rampup+up+rampdown.
can be used for fading
*/
float getindexfactor(int t,int rampup,int up, int rampdown)
{
  if (t<rampup)
  {
      return (float)t/rampup;
  }
  else
  {      
      if (t>rampup+up)
          return 1.0-(t-rampup-up)/(float)rampdown;
      else
          return 1.0;
  }
}



/*
check eventqueue. set (*quit) to 1 if ESC is pressed
of the window is closed.
*/
void eventcheck(int *quit)
{
    SDL_Event event;
    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
            case SDL_KEYUP:
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    (*quit) = 1;
                break;
            case SDL_QUIT:
                (*quit) = 1;
                break;
            default:
                break;
        }
    }
}

/*
rzm: rotate zoom move
*/
void rzm(SDL_Surface *dest, SDL_Surface *src, float x, float y, float zoom, float angle)
{
	//todo
}

/*
read a pixel from a surface
from the SDL tutorial
*/
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/*
write a pixel to a surface
from the SDL tutorial
*/
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

void xortexture8(SDL_Surface *s)
{
    int x,y;
    for (x=0;x<s->w;x++)
        for (y=0;y<s->h;y++)
            putpixel(s,x,y,(x*256/s->w)^(y*256/s->h));
}

/*
map the texture to the surface:

    index=(x+startx)+tablepitch*(y+starty)
    s(x,y)=texture  ((uTable[index]+uofset) mod texture_width,
                     (vTable[index]+voffset) mod texture_height )

with uTable=angle and vTable=ratio*textureheight/distance this
gives the tunnel effect.

*/
void tablemap8(SDL_Surface *s, SDL_Surface *texture, 
            float *uTable,float *vTable,
            int tablepitch,
            int startx, int starty,
            float uoffset, float voffset)
{
    int x,y,u,v,index;
    Uint8 *p;
    for (y=0;y<s->h;y++)
    {
        index=(startx)+tablepitch*(y+starty);
        p=(Uint8*)s->pixels+y*s->pitch;
        for (x=0;x<s->w;x++)
        {
            u=((int)(uTable[index]+uoffset));
            while (u<0) u+=texture->w;
            while (u>=texture->w) u-=texture->w;
            v=((int)(vTable[index++]+voffset));
            while (v<0) v+=texture->h;                
            while (v>=texture->h) v-=texture->w;
            *(p++)=*((Uint8*)texture->pixels+v*texture->pitch+u);
        }
    }
}

/*same as tablemap8 but works only with textures of size 255x255*/
void tablemap8_255(SDL_Surface *s, SDL_Surface *texture, 
            float *uTable,float *vTable,
            int tablepitch,
            int startx, int starty,
            float uoffset, float voffset)
{
    int x,y,u,v,index;
    Uint8 *p;
    for (y=0;y<s->h;y++)
    {
        index=(startx)+tablepitch*(y+starty);
        p=(Uint8*)s->pixels+y*s->pitch;
        for (x=0;x<s->w;x++)
        {
            u=((int)(uTable[index]+uoffset))&255;
            v=((int)(vTable[index++]+voffset))&255;
            *(p++)=*((Uint8*)texture->pixels+v*texture->pitch+u);
        }
    }
}
