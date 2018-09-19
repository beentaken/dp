/******************************************************************************/
/*!
\file		CameraBase.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "CameraBase.h"
#include "Input\InputManager.h"
#include "Utility\FrameRateController.h"
#include "Graphics\Window\GameWindow.h"
#include "Graphics\Cameras\CameraManager.h"

#ifdef _EDITOR
#include "Editor\Editor.h"
#endif

#define MAXFOV 180.f
#define MINFOV 1.f
#define MINZOOM 500.0f

static const float MAXPITCH = 89.f;
static const float MINPITCH = -89.f;
static const float MINYAW = -180.f;
static const float MAXYAW = 180.f;

CameraBase::CameraBase() :
  m_position(0.f, 0.f, 0.f),
  m_forward(0.f, 0.f, -1.f),
  m_right(1.0f, 0.0f, 0.0f),
  m_up(0.f, 1.f, 0.f),
  m_worldUp(0.f, 1.f, 0.f),
  m_isMouseLocked(true),
  orientation()
{
  //default camera values
  m_fov = 60.f;
  m_cam_speed = 500.5f;
  m_sensitivity = 0.05f;
  m_yaw = m_pitch = m_roll = 0.f;
  m_zoom = MINZOOM;
}

Mtx4 CameraBase::GetViewMatrix() const
{
  return m_viewMatrix;
}

void CameraBase::Init()
{
  m_input = &(InputManager::GetInstance());
  m_window = &(GameWindow::GetInstance());
  //get screen center
  int x = GetSystemMetrics(SM_CXSCREEN) >> 1;
  int y = GetSystemMetrics(SM_CYSCREEN) >> 1;  
  screen_center = Vec2((float)x, (float)y);
  //set cursor to start at center
  SetCursorPos((int)screen_center.x, (int)screen_center.y);
}

void CameraBase::Update()
{
  curr_dt = static_cast<float>(FrameRateController::GetInstance().GetDelta(false));

  //update if window is active
  if (m_window->GetActive())
  {
#ifdef _EDITOR
	if(!Editor::GetInstance().IsActive())
    GetMouseInput();
#endif
    UpdateMovement();
    UpdateZoom();
    UpdateView();
  }
}

void CameraBase::SetPosition(const Vec3 & pos)
{
  m_position = pos;
}

void CameraBase::SetPosition(float x, float y, float z)
{
  SetPosition(Vec3(x, y, z));
}

void CameraBase::SetCamSpeed(float speed)
{
  m_cam_speed = speed;
}

void CameraBase::SetSensitivity(float s)
{
  m_sensitivity = s;
}

void CameraBase::SetCamRotation(const Vec3 & euler)
{
  m_pitch = euler.x;
  m_yaw = euler.y;
  m_roll = euler.z;
     
}

float CameraBase::GetCamSpeed() const
{
  return m_cam_speed;
}

Vec3 CameraBase::GetPosition() const
{
  return m_position;
}

Vec3 CameraBase::GetCamForward() const
{
  return m_forward;
}

Vec3 CameraBase::GetCamRight() const
{
  return m_right;
}

Vec3 CameraBase::GetCamRotation() const
{
  //return rotation around X, Y and z-axis in degrees
  return Vec3(m_pitch, m_yaw, m_roll);
}

Mtx4 CameraBase::GetInverseCamRotationAsMatrix() const
{
  //compute rotation
  Quaternion qPitch = Quaternion(m_pitch, Vec3::EX());
  Quaternion qYaw = Quaternion(m_yaw, Vec3::EY());

  Quaternion curr_orient = qPitch * qYaw;
  curr_orient.Normalize();

  return curr_orient.ToMatrix();
}

void CameraBase::MoveForward()
{
  m_position += m_forward * m_cam_speed * curr_dt;
}

void CameraBase::MoveBack()
{
  m_position -= m_forward * m_cam_speed * curr_dt;
}

void CameraBase::MoveLeft()
{
  m_position -= m_right * m_cam_speed * curr_dt;
}

void CameraBase::MoveRight()
{
  m_position += m_right * m_cam_speed * curr_dt;
}

void CameraBase::MoveUp()
{
  m_position += m_up * m_cam_speed * curr_dt;
}

void CameraBase::MoveDown()
{
  m_position -= m_up * m_cam_speed * curr_dt;
}

void CameraBase::SetPositionWithZoom(Vec3 const & pos)
{
	SetPosition(pos + m_viewMatrix.row(2).ToVec3() * (m_zoom = MINZOOM));
}

float CameraBase::GetFOV() const
{
  return m_fov;
}

float CameraBase::GetSensitivity() const
{
  return m_sensitivity;
}

void CameraBase::SetFOV(float FOV)
{
  m_fov = FOV;
}

void CameraBase::SetCamForward(const Vec3 & f)
{
  m_forward = f;
}

void CameraBase::SetCamRight(const Vec3 & r)
{
  m_right = r;
}

void CameraBase::SetViewMatrix(const Mtx4 & view)
{
  m_viewMatrix = view;
}

void CameraBase::LockMouse()
{
  m_isMouseLocked = true;
}

void CameraBase::UnlockMouse()
{
  m_isMouseLocked = false;
}

void CameraBase::UpdateView()
{
  RecalculateVectors();
  Vec3 back = -m_forward;
  
  //build view matrix in column major (doesnt need to be transposed by glUniformMatrix4fv)
  Mtx4 view(m_right.x, m_right.y, m_right.z, -m_right.dot(m_position),
    m_up.x, m_up.y, m_up.z, -m_up.dot(m_position),
    back.x, back.y, back.z, -back.dot(m_position),
    0, 0, 0, 1);
  
  m_viewMatrix = view;
}

void CameraBase::Constraint()
{
  if (m_pitch > MAXPITCH) m_pitch = MAXPITCH;
  if (m_pitch < MINPITCH) m_pitch = MINPITCH;

  while (m_yaw < MINYAW)
    m_yaw += 360.f;

  while (m_yaw > MAXYAW)
    m_yaw -= 360.f;
}

void CameraBase::ResetOrientation()
{
  m_yaw = 0.f;
  m_pitch = 0.f;
  m_roll = 0.f;
}

void CameraBase::GetMouseInput()
{
  // Calculate our horizontal and vertical mouse movement from middle of the window
  POINT mouse;
  GetCursorPos(&mouse);
  Vec2 mousePos((float)mouse.x, (float)mouse.y);
  mouse_delta = (mousePos - screen_center) * m_sensitivity;

  if (m_isMouseLocked)
  {
    //set cursor to middle of screen
    SetCursorPos((int)screen_center.x, (int)screen_center.y);
  }
  
  //std::cout << "Mid window values: " << screen_center.x << "\t" << screen_center.y << std::endl;
  //std::cout << "Mouse values     : " << mousePos.x << "\t" << mousePos.y << std::endl;
  //std::cout << mouse_delta.x << "\t" << mouse_delta.y << std::endl << std::endl;

  //increment rotation angles by mouse movement in this frame
  m_yaw += mouse_delta.x;
  m_pitch += mouse_delta.y;
}

void CameraBase::RecalculateVectors()
{
  //get m_forward vector
  float rad_yaw = m_yaw * DEG2RAD;
  float rad_pitch = m_pitch * DEG2RAD;
  m_forward.x = std::sin(rad_yaw) * std::cos(rad_pitch);
  m_forward.y = std::sin(rad_pitch);
  m_forward.z = -std::cos(rad_yaw) * std::cos(rad_pitch);
  m_forward.normalize();

  //recalculate camera vectors using new m_forward

  m_right = m_forward.cross(m_worldUp);
  m_right.normalize();
  m_up = m_right.cross(m_forward);
  m_up.normalize();
}

Vec3 CameraBase::GetFrontPosOfCam()
{
	return m_position  + m_forward * (m_zoom = MINZOOM);
}

Mtx4 CameraBase::GetMatrixForWaterReflection(float waterHeight) const
{
  //move camera position below water
  Vec3 underWaterPos = m_position;
  float distanceAboveWater = m_position.y - waterHeight;
  underWaterPos.y -= (2 * distanceAboveWater);

  //compute rotation
  Quaternion qPitch = Quaternion(m_pitch, Vec3::EX());
  Quaternion qYaw = Quaternion(m_yaw, Vec3::EY());

  Quaternion curr_orient = qPitch * qYaw;
  curr_orient.Normalize();

  Mtx4 rot = curr_orient.ToMatrix();

  //inverse translation
  Vec3 translation(underWaterPos.x, underWaterPos.y, underWaterPos.z);
  Mtx4 transMat = Mtx4::translate(-translation);

  //translate back to origin and rotate
  return (rot * transMat);
}

