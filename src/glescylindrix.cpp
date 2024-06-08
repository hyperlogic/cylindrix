// work in progress
#include <stdio.h>

#ifdef IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

static const char* g_vertShader = "\
#version 320 es\n\
precision highp float;\n\
uniform mat4 mat;\n\
in vec3 pos;\n\
in vec2 uv;\n\
out vec2 frag_uv;\n\
void main(void)\n\
{\n\
gl_Position = mat * vec4(pos, 1.0);\n\
frag_uv = uv;\n\
}\n";

static const char* g_fragShader = "\
#version 320 es\n\
precision highp float;\n\
uniform sampler2D tex;\n\
in vec2 frag_uv;\n\
out vec4 out_color;\n\
void main(void)\n\
{\n\
out_color = texture(tex, frag_uv);\n\
}\n";

GLuint g_prog;
GLuint g_uniform_mat;
GLuint g_uniform_tex;
GLuint g_attrib_pos;
GLuint g_attrib_uv;

GLuint g_quadVBO[2];

// converts the 8-bit double_buffer into the 32-bit rgbaFrameBuffer.
void makeFrameBuffer( unsigned char* double_buffer )
{
	int i;
	for ( i = 0; i < 64000; ++i )
	{
		rgbaFrameBuffer[i] = palette[ double_buffer[i] ];
	}
}

#define GL_CheckError()                           \
    do {                                          \
        GL_CheckErrorFunc(__FILE__, __LINE__);    \
	} while (0)

// for debugging
void GL_CheckErrorFunc(const char* file, int line)
{
	GLenum error = glGetError();

	switch ( error )
	{
		case GL_NO_ERROR:
			break;

		case GL_INVALID_ENUM:
            fprintf(stderr, "GL_CheckError : GL_INVALID_ENUM, %s:%d\n", file, line);
            assert(0);
            break;
		case GL_INVALID_VALUE:
            fprintf(stderr, "GL_CheckError : GL_INVALID_VALUE, %s:%d\n", file, line);
            assert(0);
            break;
		case GL_INVALID_OPERATION:
            fprintf(stderr, "GL_CheckError : GL_INVALID_OPERATION, %s:%d\n", file, line);
            assert(0);
            break;
		case GL_OUT_OF_MEMORY:
            fprintf(stderr, "GL_CheckError : GL_OUT_OF_MEMORY, %s:%d\n", file, line);
            assert(0);
            break;
		default:
            fprintf(stderr, "GL_CheckError : ???, %s:%d\n", file, line);
			assert(0);
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
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 512, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
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
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 1, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
}

// renders the frameBufferTexture on a quad that fills the screen.
static void GL_RenderBuffer()
{
	GL_CheckError();

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

	float aspect = (float)g_width / (float)g_height;
    //abaci::Matrixf proj = abaci::Matrixf::Ortho(-aspect, aspect, -1, 1, -1, 1);
    glm::mat4 proj = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    glUniformMatrix4fv(g_uniform_mat, 1, GL_FALSE, reinterpret_cast<float*>(&proj));
    glUniform1i(g_uniform_tex, 0);

	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    glBindBuffer(GL_ARRAY_BUFFER, g_quadVBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_quadVBO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

	SYS_SwapBuffers();

	GL_CheckError();

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

static GLint GL_CompileShader(const char* source, GLenum type)
{
	int size = strlen(source);
    GLint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, &size);
	glCompileShader(shader);

    GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
        fprintf(stderr, "error compiling shader, type = %d\n", type);
    }

    GLint bufferLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufferLen);
    if (bufferLen > 1)
    {
        if (success)
        {
            fprintf(stderr, "shader compilation warning!\n");
        }

        GLsizei len = 0;
        char* buffer = new char[bufferLen];
        glGetShaderInfoLog(shader, bufferLen, &len, buffer);
        fprintf(stderr, "%s\n", buffer);
        //DumpShaderSource(source);
        delete [] buffer;
    }
    assert(success);

    return shader;
}

static void GL_ProgramInit()
{
    GLint vertShader = GL_CompileShader(g_vertShader, GL_VERTEX_SHADER);
    GLint fragShader = GL_CompileShader(g_fragShader, GL_FRAGMENT_SHADER);
    g_prog = glCreateProgram();
    glAttachShader(g_prog, vertShader);
    glAttachShader(g_prog, fragShader);
    glLinkProgram(g_prog);

    GLint success;
    glGetProgramiv(g_prog, GL_LINK_STATUS, &success);
    if (!success)
    {
        fprintf(stderr, "error linking program\n");
    }
    assert(success);

    // save uniform locs
    g_uniform_mat = glGetUniformLocation(g_prog, "mat");
    g_uniform_tex = glGetUniformLocation(g_prog, "tex");

    // save attrib locs
    g_attrib_pos = glGetAttribLocation(g_prog, "pos");
    g_attrib_uv = glGetAttribLocation(g_prog, "uv");

    glUseProgram(g_prog);

    glActiveTexture(GL_TEXTURE0);

	GL_CheckError();
}

static void GL_VBOInit()
{
    glGenBuffers(2, g_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_quadVBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_quadVBO[1]);

    glEnableVertexAttribArray(g_attrib_pos);
    glEnableVertexAttribArray(g_attrib_uv);

    const float orig_aspect = 1.33333f; // 4:3
    static float verts[] = {
        -orig_aspect, -1, 0, 0, FBV,
         orig_aspect, -1, 0, FBU, FBV,
         orig_aspect,  1, 0, FBU, 0,
        -orig_aspect,  1, 0, 0, 0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * 4, verts, GL_STATIC_DRAW);

    static unsigned short indices[] = {0, 1, 3, 1, 2, 3};

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 6, indices, GL_STATIC_DRAW);

    const GLsizei stride = sizeof(float) * 5;
    glVertexAttribPointer(g_attrib_pos, 3, GL_FLOAT, GL_FALSE, stride, (float*)0);
    glVertexAttribPointer(g_attrib_uv, 2, GL_FLOAT, GL_FALSE, stride, (float*)0 + 3);
}

// creates the framebuffer & palette textures.
void GL_Cylindrix_Init()
{
    // initialize the glprogram
	GL_ProgramInit();
    GL_CheckError();

	// initialize some vbos
    GL_VBOInit();
    GL_CheckError();

	// initialize the texture used that the game bitmap is sub-loaded into.
	GL_FrameBufferTextureInit();
    GL_CheckError();

	// init the palette texture
	GL_PalletteTextureInit();
    GL_CheckError();
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

    // AJT: GLES2
    /*
	// set up the projection matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    */

	//float aspect = (float)g_width/g_height;

    // AJT: GLES2
    /*
	gluPerspective( 90.5f, aspect, 0.1f, 50.0f );
    */

	// AJT: todo take tubeType into account.
    // AJT: GLES2
    /*
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
    */

	glBindTexture( GL_TEXTURE_2D, paletteTexture );
	glEnable( GL_TEXTURE_2D );

    // AJT: GLES2
    /*
	glDisable( GL_LIGHTING );
	glColor3f( 1, 1, 1 );
    */

	pos[0] = worldStuff->view_orientation.position[0];
	pos[1] = worldStuff->view_orientation.position[1];
	pos[2] = worldStuff->view_orientation.position[2];

	// draw the shizit.
	for ( i = 0; i < worldStuff->tube.faces; ++i )
	{
        // AJT: GLES2
		//glBegin( GL_POLYGON );
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

            // AJT: GLES2
			//glTexCoord2f( 0, texCoord );
			//glVertex3f( v[0], v[1], v[2] );
		}
        // AJT: GLES2
		//glEnd();
	}

    // AJT: GLES2
    /*
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
    */
}

// draws everything worldStuff->everything.
void GL_DrawEverything( WorldStuff* worldStuff )
{
	int i, j;
	Float_Point pos, eye, v, l, n, diff;
	float n_dot_l, len, temp;
	float texCoord;
	float intensity;

    // AJT: GLES2
    /*
	// setup the modelview
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt( worldStuff->view_orientation.position[0], worldStuff->view_orientation.position[1], worldStuff->view_orientation.position[2],
		worldStuff->view_orientation.position[0] + worldStuff->view_orientation.front[0], worldStuff->view_orientation.position[1] + worldStuff->view_orientation.front[1], worldStuff->view_orientation.position[2] + worldStuff->view_orientation.front[2],
		worldStuff->view_orientation.up[0], worldStuff->view_orientation.up[1], worldStuff->view_orientation.up[2] );
    */

	glBindTexture( GL_TEXTURE_2D, paletteTexture );
	glEnable( GL_TEXTURE_2D );

    // AJT: GLES2
    /*
	glDisable( GL_LIGHTING );
	glColor3f( 1, 1, 1 );
    */

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

        // AJT: GLES2
		//glBegin( GL_POLYGON );
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

            // AJT: GLES2
            /*
			glTexCoord2f( 0, texCoord );
			glVertex3f( v[0], v[1], v[2] );
            */
		}
        // AJT: GLES2
		//glEnd();
	}
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

	float aspect = (float)g_width / (float)g_height;
    //abaci::Matrixf proj = abaci::Matrixf::Ortho(-aspect, aspect, -1, 1, -1, 1);
    glm::mat4 proj = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    glUniformMatrix4fv(g_uniform_mat, 1, GL_FALSE, reinterpret_cast<float*>(&proj));
    glUniform1i(g_uniform_tex, 0);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDisable(GL_DEPTH_TEST);

	glBindTexture( GL_TEXTURE_2D, frameBufferTexture );
    glBindBuffer(GL_ARRAY_BUFFER, g_quadVBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_quadVBO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

	glDisable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	SYS_SwapBuffers();

printf("GL_SwapBuffers()\n");

	++frame;
}


