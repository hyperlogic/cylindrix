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

#include "pcx.h"
#include "prim.h"

extern const char* g_DataPath;

/* Allocate about 64k for image->buffer */
void PCX_Allocate( pcx_picture_ptr image )
    {

     ;

     /*
     image->buffer = (char *)malloc( 64000 + 1 );

     if ( image->buffer == NULL )
         {
          printf("Not Enough Memory!!!");
          exit(0);
         }

     */

    } /* End of PCX_Allocate */



/* Function PCX load loads and uncompresses a PCX image into image->buffer
   And loads the palette into image->palette
   Based on code from Graphics Gurus book */
void PCX_Load( char *filename, pcx_picture_ptr image )
    {

     FILE *fp;              /* File pointer */
     long index;            /* Multipurpose index  */
     long new_pos = 0;      /* Position in the image->buffer */
     int num_bytes;         /* Number of bytes in current RLE run */
     int info;              /* Data from temp_buffer */
     unsigned char *p;

     int scan_line_length;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);



     if ( (fp = fopen( newfilename, "rb") ) == NULL ) /* Open the PCX file */
         {
          SYS_Error("ERROR OPENING FILE!!! %s\n", newfilename);
          exit(0);
         }

     index = 0;

     /* Read in the header */
     fseek( fp, 0, SEEK_SET ); /* Move to beginning of file */
     fread( &image->header, sizeof( pcx_header ), 1, fp );

     /* Compute the width of the image in pixels  */
     image->xpixels = image->header.xend - image->header.xstart;

     /* Length of image in pixels */
     image->ypixels = image->header.yend - image->header.ystart;

      /* Figure out how many pixels per line */
      scan_line_length = ( image->header.bytes_per_line * image->header.num_bit_planes );

      image->xpixels = scan_line_length - 1;


      /* Allocate memory for picture...the +100 is just to be safe */
      image->buffer = (unsigned char *)malloc( ((long)(image->xpixels + 1 ) * (long)(image->ypixels + 1)) + 100 );
      if ( image->buffer == NULL )
          {
           printf("Not Enough Memory!!!");
           exit(0);
          }

      p = image->buffer;

      /* Decompress data from file into image->buffer */

      fseek( fp, 128, SEEK_SET ); /* Move to 128 bytes from beginning of file */


     while( new_pos < ( ((long)image->xpixels + 1 ) * (long)(image->ypixels + 1) ) )  /* Loop till beginning of color palette  */
         {                                                                            /* And end of data */

          info = getc(fp);              /* Store one char from the buffer  */
                                        /* In info */

          if ( info >= 192 && info <= 255 )    /* Test to se if the value in  */
              {                                /* Info is between 192 and 255 */
               num_bytes = info - 192;         /* If it is then get the next */
               /* Get actual data for the run   Value from the buffer and */
               info = getc(fp);                /* Replicate it num_bytes times
                                                  In the new buffer */

               /* Replicate the data in the buffer num_bytes times */
               while( num_bytes-- > 0 )
                   {
                    p[new_pos++] = info;
                   } /* End while */

              } /* End if RLE */

          else  /* If the info value wasn't between 192 and 255 */
              { /* Then just replicate it into the new buffer */
               p[new_pos++] = info;
              }  /* End else if not RLE */

         } /* End while */

     image->num_bytes = new_pos; /* This is the number of bytes in the uncompressed image */

     fseek( fp, -768L, SEEK_END );  /* Move to beginning of color palette  */
                                    /* Which is 768 bytes before the end */
     /* Load color palette  */
     for (index = 0; index < 256; index ++)
         {
          image->palette[index].red   = getc(fp) >>2;
          image->palette[index].green = getc(fp) >>2;
          image->palette[index].blue  = getc(fp) >>2;
         } /* End for index */
     /* Color palette loaded */

     fclose( fp );  /* Close the PCX file */

    } /* End of pcx load */


/* Enable the color palette contained in the image variable
   Based on code from graphics guru book */
void Enable_Color_Palette( pcx_picture_ptr image )
    {
     int index;

     /* Loop through all 256 color palette registers and assign
        to them the palette located in the pcx image sent */
     for ( index = 0; index < 256; index ++ )
          {
           Set_Palette_Register( index, (RGB_color_ptr)&image->palette[index] );
          }

    }

/* Enable the pcx palette from index_start to index_end */
void Enable_Palette_Range( pcx_picture_ptr image, int index_start, int index_end )
    {
     int i;

     for( i = index_start; i <= index_end; i++ )
          {
           Set_Palette_Register( i, (RGB_color_ptr)&image->palette[i] );
          }


    } /* End of Enable_Palette_Range */


/* Enable only the index'th member of the pcx palette */
void Enable_Palette_Member( pcx_picture_ptr image, int index )
    {
     Set_Palette_Register( index, (RGB_color_ptr)&image->palette[index] );
    }



void Set_One( int which_color, int index )
    {
     RGB_color color;


     color.red   = 255;
     color.green = 0;
     color.blue  = 0;

     Set_Palette_Register( index, (RGB_color_ptr)&color );

    } /* End of Set_One */



void Enable_Palette( palette_type palette ) {

	int index;

     //Loop through all 256 color palette registers and assign
     //to them the palette 
     for ( index = 0; index < 256; index ++ )
          {
           Set_Palette_Register( index, (RGB_color_ptr)&palette[index] );
          }

} /* End of Enable_Palette */
