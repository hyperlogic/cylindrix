#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <SDL/SDL.h>
#include "../../gameloop.h"
#include "../system.h"
#include <stdarg.h>

#ifdef BROWSER
#include <emscripten.h>
#endif

#ifdef DARWIN
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

unsigned char keysDown[255];
unsigned char keysPressed[255];

void SYS_SwapBuffers()
{
    SDL_GL_SwapBuffers();
}

void SYS_TimerInit()
{

}

float SYS_GetTimeSeconds()
{
    unsigned int now = SDL_GetTicks();  // milliseconds
    return now / 1000.0f;   // convert to seconds.
}

unsigned long SYS_GetTimeMS()
{
    return SYS_GetTimeSeconds() * 1000.0f;
}

unsigned long SYS_GetTimeMicro()
{
    return SYS_GetTimeSeconds() * 100000.0f;
}

// keyboard input
void SYS_InitKeyboard()
{
    memset( keysDown, 0, 255 );
    memset( keysPressed, 0, 255 );
}

// returns 1 if the given key is currently held down.
int SYS_KeyDown( unsigned char key )
{
    return keysDown[key];
}

// returns 1 if the given key has just been pressed, will return repeats, useful for typing.
int SYS_KeyPressed( unsigned char key )
{
    return keysPressed[key];
}

static SDLKey keyToSDLKey[SIZEOFKEYTABLE] =
{
    SDLK_a, // KEY_A
    SDLK_b, // KEY_B
    SDLK_c, // KEY_C
    SDLK_d, // KEY_D
    SDLK_e, // KEY_E
    SDLK_f, // KEY_F
    SDLK_g, // KEY_G
    SDLK_h, // KEY_H
    SDLK_i, // KEY_I
    SDLK_j, // KEY_J,
    SDLK_k, // KEY_K,
    SDLK_l, // KEY_L,
    SDLK_m, // KEY_M,
    SDLK_n, // KEY_N,
    SDLK_o, // KEY_O,
    SDLK_p, // KEY_P,
    SDLK_q, // KEY_Q,
    SDLK_r, // KEY_R,
    SDLK_s, // KEY_S,
    SDLK_t, // KEY_T,
    SDLK_u, // KEY_U,
    SDLK_v, // KEY_V,
    SDLK_w, // KEY_W,
    SDLK_x, // KEY_X,
    SDLK_y, // KEY_Y,
    SDLK_z, // KEY_Z,
    SDLK_UP, // KEY_UP_ARROW,
    SDLK_DOWN, // KEY_DOWN_ARROW,
    SDLK_LEFT, // KEY_LEFT_ARROW,
    SDLK_RIGHT, // KEY_RIGHT_ARROW,
    SDLK_SPACE, // KEY_SPACE,
    SDLK_LCTRL, // KEY_CONTROL,
    SDLK_LALT, // KEY_ALT,
    SDLK_F1, // KEY_F1,
    SDLK_F2, // KEY_F2,
    SDLK_F3, // KEY_F3,
    SDLK_F4, // KEY_F4,
    SDLK_F5, // KEY_F5,
    SDLK_F6, // KEY_F6,
    SDLK_F7, // KEY_F7,
    SDLK_F8, // KEY_F8,
    SDLK_F9, // KEY_F9,
    SDLK_F10, // KEY_F10,
    SDLK_F11, // KEY_F11,
    SDLK_F12, // KEY_F12,
    SDLK_RETURN, // KEY_ENTER,
    SDLK_0, // KEY_0,
    SDLK_1, // KEY_1,
    SDLK_2, // KEY_2,
    SDLK_3, // KEY_3,
    SDLK_4, // KEY_4,
    SDLK_5, // KEY_5,
    SDLK_6, // KEY_6,
    SDLK_7, // KEY_7,
    SDLK_8, // KEY_8,
    SDLK_9, // KEY_9,
    SDLK_MINUS, // KEY_MINUS,
    SDLK_EQUALS, // KEY_EQUALS,
    SDLK_LEFTBRACKET, // KEY_LEFT_BRACKET,
    SDLK_RIGHTBRACKET, // KEY_RIGHT_BRACKET,
    SDLK_BACKSLASH, // KEY_BACKSLASH,
    SDLK_SEMICOLON, // KEY_SEMICOLON,
    SDLK_QUOTE, // KEY_APOSTROPHE,
    SDLK_COMMA, // KEY_COMMA,
    SDLK_PERIOD, // KEY_PERIOD,
    SDLK_SLASH, // KEY_SLASH,
    SDLK_BACKQUOTE, // KEY_TILDE,
    SDLK_TAB, // KEY_TAB,
    SDLK_LSHIFT, // KEY_SHIFT,
    SDLK_BACKSPACE, // KEY_BACKSPACE,
    SDLK_ESCAPE // KEY_ESC,
};

static void processKeys()
{
#ifndef BROWSER
    const Uint8* keys = SDL_GetKeyState(0);

    for ( int i = 0; i < SIZEOFKEYTABLE; ++i )
    {
        SDLKey sdlKey = keyToSDLKey[i];

        if ( keys[sdlKey] )
        {
            keysPressed[i] = keysDown[i] ? 0 : 1;
            keysDown[i] = 1;
        }
        else
        {
            keysPressed[i] = 0;
            keysDown[i] = 0;
        }
    }
#endif
}

static bool process()
{
    processKeys();
    SYS_ProcessSound();
    bool done = GameLoop();

    return done;
}

int g_width = 0;
int g_height = 0;
int g_bpp = 0;
bool g_fullscreen = false;
SDL_Surface* g_screen = NULL;

static bool event_loop()
{
    bool done = false;
    SDL_Event event;
    while ( SDL_PollEvent( &event ) )
    {
        switch ( event.type )
        {
        case SDL_QUIT:
            done = true;
            break;

        case SDL_VIDEORESIZE:
            if (!g_fullscreen)
            {
                g_width = event.resize.w;
                g_height = event.resize.h;
                glViewport(0, 0, g_width, g_height);
                g_screen = SDL_SetVideoMode( event.resize.w, event.resize.h, g_bpp, SDL_HWSURFACE | SDL_RESIZABLE | SDL_OPENGL );
            }
            printf("SDL_VIDEORESIZE: (%d, %d)\n", event.resize.w, event.resize.h);
            break;
        }
    }

    if ( !done )
    {
        done = process();
    }

#ifndef BROWSER
    // TODO: bug in emscripten sdl
    // report sdl errors
    const char* error = SDL_GetError();
    if (error[0] != 0)
        printf("SDL_Error = %s\n", error);
#endif

    return done;
} 

#ifdef BROWSER
static void browser_loop()
{
    event_loop();
}
#endif

int main( int argc, char* argv[] )
{
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
        SYS_Error( "Couldn't init SDL!\n" );

    SDL_WM_SetCaption("cylindrix", "cylindrix");

    putenv("__GL_SYNC_TO_VBLANK=1");
    g_fullscreen = false;

    // Get the current desktop width & height
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();

    if (g_fullscreen)
    {
        // fullscreen but dont change video mode.
        g_width = videoInfo->current_w;
        g_height = videoInfo->current_h;
        g_bpp = videoInfo->vfmt->BitsPerPixel;

        g_screen = SDL_SetVideoMode(g_width, g_height, g_bpp, SDL_HWSURFACE | SDL_OPENGL | SDL_FULLSCREEN | SDL_DOUBLEBUF);
    }
    else
    {
        // TODO: get this from a config file or something...
        g_width = 1024; g_height = 768;  // 4:3
//      g_width = 1280; g_height = 720;  // 16:9
        g_bpp = videoInfo->vfmt->BitsPerPixel;
        g_screen = SDL_SetVideoMode(g_width, g_height, g_bpp, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF /*| SDL_RESIZABLE*/);
    }

    if ( !g_screen )
        SYS_Error( "Couldn't create SDL screen!\n" );

    GameInit( argc, const_cast<const char**>(argv) );

#ifndef BROWSER
    // TODO: bug in emscripten sdl.
    // report sdl errors
    const char* error = SDL_GetError();
    if (error[0] != 0)
        printf("SDL_Error = %s\n", error);
#endif

#ifndef BROWSER
    bool done = false;
    while ( !done )
    {
        done = event_loop();
    }
    GameShutdown();
    SDL_Quit();

    return 0;
#else
    emscripten_set_main_loop(browser_loop, 30, false);
    return 0;
#endif
}
