/*
    Copyright (C) 2001 Hotwarez LLC, Goldtree Enterprises
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "rotbmp.h"
#include <math.h>

#define MAGIC2 65536  /* This is 1 in our second fixed point number trick
                         The other one (MAGIC) is in gfx_type.h */

#define MEXP2 16     /* The number of bits we shift to convert from
                        Fixed point to regular ints and viece versa */
 
/* macro so we don't have to call an external pixel plotter */
#define funkit(x,y,color) funky_buffer[(((y)<<8) + ((y)<<6)) + (x)] = (color);    


/* globals */

char *funky_buffer; /* pointer we will use for the double
                           buffer */ 
long max_index;         /* The ending index of the image->buffer */
unsigned long image_index = 0;   /* our position in the image buffer */
long y_offset;          /* For scaling along the y axis */
long total_ystep;      /* Determines how often we repeat or skip scan(?) lines */
long total_index;      /* This is our ystep position in the image->buffer */
             

long left_step = 0;
long right_step = 0;
long left_index = 0;
long right_index = 0;





/* Digline draws a line from x1,y1 to x2,y2 but instead of drawing a
   Solid color it draws a strip from the image->buffer it is sent.  It
   Keeps track of its position in the buffer with the static long image_index.
   It also scales the line so one horizontal line from the buffer maps
   Onto our line (x1,y1),(x2,y2) */

void digline( int x1, int y1, int x2, int y2, pcx_picture_ptr image )
    {
    
     long temp_image_index = image_index; 
     int d, x, y, ax, ay, sx, sy, dx, dy;
     long xstep, ystep;
     char *buffer_ptr; /* temporary pointer for easy buffer reference */
     long local_xpixels = image->xpixels;
     
     int hackx, hacky; /* Temporary solution to nasty clipping problem */


     buffer_ptr = image->buffer; /* Alias out pointer to the image->buffer */

          
     /* Figure out the steps */
     if( x2 != x1 )
         {
          xstep = ((local_xpixels - 1) << MEXP2) / (x2 - x1); 
          xstep = ABS(xstep);
         }
     if( y2 != y1 )
         {
          ystep = ((local_xpixels - 1) << MEXP2) / (y2 - y1);
          ystep = ABS(ystep);
         }
     
     
     dx = x2-x1; ax = ABS(dx)<<1; sx = ZSGN(dx);
     dy = y2-y1; ay = ABS(dy)<<1; sy = ZSGN(dy);


     x = x1;
     y = y1;

     
     if( ax > ay )
         {
          d = ay - (ax>>1);
          for(;;)
              {
               if( x > 0 && x < 319 && y > 0 && y < 199 )
                   DB_Fast_Pixel( x, y, buffer_ptr[ (image_index >> MEXP2) ] );
               
               image_index = image_index + xstep;      
           
               if( x==x2) 
                   {
                    image_index = temp_image_index + ((local_xpixels + 1) << MEXP2);
                    return;
                   }
               if(d>=0)
                   {
                    y += sy;
                    d -= ax;   
                    if( x > 0 && x < 319 && y > 0 && y < 199 )        
                        DB_Fast_Pixel( x, y, buffer_ptr[ (image_index >> MEXP2) ] );      
                   }
               d += ay; 
               x += sx;
              
              }
         }
     else 
         {
          d = ax-(ay>>1);
          for(;;)
              {
               if( x > 0 && x < 319 && y > 0 && y < 199 )       
                   DB_Fast_Pixel( x, y, buffer_ptr[ (image_index >> MEXP2) ] );         
                 
               image_index = image_index + ystep;     
              
               if(y==y2) 
                   {
                    image_index = temp_image_index + ((local_xpixels + 1) << MEXP2);
                    return;
                   }
               if(d>=0)
                   {
                    x += sx;
                    d -= ay; 
                    if( x > 0 && x < 319 && y > 0 && y < 199 )       
                        {
                         DB_Fast_Pixel( x, y, buffer_ptr[ (image_index >> MEXP2) ] );      
                         if( ABS(ax - ay) <= 2 )
                             {
                             hackx = x - sx;
                             hacky = y + sy;
                             if( hackx > 0 && hackx < 319 && hacky > 0 && hacky < 199 )         
                                 DB_Fast_Pixel( hackx, hacky, buffer_ptr[ (image_index >> MEXP2) ] );  /*We plot extra points here   */
                             hackx = x;
                             hacky = y - sy;
                             if( hackx > 0 && hackx < 319 && hacky > 0 && hacky < 199 )  
                                 DB_Fast_Pixel( hackx, hacky, buffer_ptr[ (image_index >> MEXP2) ] );       /* To solve the 'black holes' problem */
                             }
                        }    
                   
                   }
               y += sy;
               d += ax;
              }
         }
    }



/* Figure out all of the whole number x,y values between line x1,y1 and x2,y2 */
/* Uses bressenbaumenhamens line algorithm(sorta) */
int figure_line( int x1, int y1, int x2, int y2, int xarray[], int yarray[] )
    {
     int d, x, y, ax, ay, sx, sy, dx, dy;
     int index = 0;
   
     dx = x2-x1; ax = ABS(dx)<<1; sx = ZSGN(dx);
     dy = y2-y1; ay = ABS(dy)<<1; sy = ZSGN(dy);


     x = x1;
     y = y1;

     
     if( ax > ay )
         {
          d = ay - (ax>>1);
          for(;;)
              {
              xarray[index] = x; 
              yarray[index] = y;
              index++;
                  
           
               if( x==x2) 
                   {
                    return(index);
                   }
               if(d>=0)
                   {
                    y += sy;
                    d -= ax;   
                   }
               d += ay; 
               x += sx;
              
              }
         }
     else
         {
          d = ax-(ay>>1);
          for(;;)
              {
              xarray[index] = x; 
              yarray[index] = y;
              index++;
                 
               if(y==y2) 
                   {
                    return(index);
                   }
               if(d>=0)
                   {
                    x += sx;
                    d -= ay; 
                   }
               y += sy;
               d += ax;
              }
         }
    }




int x1[500], x2[500], y2[500];


      int popo[500]; /* For some reason variable name 'y1' is crashing      
                        So I am using the temporary name 'popo' */
      
void Texture( pcx_picture_ptr image, int newx1, int newy1, int newx2, int newy2,
              int newx3, int newy3, int newx4, int newy4 )
    {
     int i;
   
     int line_length, line_length2; /* The length of the two sides of the
                                       Area to map onto */
     
     
     funky_buffer = Return_Double_Buffer(); /* Alias our pointer to the double buffer
                                               For the pixel plot macro */

     max_index = ((long)(image->xpixels + 1) * (long)(image->ypixels + 1)) << MEXP2;           
     /* This is the end of the image buffer we are mapping */


     /* Figure out all pixels between tl,bl and all pixels between */
     /* tr, br */
     line_length = figure_line( newx1, newy1, newx4, newy4, x1, popo ); /* Get values for left 'vertical' bar */
     line_length2 = figure_line( newx2, newy2, newx3, newy3, x2, y2 ); /* Get values for right 'vertical' bar */


     /* Here we figure out the y axis ratios of ypixels along the 
        Left side and along the right side of the image to the area 
        We're mapping them onto */

     if( line_length > line_length2 )
         {
          right_step = ((long)line_length2 << MEXP2) / (long)line_length;
          left_step = MAGIC2;  /*  1:1 ratio of pixels  */
         }
     else if( line_length < line_length2 )
         {
          left_step = ((long)line_length << MEXP2) / (long)line_length2;
          right_step = MAGIC2; /*  1:1 ratio of pixels */
         }
      else /* if theyre the same */
         {
          left_step = MAGIC2;  /*  1:1 ratio of pixels */
          right_step = MAGIC2; /*  1:1 ratio of pixels */
         }

     

     if( line_length < line_length2 ) /* I forgot why I did this */
         line_length = line_length2;

      
     if( line_length < image->ypixels + 1)
         y_offset = (long)(image->xpixels + 1) << MEXP2;
     else
         y_offset = -(long)(image->xpixels + 1) << MEXP2;

     
     total_ystep = ((long)image->ypixels << MEXP2) / ((long)line_length);

          
total_index = 0;                        

     for( i = 0; i < line_length; i++ )
         {
          while( total_index > MAGIC2 )
              {
               total_index = total_index - MAGIC2;
               image_index = image_index + y_offset;
              
            /*   if( image_index < 0 )
                   exit(0); */ /* Unsigned is always > 0 */

              
              }
              
          digline( x1[(int)(left_index >> MEXP2)], popo[(int)(left_index >> MEXP2)], x2[(int)(right_index >> MEXP2)], y2[(int)(right_index >> MEXP2)], image );
             
          if( total_ystep >= MAGIC2 )
              total_index = total_index + total_ystep - MAGIC2;
          else 
              total_index = total_index + ( MAGIC2 - total_ystep );

             
          left_index = left_index + left_step; 
          right_index = right_index + right_step;
             
             
          } 
             
     image_index = 0;    
     total_index = 0; /* Clean up */
     left_index = 0;
     right_index = 0;
     max_index = 0;    
     image_index = 0;  
     y_offset = 0;     
     total_ystep = 0;  
     total_index = 0;  

    } /* End of Texture */

