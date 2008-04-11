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

#include "fx.h"

void Zero_Palette( void )     
    {
     int index;
     RGB_color color;

     color.red = 0;
     color.green = 0;
     color.blue = 0;

     /* Enable color palette */
     for ( index = 0; index < 256; index ++ ) 
          {
           Set_Palette_Register( index, (RGB_color_ptr)&color );
          }
     /* Enabled */

    }

void Fade_Outa_Palette( pcx_picture_ptr image )
    {
     RGB_color color;
     int offset = 0;
     int thisun;
     int i, index;
     
     /* Enable color palette */
   for( i = 0; i < 64; i++ )  
       {

        for ( index = 0; index < 256; index ++ ) 
             {
              thisun = (int)image->palette[index].red + offset;
              if (thisun < 0 ) thisun = 0;
              color.red = thisun;
            
              thisun = (int)image->palette[index].green + offset;
              if (thisun < 0 ) thisun = 0;
              color.green = thisun;

              thisun = (int)image->palette[index].blue + offset;
              if (thisun < 0 ) thisun = 0;
              color.blue = thisun;

             Set_Palette_Register( index, (RGB_color_ptr)&color );
          
            
             }
           offset--;

       }

     /* Enabled */
    } 

void Fade_Into_Palette( pcx_picture_ptr image )
    {
     RGB_color color;
     int offset = -64;
     int thisun;
     int i, index;
     
     /* Enable color palette */
   for( i = 0; i < 64; i++ )  
       {

        for ( index = 0; index < 256; index ++ ) 
             {
              thisun = (int)image->palette[index].red + offset;
              if (thisun < 0 ) thisun = 0;
              color.red = thisun;
            
              thisun = (int)image->palette[index].green + offset;
              if (thisun < 0 ) thisun = 0;
              color.green = thisun;

              thisun = (int)image->palette[index].blue + offset;
              if (thisun < 0 ) thisun = 0;
              color.blue = thisun;

             Set_Palette_Register( index, (RGB_color_ptr)&color );
          
            
             }
           offset++;

       }

     /* Enabled */
    } 


/* Gradient the palette from index one to index two  */
void Cycle_Palette( pcx_picture_ptr image )
    {

     int index;
     RGB_color color;
     RGB_color_ptr color_ptr = &color;
          
          color_ptr->red   = image->palette[ 1 ].red;     
          color_ptr->green = image->palette[ 1 ].green;
          color_ptr->blue  = image->palette[ 1 ].blue;
          
          image->palette[254].red = color_ptr->red;      
          image->palette[254].green = color_ptr->green;
          image->palette[254].blue = color_ptr->blue;

     for ( index = 1; index < 254; index++ )
         {
          color_ptr->red   = image->palette[index + 1].red;     
          color_ptr->green = image->palette[index + 1].green;
          color_ptr->blue  = image->palette[index + 1].blue;
          
          image->palette[index].red = color_ptr->red;      
          image->palette[index].green = color_ptr->green;
          image->palette[index].blue = color_ptr->blue;
          
          Set_Palette_Register( index, color_ptr );   
          
         } /* End for index loop */
          

    } /* End Cycle_Palette */



void Grad_Palette( int index_one, int index_two, unsigned char color_one, unsigned char color_two )
    {
     /* Color one and color two are not used yet...
        Eventually I will make it so you can gradient
        From any color to any other color in as many
        Steps as you like anywhere in the palette
        Right now it only goes from black to white */

     float per_step; /*  This is how much we change every step */
     int index;

     RGB_color color;
     RGB_color_ptr color_ptr = &color;
          
    /* if ( per_step < 1 )  //This ensures that we will be moving at least one step at a time */
         per_step = 0;

     color_ptr->red   = 0;      /* Start the color out at dark grey */
     color_ptr->green = 0;      
     color_ptr->blue  = 0;      

     for ( index = 1; index < 254; index++ )
         {
          Set_Palette_Register( index, color_ptr ); /* Set the palette at index
                                                       To current step of grey   */
          if(per_step == 0.25)                                         
              {                
               color_ptr->red += 1;
              }
          if(per_step == 0.50)                                         
              {                
               color_ptr->green += 1;
              } 
          if(per_step == 0.75)                                         
              {                
              color_ptr->blue += 1;
              } 
          
          
          
          if(per_step > 0.75)                                         
              {                
              per_step = 0;
              } 
              
          per_step = per_step + 0.25f;                 
             
         
         } /* End for index loop */


    } /* End Grad_Palette */


/*

    sprite Page;  
void Chunk_Screen( void )
    {
   
     int xos[MAX_SPRITE_FRAMES];
     int yos[MAX_SPRITE_FRAMES];
     int sprite_frames = 0;
     int i, j;
                                       
     
     for ( j = 0; j < 8; j++ )
         {
          for ( i = 0; i < 13; i++)
              {
               Grab_Sprite( double_buffer, &Page, sprite_frames, i, j );
               xos[sprite_frames] = 24*i;
               yos[sprite_frames] = 24*j;
               sprite_frames++;
              }
         }

     Page.curr_frame = 0;

     while ( yos[5] < 250  )
         {
          DB_Clear_Screen();
          for ( i = 0; i < sprite_frames; i++ )
              {
               if( yos[i] < 175 )
                   {
                    Page.xo = xos[i];
                    Page.yo = yos[i];
                    Draw_Sprite( &Page );
                    Page.curr_frame++;
                   }
              }
    
            for ( i = 0; i < sprite_frames; i++ )
                {
                 yos[i] += rand()%24;
                }
         
            Page.curr_frame = 0;
            Swap_Buffer();      
         }
    }

    */

void Dim_Palette( pcx_picture_ptr image, int speed )
    {
     int thisun;
     int index;
     
     /* Enable color palette */

        for ( index = 0; index < 256; index ++ ) 
             {
              thisun = (int)image->palette[index].red - speed;
              if (thisun < 0 ) thisun = 0;
              image->palette[index].red = thisun;
            
              thisun = (int)image->palette[index].green - speed;
              if (thisun < 0 ) thisun = 0;
              image->palette[index].green = thisun;

              thisun = (int)image->palette[index].blue - speed;
              if (thisun < 0 ) thisun = 0;
              image->palette[index].blue = thisun;


            
             } /* End of loop through palette */


     /* Wait_For_Vsync(); */
     /* Set_Palette_Register( index, (RGB_color_ptr)&color ); */
          

    } /* End of Dim_Palette() */


void Brighten_Palette( pcx_picture_ptr image, pcx_picture_ptr image2, int speed )
    {
     int thisun;
     int index;
     
     /* Enable color palette */

        for ( index = 0; index < 256; index ++ ) 
             {
              thisun = (int)image->palette[index].red + speed;
              if (thisun > image2->palette[index].red ) thisun = image2->palette[index].red;
              image->palette[index].red = thisun;
            
              thisun = (int)image->palette[index].green + speed;
              if (thisun > image2->palette[index].green ) thisun = image2->palette[index].green;
              image->palette[index].green = thisun;

              thisun = (int)image->palette[index].blue + speed;
              if (thisun > image2->palette[index].blue ) thisun = image2->palette[index].blue;
              image->palette[index].blue = thisun;

            
             } /* End of loop through palette */


     /* Wait_For_Vsync(); */
     /* Set_Palette_Register( index, (RGB_color_ptr)&color ); */
          

    } /* End of Brighten_Palette() */


void Cycle_Palette_Section( int start, int stop, pcx_picture_ptr image )
    {
     int index;
     RGB_color color;



     color.red   = image->palette[start].red;     
     color.green = image->palette[start].green;
     color.blue  = image->palette[start].blue;
          
     image->palette[stop].red   = color.red;      
     image->palette[stop].green = color.green;
     image->palette[stop].blue  = color.blue;

     for ( index = start; index < stop; index++ )
         {
          color.red   = image->palette[index + 1].red;     
          color.green = image->palette[index + 1].green;
          color.blue  = image->palette[index + 1].blue;
          
          image->palette[index].red   = color.red;      
          image->palette[index].green = color.green;
          image->palette[index].blue  = color.blue;

         } /* End for index loop */

    } /* End of Cycle_Palette_Section() */
