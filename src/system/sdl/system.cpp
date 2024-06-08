#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
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

int g_width = 0;
int g_height = 0;
int g_bpp = 0;
bool g_fullscreen = false;
SDL_Window* g_window = NULL;
SDL_GLContext g_gl_context;

void SYS_SwapBuffers()
{
    SDL_GL_SwapWindow(g_window);
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

static SDL_Scancode keyToScancode[SIZEOFKEYTABLE] =
{
    SDL_SCANCODE_A, // KEY_A
    SDL_SCANCODE_B, // KEY_B
    SDL_SCANCODE_C, // KEY_C
    SDL_SCANCODE_D, // KEY_D
    SDL_SCANCODE_E, // KEY_E
    SDL_SCANCODE_F, // KEY_F
    SDL_SCANCODE_G, // KEY_G
    SDL_SCANCODE_H, // KEY_H
    SDL_SCANCODE_I, // KEY_I
    SDL_SCANCODE_J, // KEY_J,
    SDL_SCANCODE_K, // KEY_K,
    SDL_SCANCODE_L, // KEY_L,
    SDL_SCANCODE_M, // KEY_M,
    SDL_SCANCODE_N, // KEY_N,
    SDL_SCANCODE_O, // KEY_O,
    SDL_SCANCODE_P, // KEY_P,
    SDL_SCANCODE_Q, // KEY_Q,
    SDL_SCANCODE_R, // KEY_R,
    SDL_SCANCODE_S, // KEY_S,
    SDL_SCANCODE_T, // KEY_T,
    SDL_SCANCODE_U, // KEY_U,
    SDL_SCANCODE_V, // KEY_V,
    SDL_SCANCODE_W, // KEY_W,
    SDL_SCANCODE_X, // KEY_X,
    SDL_SCANCODE_Y, // KEY_Y,
    SDL_SCANCODE_Z, // KEY_Z,
    SDL_SCANCODE_UP, // KEY_UP_ARROW,
    SDL_SCANCODE_DOWN, // KEY_DOWN_ARROW,
    SDL_SCANCODE_LEFT, // KEY_LEFT_ARROW,
    SDL_SCANCODE_RIGHT, // KEY_RIGHT_ARROW,
    SDL_SCANCODE_SPACE, // KEY_SPACE,
    SDL_SCANCODE_LCTRL, // KEY_CONTROL,
    SDL_SCANCODE_LALT, // KEY_ALT,
    SDL_SCANCODE_F1, // KEY_F1,
    SDL_SCANCODE_F2, // KEY_F2,
    SDL_SCANCODE_F3, // KEY_F3,
    SDL_SCANCODE_F4, // KEY_F4,
    SDL_SCANCODE_F5, // KEY_F5,
    SDL_SCANCODE_F6, // KEY_F6,
    SDL_SCANCODE_F7, // KEY_F7,
    SDL_SCANCODE_F8, // KEY_F8,
    SDL_SCANCODE_F9, // KEY_F9,
    SDL_SCANCODE_F10, // KEY_F10,
    SDL_SCANCODE_F11, // KEY_F11,
    SDL_SCANCODE_F12, // KEY_F12,
    SDL_SCANCODE_RETURN, // KEY_ENTER,
    SDL_SCANCODE_0, // KEY_0,
    SDL_SCANCODE_1, // KEY_1,
    SDL_SCANCODE_2, // KEY_2,
    SDL_SCANCODE_3, // KEY_3,
    SDL_SCANCODE_4, // KEY_4,
    SDL_SCANCODE_5, // KEY_5,
    SDL_SCANCODE_6, // KEY_6,
    SDL_SCANCODE_7, // KEY_7,
    SDL_SCANCODE_8, // KEY_8,
    SDL_SCANCODE_9, // KEY_9,
    SDL_SCANCODE_MINUS, // KEY_MINUS,
    SDL_SCANCODE_EQUALS, // KEY_EQUALS,
    SDL_SCANCODE_LEFTBRACKET, // KEY_LEFT_BRACKET,
    SDL_SCANCODE_RIGHTBRACKET, // KEY_RIGHT_BRACKET,
    SDL_SCANCODE_BACKSLASH, // KEY_BACKSLASH,
    SDL_SCANCODE_SEMICOLON, // KEY_SEMICOLON,
    SDL_SCANCODE_APOSTROPHE, // KEY_APOSTROPHE,
    SDL_SCANCODE_COMMA, // KEY_COMMA,
    SDL_SCANCODE_PERIOD, // KEY_PERIOD,
    SDL_SCANCODE_SLASH, // KEY_SLASH,
    SDL_SCANCODE_GRAVE, // KEY_TILDE,
    SDL_SCANCODE_TAB, // KEY_TAB,
    SDL_SCANCODE_LSHIFT, // KEY_SHIFT,
    SDL_SCANCODE_BACKSPACE, // KEY_BACKSPACE,
    SDL_SCANCODE_ESCAPE // KEY_ESC,
};

static void processKeys()
{
    const Uint8* keys = SDL_GetKeyboardState(0);

    assert(keys);

    for ( int i = 0; i < SIZEOFKEYTABLE; ++i )
    {
        SDL_Scancode scanCode = keyToScancode[i];

        if ( keys[scanCode] )
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
}

static bool process()
{
    processKeys();
    SYS_ProcessSound();
    bool done = GameLoop();

    return done;
}

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

        /* AJT: OLD SDL1 code
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
        */
        }
    }

    if ( !done )
    {
        done = process();
    }

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
    const char* error = nullptr;

    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) < 0 )
    {
        SYS_Error( "Couldn't init SDL!\n" );
        // TODO: bug in emscripten sdl.
        // report sdl errors
        error = SDL_GetError();
        if (error[0] != 0)
            printf("SDL_Init() failed! SDL_Error = %s\n", error);
    }

    setenv("__GL_SYNC_TO_VBLANK", "1", true);
    g_fullscreen = false;

    /* AJT: OLD SDL1 code
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
        //g_width = 1024; g_height = 768;  // 4:3
        g_width = 320 * 2; g_height = 240 * 2;
//      g_width = 1280; g_height = 720;  // 16:9
        g_bpp = videoInfo->vfmt->BitsPerPixel;
        g_screen = SDL_SetVideoMode(g_width, g_height, g_bpp, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF);
    }

    if ( !g_screen )
        SYS_Error( "Couldn't create SDL screen!\n" );
    */

    uint32_t windowFlags = SDL_WINDOW_OPENGL;
    if (g_fullscreen)
    {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    else
    {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }
    g_width = 320 * 2;
    g_height = 240 * 2;
    g_window = SDL_CreateWindow("splatapult", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_width, g_height, windowFlags);

    if (!g_window)
    {
        // report sdl errors
        error = SDL_GetError();
        if (error[0] != 0)
            printf("CreateWindow! SDL_Error = %s\n", error);
    }

    g_gl_context = SDL_GL_CreateContext(g_window);
    SDL_GL_MakeCurrent(g_window, g_gl_context);

    GameInit( argc, const_cast<const char**>(argv) );

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
