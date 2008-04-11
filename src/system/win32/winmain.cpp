#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <ctype.h>
#include "..\system.h"

#define WINDOWED		0					// predefined flags for initialization
#define FULLSCREEN		1

#define SCRWIDTH		640					// width of the window
#define SCRHEIGHT		480					// height of the window
#define SCRDEPTH		16					// bit depth

#define WNDCLASSNAME	gameName			// window class name
#define WNDNAME			gameName			// string that will appear in the title bar

HDC hdc;									// device context handle
HGLRC hglrc;								// OpenGL rendering context
HWND hwnd;									// window handle

int screenw = SCRWIDTH;						// when window is resized, the new dimensions...
int screenh = SCRHEIGHT;					// ...are stored in these variables
int screenmode = 0;							// FULLSCREEN or WINDOWED?

int quit = FALSE;							// indicates the state of application

int indexPixelFormat;						// number of available pixel formats

unsigned char keysDown[255];
unsigned char keysPressed[255];

// SysShutdown: shutdowns the application and releases memory
void SysShutdown();

// SysSetDisplayMode: sets display mode
void SysSetDisplayMode( int width, int height, int depth );

// SysRecoverDisplayMode: switch back to default resolution
void SysRecoverDisplayMode();

void SysShutdown()
{
	// release device context in use by rc
	wglMakeCurrent( hdc, NULL );

	// delete rendering context
	wglDeleteContext( hglrc );

	// if FULLSCREEN, change back to original resolution
	if ( screenmode == FULLSCREEN )	
		SysRecoverDisplayMode();
}


void SysSetDisplayMode( int width, int height, int depth )
{	
	DEVMODE dmode;
	
	memset( &dmode, 0, sizeof(DEVMODE) );
	dmode.dmSize = sizeof(DEVMODE);
	dmode.dmPelsWidth = width;
	dmode.dmPelsHeight = height;
	dmode.dmBitsPerPel = depth;
	dmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	
	// change resolution, if possible
	if ( ChangeDisplaySettings( &dmode, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
	{
		// if not... failed to change resolution

		screenmode = 0;	// this prevents SysShutdown from changing resolution back

		MessageBox( NULL, "Your system doesn't support this screen resolution", "Display Error", MB_OK );
		SysShutdown();
	}
}


void SysRecoverDisplayMode()
{
	ChangeDisplaySettings( NULL, 0 );
}



// CreateWnd: creates either a "full-screen" or a "windowed" window according to the flag - type
//            type = 0: windowed
//			  type = 1: full-screen

void CreateWnd( HINSTANCE hinst, int width, int height, int depth, int type )
{	
	// center position of the window
	int posx = (GetSystemMetrics( SM_CXSCREEN ) / 2) - (width / 2);
	int posy = (GetSystemMetrics( SM_CYSCREEN ) / 2) - (height / 2);

	// set up the window for a windowed application by default
	DWORD wndStyle = WS_OVERLAPPEDWINDOW;
	screenmode = WINDOWED;

	if ( type == FULLSCREEN )	// create a full-screen application if requested
	{
		wndStyle = WS_POPUP;
		screenmode = FULLSCREEN;

		posx = 0;
		posy = 0;

		// change resolution before the window is created
		SysSetDisplayMode( width, height, depth );
	}

	// create the window
	hwnd = CreateWindowEx( 0,
						   WNDCLASSNAME,
						   WNDNAME,
						   wndStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						   posx, posy,
						   width, height,
						   NULL,
						   NULL,
						   hinst,
						   NULL );

	// at this point WM_CREATE message is sent/received
	// the WM_CREATE branch inside WinProc function will execute here
}

void SetGLFormat()
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			SCRDEPTH,
			0,0,0,0,0,0,0,0,0,0,0,0,0, // useles parameters
			16,
			0,0,PFD_MAIN_PLANE,0,0,0,0
	};
	
	// Choose the closest pixel format available
	if ( !(indexPixelFormat = ChoosePixelFormat( hdc, &pfd )) )
	{
		MessageBox(hwnd, "Failed to find pixel format", "Pixel Format Error", MB_OK);
		SysShutdown();
	}
	
	// Set the pixel format for the provided window DC
	if ( !SetPixelFormat( hdc, indexPixelFormat, &pfd ) )
	{
		MessageBox( hwnd, "Failed to Set Pixel Format", "Pixel Format Error", MB_OK );
		SysShutdown();
	}
}

void Resize( int width, int height )
{
	if ( height <= 0 )
		height = 1;

	glViewport( 0, 0, width, height );

	glMatrixMode( GL_PROJECTION );
	
	glLoadIdentity();

	gluPerspective( 45.0f, (float)width / (float)height, 0.2f, 255.0f );

	glMatrixMode( GL_MODELVIEW );

	glLoadIdentity();
}

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

void SetVSync( int interval )
{
	const char *extensions = (const char*)glGetString( GL_EXTENSIONS );

	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		if( wglSwapIntervalEXT )
			wglSwapIntervalEXT(interval);
	}
}

// InitOpenGL: initializes OpenGL; resize projection and other setup

void InitOpenGL()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0f );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	glShadeModel( GL_SMOOTH );

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
}


unsigned char VirtualKeyToSysKey( DWORD vKey )
{
	switch ( vKey )
	{
		case '0': case '1':	case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9':
			return KEY_0 + ((unsigned char)vKey - '0');
			break;
		case 'A': case 'B': case 'C': case 'D': case 'E':
		case 'F': case 'G': case 'H': case 'I': case 'J':
		case 'K': case 'L': case 'M': case 'N': case 'O':
		case 'P': case 'Q': case 'R': case 'S': case 'T':
		case 'U': case 'V': case 'W': case 'X': case 'Y':
		case 'Z':
			return KEY_A + ((unsigned char)vKey - 'A');
			break;
		case VK_UP:
			return KEY_UP_ARROW;
			break;
		case VK_DOWN:
			return KEY_DOWN_ARROW;
			break;
		case VK_LEFT:
			return KEY_LEFT_ARROW;
			break;
		case VK_RIGHT:
			return KEY_RIGHT_ARROW;
			break;
		case VK_SPACE:
			return KEY_SPACE;
			break;
		case VK_CONTROL:
			return KEY_CONTROL;
			break;
		case VK_MENU:
			return KEY_ALT;
			break;
		case VK_F1:
			return KEY_F1;
			break;
		case VK_F2:
			return KEY_F2;
			break;
		case VK_F3:
			return KEY_F3;
			break;
		case VK_F4:
			return KEY_F4;
			break;
		case VK_F5:
			return KEY_F5;
			break;
		case VK_F6:
			return KEY_F6;
			break;
		case VK_F7:
			return KEY_F7;
			break;
		case VK_F8:
			return KEY_F8;
			break;
		case VK_F9:
			return KEY_F9;
			break;
		case VK_F10:
			return KEY_F10;
			break;
		case VK_F11:
			return KEY_F11;
			break;
		case VK_F12:
			return KEY_F12;
			break;
		case VK_RETURN:
			return KEY_ENTER;
			break;
		case '-':
			return KEY_MINUS;
			break;
		case '=':
			return KEY_EQUALS;
			break;
		case '[':
			return KEY_LEFT_BRACKET;
			break;
		case ']':
			return KEY_RIGHT_BRACKET;
			break;
		case '\\':
			return KEY_BACKSLASH;
			break;
		case ';':
			return KEY_SEMICOLON;
			break;
		case '\"':
			return KEY_APOSTROPHE;
			break;
		case ',':
			return KEY_COMMA;
			break;
		case '.':
			return KEY_PERIOD;
			break;
		case '/':
			return KEY_SLASH;
			break;
		case '~':
			return KEY_TILDE;
			break;
		case VK_TAB:
			return KEY_TAB;
			break;
		case VK_SHIFT:
			return KEY_SHIFT;
			break;
		case VK_BACK:
			return KEY_BACKSPACE;
			break;
		case VK_ESCAPE:
			return KEY_ESC;
			break;
		default:
			return 0xff;
			break;
	}
}

//*=====================
//  The event handler
//*=====================

LRESULT CALLBACK WinProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	unsigned char key;
	switch ( msg )
	{
		case WM_CREATE:
			if ( (hdc = GetDC(hwnd)) == NULL )				// get device context
			{
				MessageBox( hwnd, "Failed to Get the Window Device Context", "Device Context Error", MB_OK );
				SysShutdown();
				break;
			}

			SetGLFormat();					// select pixel format, needed before wglCreateContext call
			
			if ( (hglrc = wglCreateContext( hdc )) == NULL )	// create the rendering context
			{
				MessageBox( hwnd, "Failed to Create the OpenGL Rendering Context", "OpenGL Rendering Context Error", MB_OK );
				SysShutdown();
				break;
			}
			
			if ( wglMakeCurrent( hdc, hglrc ) == FALSE)		// make hglrc current rc
			{
				MessageBox( hwnd, "Failed to Make OpenGL Rendering Context Current", "OpenGL Rendering Context Error", MB_OK );
				SysShutdown();					
			}

			Resize( SCRWIDTH, SCRHEIGHT );

			SetVSync( 0 );  // turn off vsync.

			InitOpenGL();					// initialize OpenGL before showing the window

			ShowWindow( hwnd, SW_SHOW );	// everything went OK, show the window
			UpdateWindow( hwnd );
			break;			

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
	
		case WM_SIZE:
			// resize the viewport after the window had been resized
			Resize( LOWORD( lparam ), HIWORD( lparam ) );
			
			screenw = LOWORD( lparam );
			screenh = HIWORD( lparam );			
			break;			

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			key = VirtualKeyToSysKey( wparam );
			keysDown[key] = 1;
			keysPressed[key] = 1; 
			break;			

		case WM_KEYUP:
		case WM_SYSKEYUP:
			key = VirtualKeyToSysKey( wparam );
			keysDown[key] = 0;
			break;
	}

	return DefWindowProc( hwnd, msg, wparam, lparam );
}

void RenderFrame()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

//*=====================
//  WinMain
//*=====================

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd )
{
	MSG msg;
	WNDCLASSEX ex;
	int argc;
	char cmdline[512];
	const char* argv[32];
	char* tok;

	// Convert command line into C-style argc & argv.
	strcpy( cmdline, lpcmdline );
	argv[0] = "cylindrix";
	argc = 1;
	tok = strtok( cmdline, " \t" );
	while ( tok )
	{
		argv[argc++] = tok;		
		tok = strtok( 0, " \t" );
	}

	ex.cbSize = sizeof(WNDCLASSEX);
	ex.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	ex.lpfnWndProc = WinProc;
	ex.cbClsExtra = 0;
	ex.cbWndExtra = 0;
	ex.hInstance = hinstance;
	ex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	ex.hCursor = LoadCursor(NULL, IDC_ARROW);
	ex.hbrBackground = NULL;
	ex.lpszMenuName = NULL;
	ex.lpszClassName = WNDCLASSNAME;
	ex.hIconSm = NULL;

	if ( !RegisterClassEx( &ex ) )
	{
		MessageBox( NULL, "Failed to register the window class", "Window Reg Error", MB_OK );
		return 1;
	}

	// Create the window
	CreateWnd( hinstance, screenw, screenh, SCRDEPTH, WINDOWED );

	GameInit( argc, argv );

	// The message loop
	while ( !quit )
	{
		quit = GameLoop();

		// clear keys pressed.
		memset( keysPressed, 0, 255 );

		SwapBuffers( hdc );

		if ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			if (msg.message == WM_QUIT)
				quit = TRUE;

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	
	GameShutdown();
	SysShutdown();

	return msg.lParam;
}
