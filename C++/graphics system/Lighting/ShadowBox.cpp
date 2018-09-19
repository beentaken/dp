/******************************************************************************/
/*!
\file		ShadowBox.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "ShadowBox.h"
#include "Math\OMath.h"
#include "Graphics\Cameras\CameraManager.h"
#include "Graphics\DebugDraw\PointRenderer.h"

#define FRUSTUM_BACK_OFFSET 10.f

void ShadowBox::Update(const Mtx4 & lightView)
{
  //set lightview matrix for this frame
  m_lightViewMtx = lightView;

  //update box rotation and position to where camera is
  CameraBase* pCam = CameraManager::GetInstance().GetActiveCamera();
  Vec3 camPos = pCam->GetPosition();
  Mtx3 camRot = Mtx3(pCam->GetInverseCamRotationAsMatrix());

  //calculate center of far/near plane
  Vec3 worldForward = camRot * m_forward;
  Vec3 centerNear = camPos + (m_near * worldForward);
  Vec3 centerFar = camPos + (m_shadowDist * worldForward);

  //use that to calculate 8 pts of view frustum
  std::vector<Vec4> frustum = CalculateFrustumVertices(camRot, worldForward, centerNear, centerFar);

  //take min and max of frustum pts and construct a box
  m_min = m_max = Vec3(frustum[0]);
  for (auto& elem : frustum)
  {
    m_min.x = std::min(elem.x, m_min.x);
    m_min.y = std::min(elem.y, m_min.y);
    m_min.z = std::min(elem.z, m_min.z);

    m_max.x = std::max(elem.x, m_max.x);
    m_max.y = std::max(elem.y, m_max.y);
    m_max.z = std::max(elem.z, m_max.z);
  }


  //increase size of box at the back so shadows dont get clipped
  m_max.z += FRUSTUM_BACK_OFFSET;

  //debug draw the box
#define DEBUG_DRAW
#ifdef DEBUG_DRAW
  PointRenderer& render = PointRenderer::GetInstance();

  Mtx3 inv = Mtx3(m_lightViewMtx.inverse());

  Vec3 nPleftTop = inv * Vec3(m_min.x, m_max.y, m_max.z);
  Vec3 nPleftBot = inv * Vec3(m_min.x, m_min.y, m_max.z);
  Vec3 nPRightBot = inv * Vec3(m_max.x, m_min.y, m_max.z);
  Vec3 nPRightTop = inv * m_max;

  Vec3 fPleftTop = inv * Vec3(m_min.x, m_max.y, m_min.z);
  Vec3 fPleftBot = inv * m_min;
  Vec3 fPRightBot = inv * Vec3(m_max.x, m_min.y, m_min.z);
  Vec3 fPRightTop = inv * Vec3(m_max.x, m_max.y, m_min.z);

  //front face
  render.AddLineToRender(nPleftTop, nPRightTop);
  render.AddLineToRender(nPleftTop, nPleftBot);
  render.AddLineToRender(nPleftBot, nPRightBot);
  render.AddLineToRender(nPRightBot, nPRightTop);

  //sides
  render.AddLineToRender(nPleftTop, fPleftTop);
  render.AddLineToRender(nPleftBot, fPleftBot);
  render.AddLineToRender(nPRightTop, fPRightTop);
  render.AddLineToRender(nPRightBot, fPRightBot);

  //back
  render.AddLineToRender(fPleftTop, fPRightTop);
  render.AddLineToRender(fPleftTop, fPleftBot);
  render.AddLineToRender(fPleftBot, fPRightBot);
  render.AddLineToRender(fPRightBot, fPRightTop);

  render.AddPointToRender(nPleftTop);
  render.AddPointToRender(nPleftBot);
  render.AddPointToRender(nPRightBot);
  render.AddPointToRender(nPRightTop);
  render.AddPointToRender(fPleftTop);
  render.AddPointToRender(fPleftBot);
  render.AddPointToRender(fPRightBot);
  render.AddPointToRender(fPRightTop);

  render.AddPointToRender(GetCenter());
#endif

}

//returns the center of box in world space
Vec3 ShadowBox::GetCenter() const
{
  Vec3 pt = (m_min + m_max) * 0.5f;
  Vec4 temp(pt.x, pt.y, pt.z, 1.f);
  Mtx4 invMtx = m_lightViewMtx.inverse();
  Vec4 res = invMtx * temp;
  return Vec3(res.x, res.y, res.z);
}

float ShadowBox::GetWidth() const
{
  return m_max.x - m_min.x;
}

float ShadowBox::GetHeight() const
{
  return m_max.y - m_min.y;
}

float ShadowBox::GetLength() const
{
  return m_max.z - m_min.z;
}

void ShadowBox::Init(float shadowDist, float fov, float nearPlane, float aspect)
{
  m_shadowDist = shadowDist;
  m_fov = fov;
  m_aspect = aspect;
  m_near = nearPlane;
  //calculate size of shadowbox based on initial shadowdistance
  m_farPlaneWidth = m_shadowDist * std::tanf(fov * DEG2RAD);    //opp = adj * tan_theta
  m_nearPlaneWidth = m_near * std::tanf(fov * DEG2RAD);
  m_farPlaneHeight = m_farPlaneWidth / aspect;
  m_nearPlaneHeight = m_nearPlaneWidth / aspect;
}

void ShadowBox::Setup(float currfov, float curraspect)
{
  //if fov or aspect not equal, update size of box
  if (m_fov != currfov || m_aspect != curraspect)
    Init(m_shadowDist, currfov, m_near, curraspect);
}

std::vector<Vec4> ShadowBox::CalculateFrustumVertices(const Mtx4 & rot, const Vec3 & forward, 
  const Vec3 & centerNear, const Vec3 & centerFar)
{
  Vec3 up = Mtx3(rot) * m_up;
  Vec3 right = Vec3CrossProduct(forward, up);
  Vec3 down = -up;
  Vec3 left = -right;

  //calculate middle points on frustum planes
  Vec3 farTop = centerFar + (up * m_farPlaneHeight);
  Vec3 farBot = centerFar + (down * m_farPlaneHeight);
  Vec3 nearTop = centerNear + (up * m_nearPlaneHeight);
  Vec3 nearBot = centerNear + (down * m_nearPlaneHeight);

  //calculate the 8 vertex that make up the frustum
  std::vector<Vec4> points;
  points.resize(8);
  points[0] = CalculateFrustumCorner(farTop, right, m_farPlaneWidth);
  points[1] = CalculateFrustumCorner(farTop, left, m_farPlaneWidth);
  points[2] = CalculateFrustumCorner(farBot, right, m_farPlaneWidth);
  points[3] = CalculateFrustumCorner(farBot, left, m_farPlaneWidth);
  points[4] = CalculateFrustumCorner(nearTop, right, m_nearPlaneWidth);
  points[5] = CalculateFrustumCorner(nearTop, left, m_nearPlaneWidth);
  points[6] = CalculateFrustumCorner(nearBot, right, m_nearPlaneWidth);
  points[7] = CalculateFrustumCorner(nearBot, left, m_nearPlaneWidth);

  return points;
}

Vec4 ShadowBox::CalculateFrustumCorner(const Vec3 & start, const Vec3 & dir, float dist)
{
  Vec3 pt = start + (dir * dist);
  Vec4 temp(pt.x, pt.y, pt.z, 1.f);

  //return frustum based on light orientation
  return m_lightViewMtx * temp;
}
