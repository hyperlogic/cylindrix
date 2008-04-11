// OpenGL  base code
// updated: september 2002

// winmain.cpp - window initialization and main loop
// from:

// www.thepixels.net
// by:    Greg Damon
//        gregd@thepixels.net
// --------------------------

#include <windows.h>

#include <gl/gl.h>			// OpenGL headers
#include <gl/glu.h>

#include "winmain.h"

HDC hdc;
HWND hwnd;
HGLRC hglrc;

int screenw = SCRWIDTH;
int screenh = SCRHEIGHT;
int screenmode = 0;

int quit = FALSE;

int indexPixelFormat = 0;

// CreateWnd: creates either a "full-screen" or a "windowed" window according to the flag - type
//            type = 0: windowed
//			  type = 1: full-screen

void CreateWnd (HINSTANCE hinst, int posx, int posy, int width, int height, int depth, int type)
{	
	// center position of the window
	//int posx = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
	//int posy = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);

	// set up the window for a windowed application by default
	long wndStyle = WS_OVERLAPPEDWINDOW;
	screenmode = WINDOWED;

	if (type == FULLSCREEN)	// create a full-screen application if requested
	{
		wndStyle = WS_POPUP;
		screenmode = FULLSCREEN;

		posx = 0;
		posy = 0;

		// change resolution before the window is created
		SysSetDisplayMode(screenw, screenh, SCRDEPTH);
	}

	// create the window
	hwnd = CreateWindowEx(NULL,
						  WNDCLASSNAME,
						  WNDNAME,
						  wndStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
						  posx, posy,
						  width, height,
						  NULL,
						  NULL,
						  hinst,
						  NULL);

	// at this point WM_CREATE message is sent/received
	// the WM_CREATE branch inside WinProc function will execute here
}

void SetGLFormat (void)
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
	if (!(indexPixelFormat = ChoosePixelFormat(hdc, &pfd)))
	{
		MessageBox(hwnd, "Failed to find pixel format", "Pixel Format Error", MB_OK);
		SysShutdown();
	}
	
	// Set the pixel format for the provided window DC
	if ( !SetPixelFormat(hdc, indexPixelFormat, &pfd) )
	{
		MessageBox(hwnd, "Failed to Set Pixel Format", "Pixel Format Error", MB_OK);
		SysShutdown();
	}
}

void Resize (int width, int height)
{
	if (height <= 0)
		height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();

	gluPerspective(45.0f, (float)width/(float)height, 0.2f, 255.0f);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}

// InitOpenGL: initializes OpenGL; resize projection and other setup

void InitOpenGL (void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}

//*=====================
//  The event handler
//*=====================

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_CREATE:
			{
				if ((hdc = GetDC(hwnd)) == NULL)				// get device context
				{
					MessageBox(hwnd, "Failed to Get the Window Device Context", "Device Context Error", MB_OK);
					SysShutdown();
					break;
				}

				SetGLFormat();					// select pixel format, needed before wglCreateContext call
				
				if ((hglrc = wglCreateContext(hdc)) == NULL)	// create the rendering context
				{
					MessageBox(hwnd, "Failed to Create the OpenGL Rendering Context", "OpenGL Rendering Context Error", MB_OK);
					SysShutdown();
					break;
				}
				
				if ((wglMakeCurrent(hdc, hglrc)) == FALSE)		// make hglrc current rc
				{
					MessageBox(hwnd, "Failed to Make OpenGL Rendering Context Current", "OpenGL Rendering Context Error", MB_OK);
					SysShutdown();					
				}

				Resize(SCRWIDTH, SCRHEIGHT);

				InitOpenGL();					// initialize OpenGL before showing the window

				ShowWindow(hwnd, SW_SHOW);		// everything went OK, show the window
				UpdateWindow(hwnd);

				break;
			}

		case WM_DESTROY:
			{
				SysShutdown();					// release memory and shutdown
				break;
			}
	
		case WM_SIZE:
			{
				// resize the viewport after the window had been resized
				Resize(LOWORD(lparam), HIWORD(lparam));			
				
				screenw = LOWORD(lparam);
				screenh = HIWORD(lparam);
				
				break;
			}
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//*=====================
//  WinMain
//*=====================

int WINAPI XWinMainX(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd)
{
	MSG msg;
	WNDCLASSEX ex;

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

	if (!RegisterClassEx(&ex))
	{
		MessageBox(NULL, "Failed to register the window class", "Window Reg Error", MB_OK);
		return 1;
	}

	// Create the window
	CreateWnd(hinstance, 0, 0, screenw, screenh, SCRDEPTH, WINDOWED);	

	// The message loop
	while (!quit)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				quit = TRUE;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

//		RenderFrame();

		SwapBuffers(hdc);

		if (GetAsyncKeyState(VK_ESCAPE))
			SysShutdown();
	}

	return msg.lParam;
}
