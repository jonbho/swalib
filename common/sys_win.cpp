#include "stdafx.h"
#include "sys.h"

extern int Main(void);

HINSTANCE WIN_hInst      = 0;
int       WIN_nCmdShow   = 0;
HWND      WIN_hWnd       = 0;
HDC       WIN_hDC        = 0;
HGLRC     WIN_hGLRC      = 0;
bool      WIN_bGottaQuit = false;

//-----------------------------------------------------------------------------
LRESULT CALLBACK WIN_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message)
  {
    case WM_PAINT:   hdc = BeginPaint(hWnd, &ps); EndPaint(hWnd, &ps); break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default:         return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

//-----------------------------------------------------------------------------
ATOM WIN_RegisterClass()
{
  WNDCLASS wc;

  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = WIN_WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = WIN_hInst;
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "WIN_WindowClass";

  return RegisterClass(&wc);
}

//-----------------------------------------------------------------------------
bool WIN_SetFullScreen()
{
  DEVMODE dmScreenSettings;
  memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
  dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
  dmScreenSettings.dmPelsWidth  = SCR_WIDTH;
  dmScreenSettings.dmPelsHeight = SCR_HEIGHT;
  dmScreenSettings.dmBitsPerPel = 32;
  dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

  // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
  if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
      return true;
  else
      return false;
}

//-----------------------------------------------------------------------------
bool WIN_InitInstance()
{
  DWORD dwStyle, dwExStyle;
  if (SCR_FULLSCREEN)
  {
    dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    dwExStyle = WS_EX_APPWINDOW;
    ShowCursor(FALSE); 
  }
  else
  {
    dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  }
  RECT r; r.left = 0; r.right = SCR_WIDTH; r.top = 0; r.bottom = SCR_HEIGHT;
  AdjustWindowRect(&r, dwStyle, FALSE);
  WIN_hWnd = CreateWindowEx(dwExStyle, "WIN_WindowClass", "Gamecrash Window", dwStyle,
          0, 0, r.right-r.left, r.bottom-r.top, NULL, NULL, WIN_hInst, NULL);

  if (!WIN_hWnd)
     return false;

  ShowWindow(WIN_hWnd, WIN_nCmdShow);
  UpdateWindow(WIN_hWnd);

  return true;
}

//-----------------------------------------------------------------------------
void WIN_EnableOpenGL()
{
  PIXELFORMATDESCRIPTOR pfd;
  int format;

  WIN_hDC = GetDC( WIN_hWnd );

  // Set the pixel format for the DC
  ZeroMemory( &pfd, sizeof( pfd ) );
  pfd.nSize = sizeof( pfd );
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 16;
  pfd.iLayerType = PFD_MAIN_PLANE;
  format = ChoosePixelFormat( WIN_hDC, &pfd );
  SetPixelFormat( WIN_hDC, format, &pfd );

  // Create and enable the render context (RC)
  WIN_hGLRC = wglCreateContext( WIN_hDC );
  wglMakeCurrent( WIN_hDC, WIN_hGLRC );
}

//-----------------------------------------------------------------------------
void WIN_DisableOpenGL()
{
  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( WIN_hGLRC );
  ReleaseDC( WIN_hWnd, WIN_hDC );
}

//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hI, HINSTANCE hPrevI, LPSTR lpCmdLine, int nCS)
{
  WIN_hInst    = hI;
  WIN_nCmdShow = nCS;
  if (!WIN_RegisterClass()) return -1;
  if (SCR_FULLSCREEN) { if (!WIN_SetFullScreen()) return -1; }
  if (!WIN_InitInstance ()) return -1;
  WIN_EnableOpenGL();
  int retval = Main();
  WIN_DisableOpenGL();
  return retval;
}

//-----------------------------------------------------------------------------
void SYS_Pump()
{
  MSG msg;

  // Process messages if there are any
  while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
  {
    if (msg.message == WM_QUIT)
      WIN_bGottaQuit = true;
    else
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

//-----------------------------------------------------------------------------
void SYS_Show()
{
  glFlush();
  SwapBuffers( WIN_hDC );
}

//-----------------------------------------------------------------------------
bool SYS_GottaQuit()
{
  return WIN_bGottaQuit;
}

//-----------------------------------------------------------------------------
void SYS_Sleep(int ms)
{
  Sleep(ms);
}

//-----------------------------------------------------------------------------
bool SYS_KeyPressed(int key)
{
  return GetFocus() == WIN_hWnd && (GetAsyncKeyState(key) & 0x8000) != 0;
}

//-----------------------------------------------------------------------------
ivec2 SYS_MousePos()
{
  POINT pt;
  GetCursorPos(&pt);
  ScreenToClient(WIN_hWnd, &pt);

  ivec2 r = { pt.x, SCR_HEIGHT - pt.y };
  return r;
}

//-----------------------------------------------------------------------------
bool  SYS_MouseButonPressed(int button)
{
  return GetFocus() == WIN_hWnd && (GetAsyncKeyState(button) & 0x8000) != 0;
}
