//
// gl port work
//

#ifndef GLCYLINDRIX_H
#define GLCYLINDRIX_H

// AJT: dependency issue.
//#include "allegro/palette.h"

void GL_Cylindrix_Init();

void GL_Clear();

enum TubeType { CheckerTube, WireTube, PointTube };
void GL_DrawTube( WorldStuff* worldStuff, enum TubeType tubeType );

void GL_DrawEverything( WorldStuff* worldStuff );

void GL_SwapBuffers();

void GL_SubloadAndSwapBuffer();

// AJT: dependency issue.
//void GL_SetFrameBufferPalette( PALETTE palette );

void GL_FadeOutFrameBuffer( int frames );
void GL_FadeInFrameBuffer( int frames );

#endif

