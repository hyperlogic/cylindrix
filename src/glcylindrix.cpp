// work in progress
#ifdef WIN32
#include <windows.h> // AJT: DEPENDENCY ISSUE
#endif

#include <stdio.h>

#ifdef DARWIN
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "types.h"
#include "util.h"
#include "glcylindrix.h"
#include "assert.h"
#include <math.h>

static int frame = 0;

static GLuint frameBufferTexture;
static GLuint paletteTexture;

extern unsigned int palette[256];

static unsigned int rgbaFrameBuffer[320*200];

#define FBU 320.0f/512.0f
#define FBV 200.0f/256.0f

static int fadeFrames = 0;
static float fadeDelta = 0;
static float fadeAlpha = 0.0;

extern unsigned char *double_buffer; // from prim.c

extern int g_width, g_height;

// converts the 8-bit double_buffer into the 32-bit rgbaFrameBuffer.
void makeFrameBuffer( unsigned char* double_buffer )
{
	int i;
	for ( i = 0; i < 64000; ++i )
	{
		rgbaFrameBuffer[i] = palette[ double_buffer[i] ];
	}
}

// for debugging
void GL_CheckError()
{
	GLenum error = glGetError();

	switch ( error )
	{
		case GL_NO_ERROR:
			break;

		case GL_INVALID_ENUM:
		case GL_INVALID_VALUE:
		case GL_INVALID_OPERATION:
		case GL_STACK_OVERFLOW:
		case GL_STACK_UNDERFLOW:
		case GL_OUT_OF_MEMORY:
		default:
			assert( 0 );
			break;
	}
}

// creates a 512x256 texture that we will use to subload the software framebuffer into.
static void GL_FrameBufferTextureInit()
{
    // create our render texture object
    glGenTextures( 1, &frameBufferTexture );
    glBindTexture( GL_TEXTURE_2D, frameBufferTexture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, 4, 512, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );	
}

// creates a 1x256 texture that we will use to subload the current 8-bit palette into.
static void GL_PalletteTextureInit()
{
    // create the pallete texture
    glGenTextures( 1, &paletteTexture );
    glBindTexture( GL_TEXTURE_2D, paletteTexture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );	
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );	
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, 4, 1, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );	
}

// renders the frameBufferTexture on a quad that fills the screen.
static void GL_RenderBuffer()
{
	// process fade.
	if ( fadeFrames > 0 )
	{
		fadeAlpha += fadeDelta;
		fadeFrames--;
	}
	else
	{				
		fadeAlpha = 1.0f;
	}

	// clamp fade alpha
	if ( fadeAlpha < 0 )
		fadeAlpha = 0;
	else if ( fadeAlpha > 1 )
		fadeAlpha = 1;	
	
	// Clear all pixels.
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();	
	float aspect = (float)g_width/g_height;
	glOrtho(-aspect, aspect, -1, 1, -1, 1);

	glMatrixMode( GL_MODELVIEW );	
	glLoadIdentity();

	glBindTexture( GL_TEXTURE_2D, frameBufferTexture );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);	

	float orig_aspect = 1.33333f; // 4:3
	glBegin( GL_POLYGON );
	glColor3f( fadeAlpha, fadeAlpha, fadeAlpha );
	
	glTexCoord2f( 0, FBV );	
	glVertex3f( -orig_aspect, -1, 0 );

	glTexCoord2f( FBU, FBV );	
	glVertex3f( orig_aspect, -1, 0 );

	glTexCoord2f( FBU, 0 );	
	glVertex3f( orig_aspect, 1, 0 );

	glTexCoord2f( 0, 0 );	
	glVertex3f( -orig_aspect, 1, 0 );
	
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glFlush();

	SYS_SwapBuffers();

	++frame;
}

// renders the 8-bit double_buffer, using opengl.
void GL_SubloadAndSwapBuffer()
{
	makeFrameBuffer( double_buffer );

	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);	
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_RGBA, GL_UNSIGNED_BYTE, rgbaFrameBuffer );

	GL_RenderBuffer();
}

// subloads the current palette into the palette texture.
void GL_SubloadPaletteTexture()
{
	glBindTexture( GL_TEXTURE_2D, paletteTexture );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 1, 256, GL_RGBA, GL_UNSIGNED_BYTE, palette );	
}

// AJT: depencency issue
/*
// converts the games 6-bit palette into a 32-bit one.
void GL_SetFrameBufferPalette( PALETTE paletteIn )
{
	int i;
	for ( i = 0; i < 256; ++i )
	{
		// for some reason these r,g,b values are 6-bit.
		palette[i] = 0xff000000 | (paletteIn[i].b << 18) | (paletteIn[i].g << 10) | (paletteIn[i].r << 2);
	}
}
*/

// sets up a palette fade
void GL_FadeOutFrameBuffer( int frames )
{
	fadeFrames = frames;
	fadeDelta = -1.0f / (float)frames; 
}

// sets up a palette fade
void GL_FadeInFrameBuffer( int frames )
{
	fadeFrames = frames;
	fadeDelta = 1.0f / (float)frames;
}

// creates the framebuffer & palette textures.
void GL_Cylindrix_Init()
{
	// initialize the texture used that the game bitmap is sub-loaded into.
	GL_FrameBufferTextureInit();

	// init the palette texture
	GL_PalletteTextureInit();	
}

void GL_Cylindrix_Shutdown()
{
    glDeleteTextures( 1, &frameBufferTexture );
    glDeleteTextures( 1, &paletteTexture );
}

// clears the screen using the 0'th palette entry.
void GL_Clear()
{
	float r, g, b;

	// get clear color from palette.
	r = (float)(palette[0] & 0x000000ff) / 255.0f;
	g = (float)((palette[0] & 0x0000ff00) >> 8) / 255.0f;
	b = (float)((palette[0] & 0x00ff0000) >> 16) / 255.0f;

	// Clear all pixels.
	glClearColor( r, g, b, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// set up the palette texture
	GL_SubloadPaletteTexture();
}

// converts an intensity (0,1) & gradent into a texture coord for use in rendering.
float GL_IntensityToPaletteTexCoord( WorldStuff* worldStuff, float intensity, Gradient gradient )
{
	// look up gradient properties.
    unsigned char num_colors = worldStuff->color_info.gradient[gradient].num_colors;
    unsigned char offset = worldStuff->color_info.gradient[gradient].first;	
    unsigned int shade;
	
	// clamp intensity
	if ( intensity < 0 )	
		intensity = 0;
	else if ( intensity > 1 )
		intensity = 1;

	shade = offset + (intensity * (num_colors - 1));
	//shade = (offset + (num_colors - 1)) - ((intensity * (num_colors - 1)));

	// clamp shade
	if( shade < offset )
		shade = offset;
	else if ( shade > offset + (num_colors - 1) )
		shade = offset + (num_colors - 1);
	
	return ((float)shade + 0.5f) / 255.0f;
}

// draws the cylinder
void GL_DrawTube( WorldStuff* worldStuff, enum TubeType tubeType )
{
	int i, j;
	Float_Point pos;
	Float_Point v;
	Float_Point diff;
	float dot, len;
	float texCoord;
	float intensity;

	// set up the projection matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

	float aspect = (float)g_width/g_height;
	gluPerspective( 90.5f, aspect, 0.1f, 50.0f );

	// AJT: todo take tubeType into account.	

	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );

	// setup the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt( worldStuff->view_orientation.position[0], worldStuff->view_orientation.position[1], worldStuff->view_orientation.position[2],
			   worldStuff->view_orientation.position[0] + worldStuff->view_orientation.front[0], 
			   worldStuff->view_orientation.position[1] + worldStuff->view_orientation.front[1], 
			   worldStuff->view_orientation.position[2] + worldStuff->view_orientation.front[2],
			   worldStuff->view_orientation.up[0], worldStuff->view_orientation.up[1], worldStuff->view_orientation.up[2] );

	glBindTexture( GL_TEXTURE_2D, paletteTexture );
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	glColor3f( 1, 1, 1 );
	
	pos[0] = worldStuff->view_orientation.position[0];
	pos[1] = worldStuff->view_orientation.position[1];
	pos[2] = worldStuff->view_orientation.position[2];

	// draw the shizit.
	for ( i = 0; i < worldStuff->tube.faces; ++i )
	{
		glBegin( GL_POLYGON );
		for ( j = 0; j < worldStuff->tube.face[i].size; j++ )
		{
			// convert from fixed point
			v[0] = mtof( worldStuff->tube.point[worldStuff->tube.face[i].index[j]][0] );
			v[1] = mtof( worldStuff->tube.point[worldStuff->tube.face[i].index[j]][1] );
			v[2] = mtof( worldStuff->tube.point[worldStuff->tube.face[i].index[j]][2] );

			// figure out disance from v to pos.
			diff[0] = v[0] - pos[0];
			diff[1] = v[1] - pos[1];
			diff[2] = v[2] - pos[2];
			dot = (diff[0] * diff[0]) + (diff[1] * diff[1]) + (diff[2] * diff[2]);
			len = (float)sqrt( dot );

			// TODO: make this based on something (yon?)
			intensity = 1.0f - (len / 25.0f);
			texCoord = GL_IntensityToPaletteTexCoord( worldStuff, intensity, worldStuff->tube.face[i].gradient );
	
			glTexCoord2f( 0, texCoord );
			glVertex3f( v[0], v[1], v[2] );
		}
		glEnd();
	}
	glDisable( GL_TEXTURE_2D );		
	glFlush();		
	glDisable( GL_CULL_FACE );
}

// draws everything worldStuff->everything.
void GL_DrawEverything( WorldStuff* worldStuff )
{
	int i, j;
	Float_Point pos, eye, v, l, n, diff;
	float n_dot_l, len, temp;
	float texCoord;
	float intensity;

	// setup the modelview
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt( worldStuff->view_orientation.position[0], worldStuff->view_orientation.position[1], worldStuff->view_orientation.position[2],
		worldStuff->view_orientation.position[0] + worldStuff->view_orientation.front[0], worldStuff->view_orientation.position[1] + worldStuff->view_orientation.front[1], worldStuff->view_orientation.position[2] + worldStuff->view_orientation.front[2],
		worldStuff->view_orientation.up[0], worldStuff->view_orientation.up[1], worldStuff->view_orientation.up[2] );

	glBindTexture( GL_TEXTURE_2D, paletteTexture );
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	glColor3f( 1, 1, 1 );

	pos[0] = worldStuff->view_orientation.position[0];
	pos[1] = worldStuff->view_orientation.position[1];
	pos[2] = worldStuff->view_orientation.position[2];

	eye[0] = worldStuff->view_orientation.front[0];
	eye[1] = worldStuff->view_orientation.front[1];
	eye[2] = worldStuff->view_orientation.front[2];

	// draw the shizit.
	for ( i = 0; i < worldStuff->everything.faces; ++i )
	{				
		n[0] = mtof( worldStuff->everything.face[i].normal[0] );
		n[1] = mtof( worldStuff->everything.face[i].normal[1] );
		n[2] = mtof( worldStuff->everything.face[i].normal[2] );

		// normalize n
		temp = (float)sqrt( (n[0] * n[0]) + (n[1] * n[1]) + (n[2] * n[2]) );
		n[0] /= temp;
		n[1] /= temp;
		n[2] /= temp;

		glBegin( GL_POLYGON );
		for ( j = 0; j < worldStuff->everything.face[i].size; j++ )
		{
			// convert from fixed point
			v[0] = mtof( worldStuff->everything.point[worldStuff->everything.face[i].index[j]][0] );
			v[1] = mtof( worldStuff->everything.point[worldStuff->everything.face[i].index[j]][1] );
			v[2] = mtof( worldStuff->everything.point[worldStuff->everything.face[i].index[j]][2] );

			// figure out disance from v to pos.
			diff[0] = v[0] - pos[0];
			diff[1] = v[1] - pos[1];
			diff[2] = v[2] - pos[2];
			temp = (diff[0] * diff[0]) + (diff[1] * diff[1]) + (diff[2] * diff[2]);
			len = (float)sqrt( temp );

			// figure out the dot product between n and l.
			l[0] = pos[0] - v[0];
			l[1] = pos[1] - v[1];
			l[2] = pos[2] - v[2];
			temp = (float)sqrt( (l[0] * l[0]) + (l[1] * l[1]) + (l[2] * l[2]) );  // get length of l
			l[0] /= temp;  // normalize l
			l[1] /= temp;
			l[2] /= temp;
			n_dot_l = (n[0] * l[0]) + (n[1] * l[1]) + (n[2] * l[2]);  // n dot l

			// clamp n_dot_l
			if ( n_dot_l < 0 )
				n_dot_l = 0;
			else if ( n_dot_l > 1 )
				n_dot_l = 1;

			intensity = n_dot_l * (1.0f - (len / 25.0f));
						
			texCoord = GL_IntensityToPaletteTexCoord( worldStuff, intensity, worldStuff->everything.face[i].gradient );
	
			glTexCoord2f( 0, texCoord );
			glVertex3f( v[0], v[1], v[2] );
		}
		glEnd();		
	}
	glDisable( GL_TEXTURE_2D );
	glFlush();
}

// overlayes the contents of the framebuffer onto the screen, blended with the current contents.
// this is used to overlay the old software hud onto the new gl rendered scene.
void GL_SwapBuffers()
{
	palette[0] = 0x00000000;

	// ok we know the double_buffer has the hud drawn in it.
	makeFrameBuffer( double_buffer );

	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_RGBA, GL_UNSIGNED_BYTE, rgbaFrameBuffer );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();	
	float aspect = (float)g_width/g_height;
	glOrtho(-aspect, aspect, -1, 1, -1, 1);

    glMatrixMode( GL_MODELVIEW );	
    glLoadIdentity();

	glBindTexture( GL_TEXTURE_2D, frameBufferTexture );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glDisable(GL_DEPTH_TEST);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	float orig_aspect = 1.33333f; // 4:3
    glBegin( GL_POLYGON );
	glColor4f( 1, 1, 1, 1 );
	
	glTexCoord2f( 0, FBV );	
	glVertex3f( -orig_aspect, -1, 0 );

	glTexCoord2f( FBU, FBV );	
	glVertex3f( orig_aspect, -1, 0 );

	glTexCoord2f( FBU, 0 );	
	glVertex3f( orig_aspect, 1, 0 );

	glTexCoord2f( 0, 0 );	
	glVertex3f( -orig_aspect, 1, 0 );

	glEnd();

	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	SYS_SwapBuffers();

	++frame;
}


