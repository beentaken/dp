/******************************************************************************/
/*!
\file		GameWindow.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _GAMEWINDOW_H
#define _GAMEWINDOW_H

#include "Utility\Singleton.h"
#include "Graphics\ogl.h"
#include "Math\OMath.h"
#include <string>

class InputManager;
class GameWindow : public Singleton<GameWindow>
{
public:
  GameWindow();
  ~GameWindow() = default;
  void Init(int width, int height, bool fs);
  void Terminate();
  void SetActive(bool active);
  bool GetActive() const;

  //getters
  HDC GetDeviceContext() const;
  int GetWidth() const;
  int GetHeight() const;
  int GetDWidth() const;
  int GetDHeight() const;
  HWND GetWindowHandle() const;
  bool isFullScreen() const;
  bool StartedInFullScreen() const;
  Vec2 GetWindowToDisplayRatio() const;

  //windows resizing on Windows API
  void Resize(unsigned width, unsigned height);
  void ToggleFullscreen();
  void ChangeToResizableWindowAfterLoading();

private:
  void InitWindow();
  void CreateConsole();
  bool LoadExtensions(HWND hwnd);

  //need to serialize vsync boolean
  bool InitGL(bool vsync);
  void SetGLOptions();
  HWND ConstructWindow(int width, int height, bool fullscreen);

  //error handling
  std::string GetLastErrorAsString() const;

  //windows creation (make getters for these if needed)
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
  PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
  LPCSTR m_appName;
  HINSTANCE m_hinstance;
  HWND m_hwnd;
  PIXELFORMATDESCRIPTOR m_pixFormat;
  HDC m_deviceContext;
  HGLRC m_renderContext;
  RECT m_windowRect;
  std::string m_videoCard;

  //window stats
  int m_oldwidth;
  int m_oldheight;
  int m_displayWidth;
  int m_displayHeight;
  int m_width;
  int m_height;
  bool m_vsync;
  bool m_isFullScreen;
  bool m_startedInFullscreen;
  bool m_isActive;
  Vec2 m_W2Dratio;        //used to change between fullscreen and non-fullscreen
};

#endif

