// OpenGL  base code
// updated: september 2002

// winmain.h
// from:

// www.thepixels.net
// by:    Greg Damon
//        gregd@thepixels.net
// --------------------------

#define WINDOWED		0					// predefined flags for initialization
#define FULLSCREEN		1

#define SCRWIDTH		640					// width of the window
#define SCRHEIGHT		480					// height of the window
#define SCRDEPTH		16					// bit depth

#define WNDCLASSNAME	"GLClass"			// window class name
#define WNDNAME			"glCylindrix"	// string that will appear in the title bar

extern HDC hdc;								// device context handle
extern HGLRC hglrc;							// OpenGL rendering context
extern HWND hwnd;							// window handle

extern int screenw;							// when window is resized, the new dimensions...
extern int screenh;							// ...are stored in these variables
extern int screenmode;						// FULLSCREEN or WINDOWED?

extern int quit;							// indicates the state of application

extern indexPixelFormat;					// number of available pixel formats

void CreateWnd (HINSTANCE hinst, int posx, int posy, int width, int height, int depth, int type);
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);