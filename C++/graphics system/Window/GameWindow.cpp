/******************************************************************************/
/*!
\file		GameWindow.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "GameWindow.h"
#include "Input/InputManager.h"
#include "Utility\Logging.h"
#include "../resource.h"
#include "Scripts\PauseBehaviour.h"
#include "Utility\FrameRateController.h"
#include "Editor\Editor.h"
// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

//Handle windows/system messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
  static MenuManager & mm = MenuManager::GetInstance();
  static InputManager & im = InputManager::GetInstance();
  static EventHandler<PauseEvent> & phnd = EventHandler<PauseEvent>::GetInstance();
  static GameWindow & gw = GameWindow::GetInstance();
  static FrameRateController & frc = FrameRateController::GetInstance();

  switch (umessage)
  {
    // Check if the window is being closed.
    case WM_CLOSE:
    {
      //MLogX_("Set to close");
      // PostQuitMessage(0);
#ifdef _EDITOR

		if (Editor::GetInstance().IsNotPlaying())
		{
			PostQuitMessage(0);
		}
		else
#endif
      // Hack to open confirmation menu for quitting
      if (!frc.GetPause())
      {
        phnd.TriggerEvent(true);
        mm.SelectNext(2);
        mm.Enter();
      }
      return 0;
    }
    
    //check if window is active
    case WM_SETFOCUS:
    {
#ifdef _EDITOR

		if(Editor::GetInstance().IsPlaying())

#endif
      gw.SetActive(true);
      im.AcceptInput(true);
      // Kills pause menu on return
      // EventHandler<PauseEvent>::GetInstance().TriggerEvent(false);
      break;
    }

    case WM_KILLFOCUS:
    {
#ifdef _EDITOR
      if (Editor::GetInstance().IsPlaying())
#endif
      {
        gw.SetActive(false);
        im.AcceptInput(false);
        phnd.TriggerEvent(true);
      }
      break;
    }
    case WM_ACTIVATE:
    {
      if(wparam == WA_INACTIVE)
        ShowWindow(gw.GetWindowHandle(), SW_MINIMIZE);
      break;
    }
    //Windows resize
    case WM_SIZE:
    {
      // LoWord=Width, HiWord=Height
      // call glViewport here
      static GraphicsSystem& gsys = GraphicsSystem::GetInstance();
      if (gsys.LoadingDone())
      {
        gsys.ResizeViewport(LOWORD(lparam), HIWORD(lparam));
      }
      
      break;
    }
    //Resource Management
    case WM_DROPFILES:
    {
      int itemCount = DragQueryFile((HDROP)wparam, 0xFFFFFFFF, 0, 0);
      if (itemCount)
      {
        char buffer[512] = { 0 };
        DragQueryFile((HDROP)wparam, 0, buffer, 512);
        DragFinish((HDROP)wparam);

        //std::cout << "DRAG FILENAME: " << buffer << std::endl;
        //QFEngine::GetInstance().m_Resource->GetDragDropInDirectory(buffer);
      }
      break;
    }

    // All other messages pass to the message handler in the system class.
    default:
    {
      return im.MessageHandler(hwnd, umessage, wparam, lparam);
    }
  }
  return 0;
}


#pragma region OPENGL_INITIALIZATION
void GameWindow::SetGLOptions()
{
  //depth test only
  glEnable(GL_DEPTH_TEST);

  // Set the polygon winding to counterclockwise (clockwise is now backfacing) //CS200 convention is counterclockwise so we follow that
  glFrontFace(GL_CCW);
  
  // Enable back face culling.
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_ALPHA_TEST);

  // Enable blending and alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //enable stencil testing for outlining
  glEnable(GL_STENCIL_TEST);            
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  //default clear for depth and stencil
  glClearDepth(1);
  glClearStencil(0);

  //set viewport
  glViewport(0, 0, m_width, m_height);
}

bool GameWindow::InitGL(bool vsync)
{
	
  //set the pointer to the window
  Assert("GameWindow::InitGL(): Error: handle to hwnd is NULL", m_hwnd);

  // Get the device context for this window.
  m_deviceContext = GetDC(m_hwnd);
  Assert("GameWindow::InitGL(): Error: Device context is NULL", m_deviceContext);

  //create PIXELFORMATDESCRIPTOR struct
  m_pixFormat =
  {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,     //Flags
    PFD_TYPE_RGBA,                                                  //The kind of framebuffer. RGBA or palette.
    32,                                                             //Colordepth of the framebuffer.
    0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0, 0, 0, 0,
    24,                        //Number of bits for the depthbuffer
    8,                        //Number of bits for the stencilbuffer
    0,                        //Number of Aux buffers in the framebuffer.
    PFD_MAIN_PLANE,
    0,
    0, 0, 0
  };
  int format = ChoosePixelFormat(m_deviceContext, &m_pixFormat);

  // If the video card/display can handle our desired pixel format then we set it as the current one.
  int result = SetPixelFormat(m_deviceContext, format, &m_pixFormat);
  Assert("GameWindow::InitGL(): Pixel format not correctly set", result == 1);

  int attributeList[5];
  attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
  attributeList[1] = 3;
  attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
  attributeList[3] = 3;
  // Null terminate the attribute list.
  attributeList[4] = 0;

  // Create a OpenGL 4.1 rendering context and set it
  m_renderContext = wglCreateContextAttribsARB(m_deviceContext, 0, attributeList);
  Assert("Unable to create opengl context: Error: " + GetLastErrorAsString(), m_renderContext);

  // Set the rendering context to active.
  result = wglMakeCurrent(m_deviceContext, m_renderContext);
  Assert("Unable to create opengl context: Error: " + GetLastErrorAsString(), result == 1);


  //once we have the rendering context, init glew to get the extensions
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return false;
  }

  //set openGL options
  SetGLOptions();

  // Get the name of the video card.
  std::string vendorString = (char*)glGetString(GL_VENDOR);
  std::string rendererString = (char*)glGetString(GL_RENDERER);
  m_videoCard = vendorString + "-" + rendererString;

  //vsync options
  m_vsync = vsync;
  if (vsync)
    result = wglSwapIntervalEXT(1);
  else
    result = wglSwapIntervalEXT(0);

  // Check if vsync was set correctly.
  if (result != 1)
    return false;

  //if everything is ok we return true
  return true;
}

#pragma endregion

#pragma region WINDOW_CREATION

void GameWindow::Init(int width, int height, bool fs)
{
#ifdef _DEBUG
  CreateConsole();
#endif
  //register window class and create temp window for extensions loading
  InitWindow();

  //construct actual window
  m_hwnd = ConstructWindow(width, height, fs);
  Assert("GameWindow::Init(): Error creating window", m_hwnd);

  bool result = InitGL(true);
  Assert("GameWindow::Init(): Error initializing openGL", result);

  //show window
  ShowWindow(m_hwnd, SW_SHOW);
  //set as main focus
  SetForegroundWindow(m_hwnd);
  SetFocus(m_hwnd);
  m_isActive = true;
}



void GameWindow::InitWindow()
{
  m_appName = "New Application";
  m_displayWidth = GetSystemMetrics(SM_CXSCREEN);
  m_displayHeight = GetSystemMetrics(SM_CYSCREEN);

  // Get the instance of this application.
  m_hinstance = GetModuleHandle(NULL);
  /******************************************************************************/
  /*
  Register Window class
  */
  /******************************************************************************/
  WNDCLASSEX wc;
  HWND temphwnd;

  // Setup the windows class with default settings.
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_hinstance;
  wc.hIcon = LoadIcon(m_hinstance, MAKEINTRESOURCE(IDI_ICON1));
  wc.hIconSm = wc.hIcon;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = m_appName;
  wc.cbSize = sizeof(WNDCLASSEX);

  // Register the window class.
  if (!RegisterClassEx(&wc))
  {
    std::cout << "GameWindow::InitWindow(): Unable to register class: " << GetLastErrorAsString() << std::endl;
  }

  /******************************************************************************/
  /*
  Create a temporary window for the windows OpenGL extension setup.
  */
  /******************************************************************************/

  temphwnd = CreateWindowEx(WS_EX_APPWINDOW,        // window extended style
    m_appName,              // class name
    m_appName,              // window name
    WS_OVERLAPPEDWINDOW,    // window style : overlapped for window with title bar, WS_POPUP for full screen window
    0,                      // x and y position
    0,
    640,                    // temporary width
    480,                    // temporary height
    NULL,                   // handle to parent window
    NULL,                   // handle to menu
    m_hinstance,            // handle to instance of window
    NULL);                  // pointer to window creation data

                            //if temporary window fails to create, exit

  Assert("Windows creation failed : " + GetLastErrorAsString(), temphwnd);

  // Don't show the temporary window.
  ShowWindow(temphwnd, SW_HIDE);

  // load openGL extensions with the temporary window
  bool result = LoadExtensions(temphwnd);
  Assert("GameWindow::Init(): Error: Could not initialize the OpenGL extensions.", result);

  // Release the temporary window now that the extensions have been initialized.
  DestroyWindow(temphwnd);
  temphwnd = NULL;
}

HWND GameWindow::ConstructWindow(int width, int height, bool fullscreen)
{
  m_isFullScreen = fullscreen;

  DWORD       dwExStyle;                      // Window Extended Style
  DWORD       dwStyle;                        // Window Style
  int posX, posY;
  if (fullscreen)
  {
    // Set the position of the window to the top left corner.
    posX = posY = 0;
    m_startedInFullscreen = true;

    //set fullscreen windows style
    dwExStyle = WS_EX_APPWINDOW;
    dwStyle = WS_POPUP;

    //store original width and height to switch back
    m_oldwidth = width;
    m_oldheight = height;

    //set width and height for fullscreen
    m_width = m_displayWidth;
    m_height = m_displayHeight;
    m_W2Dratio = Vec2(m_width / 1600.f, m_height / 900.f); // Game UI was built using the resolution[1600, 900]

    //change display settings for full screen
    DEVMODE dmScreenSettings;                                     // Device Mode
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));       // Makes Sure Memory's Cleared
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth = (unsigned long)m_displayWidth;
    dmScreenSettings.dmPelsHeight = (unsigned long)m_displayHeight;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
      std::cout << "GameWindow::ConstructWindow: Error: unable to change to fullscreen" << std::endl;
    }
  }
  else
  {
    m_width = width;
    m_height = height;
    m_W2Dratio = Vec2(m_width / 1600.f, m_height / 900.f); // Game UI was built using the resolution[1600, 900]

    // Place the window in the middle of the screen.
    posX = (m_displayWidth - m_width) / 2;
    posY = (m_displayHeight - m_height) / 2;

    //set windowed style
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    //dwStyle = WS_OVERLAPPEDWINDOW;
    //since no option to runtime fullscreen, disable maximize option
    dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
  }

  //set window RECT
  m_windowRect.left = (long)0;
  m_windowRect.right = (long)m_width;
  m_windowRect.top = (long)0;
  m_windowRect.bottom = (long)m_height;

  // Adjust window To requested Size
  AdjustWindowRectEx(&m_windowRect, dwStyle, FALSE, dwExStyle);

  //create the window and return it
  return CreateWindowEx(dwExStyle,                      // Extended Style For The Window
    m_appName,                                          // registered class Name
    "Little Chef Story",                                // game name 
    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,        // Selected Window Style
    posX, posY,                                         // top left corner of window                                    
    m_windowRect.right - m_windowRect.left,             // Calculate Adjusted Window Width
    m_windowRect.bottom - m_windowRect.top,             // Calculate Adjusted Window Height
    NULL,                                               // No Parent Window
    NULL,                                               // No Menu
    m_hinstance,                                        // Instance
    NULL);                                              // Don't Pass Anything To WM_CREATE
}

void GameWindow::CreateConsole()
{
  //Create the console
  if (AllocConsole())
  {
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE* file = nullptr;

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    //redirect std output to console
    freopen_s(&file, "CONOUT$", "w", stdout);
    freopen_s(&file, "CONIN$", "r", stdin);
    freopen_s(&file, "CONOUT$", "w", stderr);

    //Clear the error state for each of the C++ standard stream objects. We need to do this, as
    //attempts to access the standard streams before they refer to a valid target will cause the
    //iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
    //to always occur during startup regardless of whether anything has been read from or written to
    //the console or not.
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();

    SetConsoleTitle("Console: GameWindow");
  }
  else
  {
    MessageBox(m_hwnd, (std::string("Could not initialize the console") + GetLastErrorAsString()).data(), "Error", MB_OK);
  }
}

bool GameWindow::LoadExtensions(HWND hwnd)
{
  HDC deviceContext;
  HGLRC renderContext;
  PIXELFORMATDESCRIPTOR pixelFormat;
  ZeroMemory(&pixelFormat, sizeof(PIXELFORMATDESCRIPTOR));
  // Get the device context for this window.
  deviceContext = GetDC(hwnd);
  // Set a temporary default pixel format.
  SetPixelFormat(deviceContext, 1, &pixelFormat);
  // Create a temporary rendering context.
  renderContext = wglCreateContext(deviceContext);
  // Set the temporary rendering context as the current rendering context for this window.
  int error = wglMakeCurrent(deviceContext, renderContext);
  if (error != 1)
  {
    return false;
  }

  wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
  if (!wglChoosePixelFormatARB)
  {
    return false;
  }

  wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  if (!wglCreateContextAttribsARB)
  {
    return false;
  }

  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  if (!wglSwapIntervalEXT)
  {
    return false;
  }

  // Release the temporary rendering context now that the extensions have been loaded.
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(renderContext);
  renderContext = NULL;

  // Release the device context for this window.
  ReleaseDC(hwnd, deviceContext);
  deviceContext = nullptr;

  //all ok
  return true;
}

#pragma endregion

#pragma region CLASS_FUNCTIONS
GameWindow::GameWindow():
m_appName(NULL),
m_hinstance(NULL),
m_hwnd(NULL),
m_oldheight(0),
m_oldwidth(0),
m_width(0),
m_height(0),
m_isFullScreen(false),
m_startedInFullscreen(false),
m_isActive(false),
m_displayWidth(0),
m_displayHeight(0)
{

}

void GameWindow::Terminate()
{
  if (m_isFullScreen)
  {
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(TRUE);
  }

  //release device and rendering context
  if (m_renderContext)
  {
    if (!wglMakeCurrent(NULL, NULL))
    {
      MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    }
    if (!wglDeleteContext(m_renderContext))
    {
      MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    }
    m_renderContext = NULL;
  }
  if (m_deviceContext && !ReleaseDC(m_hwnd, m_deviceContext))
  {
    MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    m_deviceContext = NULL;
  }

  //destroy window and unregister class
  if (m_hwnd && !DestroyWindow(m_hwnd))
  {
    MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    m_hwnd = NULL;
  }
  if (!UnregisterClass(m_appName, m_hinstance))
  {
    MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    m_hinstance = NULL;
  }
}

void GameWindow::SetActive(bool active)
{
  m_isActive = active;
}

bool GameWindow::GetActive() const
{
  return m_isActive;
}

HDC GameWindow::GetDeviceContext() const
{
  return m_deviceContext;
}

int GameWindow::GetWidth() const
{
  return m_width;
}

int GameWindow::GetHeight() const
{
  return m_height;
}

int GameWindow::GetDWidth() const
{
	return m_displayWidth;
}

int GameWindow::GetDHeight() const
{
	return m_displayHeight;
}

HWND GameWindow::GetWindowHandle() const
{
	return m_hwnd;
}

bool GameWindow::isFullScreen() const
{
  return m_isFullScreen;
}

bool GameWindow::StartedInFullScreen() const
{
  return m_startedInFullscreen;
}

Vec2 GameWindow::GetWindowToDisplayRatio() const
{
  return m_W2Dratio;
}

void GameWindow::Resize(unsigned width, unsigned height)
{
  m_width = width;
  m_height = height;

  //for some reason after letterbox dont need this code anymore
  /*
  if (!m_isFullScreen)
  {// Game UI was built using the resolution[1600, 900]
    m_W2Dratio = Vec2(m_width / 1600.f, m_height / 900.f);
  }
  */
}

void GameWindow::ToggleFullscreen()
{
  if (!m_isFullScreen)
  {
    m_oldwidth = m_width;
    m_oldheight = m_height;

    //set to borderless window
    m_isFullScreen = true;
    SetWindowLongPtr(m_hwnd, GWL_STYLE,
      WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);

    //move window to correct position
    SetWindowPos(m_hwnd, HWND_TOP, 0, 0, m_displayWidth, m_displayHeight, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);

    //resize gl viewport and set scaleratio
    GraphicsSystem::GetInstance().ResizeViewport(m_displayWidth, m_displayHeight);
  }
  else
  {
    m_isFullScreen = false;

    int posX = (m_displayWidth - m_oldwidth) / 2;
    int posY = (m_displayHeight - m_oldheight) / 2;

    //set to windowed mode
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = m_oldwidth;
    rect.bottom = m_oldheight;
    SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    //move window to correct position
    SetWindowPos(m_hwnd, HWND_TOP, posX, posY, rect.right - rect.left, rect.bottom - rect.top,
      SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);

    GraphicsSystem::GetInstance().ResizeViewport(m_oldwidth, m_oldheight);
  }
}

void GameWindow::ChangeToResizableWindowAfterLoading()
{
  SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
}

std::string GameWindow::GetLastErrorAsString() const
{
  //Get the error message, if any.
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0)
    return std::string("No error message recorded");

  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

  std::string message(messageBuffer, size);

  //Free the buffer.
  LocalFree(messageBuffer);

  return message;
}

#pragma endregion
