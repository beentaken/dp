/******************************************************************************/
/*!
\file		FPSCamera.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "FPSCamera.h"
#include "Utility\FrameRateController.h"

#define MAXFOV 180.f
#define MINFOV 1.f

void FPSCamera::UpdateMovement()
{
  //get original camera vectors before transpose
#if 0
  forward = viewMatrix.col(2).ToVec3();
  right = viewMatrix.col(0).ToVec3();
#endif

  m_forward = -m_viewMatrix.row(2).ToVec3();        //row2 is the back vector negate to get forward
  m_right = m_viewMatrix.row(0).ToVec3();
  m_up = m_viewMatrix.row(1).ToVec3();

  //move forward and backward
  if (m_input->IsKeyDown(KEY_W))
  {
    MoveForward();
  }

  if (m_input->IsKeyDown(KEY_S))
  {
    MoveBack();
  }

  //strafe left and right
  if (m_input->IsKeyDown(KEY_A))
  {
    MoveLeft();
  }

  if (m_input->IsKeyDown(KEY_D))
  {
    MoveRight();
  }

  //std::cout << "Camera Position: ";
  //m_position.print();
}  

void FPSCamera::UpdateView()
{
  Quaternion qPitch = Quaternion(m_pitch, Vec3::EX());
  Quaternion qYaw = Quaternion(m_yaw, Vec3::EY());

  Quaternion curr_orient = qPitch * qYaw;
  curr_orient.Normalize();

  Mtx4 rot = curr_orient.ToMatrix();

  //inverse translation
  Vec3 translation(m_position.x, m_position.y, m_position.z);
  Mtx4 transMat = Mtx4::translate(-translation);

  //translate back to origin and rotate
  m_viewMatrix = rot * transMat;
}


void FPSCamera::UpdateZoom()
{
  float scrolldelta = m_input->getScrollDeltaY();

  if (m_fov >= MINFOV && m_fov <= MAXFOV)
    m_fov -= scrolldelta;

  m_fov = (m_fov < MINFOV) ? MINFOV : m_fov;
  m_fov = (m_fov > MAXFOV) ? MAXFOV : m_fov;

  //std::cout << "fov: " << fov << std::endl;
}
