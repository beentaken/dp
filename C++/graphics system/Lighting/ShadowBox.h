/******************************************************************************/
/*!
\file		ShadowBox.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef SHADOW_BOX_H
#define SHADOW_BOX_H
#pragma once


//represents the a 3D cuboid box that determines an area in which object will cast shadow
//used for shadow map orthographic projection
class ShadowBox
{
public:
  ShadowBox() = default;
  void Init(float shadowDist, float fov, float nearPlane, float aspect);
  void Setup(float currfov, float curraspect);
  void Update(const Mtx4 & lightView);
  Vec3 GetCenter() const;
  float GetWidth() const;
  float GetHeight() const;
  float GetLength() const;

private:
  std::vector<Vec4> CalculateFrustumVertices(const Mtx4& rot, const Vec3& forward, const Vec3& centerNear, const Vec3& centerFar);
  
  //returns corner of view frustum in as seen from light
  Vec4 CalculateFrustumCorner(const Vec3& start, const Vec3& dir, float width);

  Vec3 m_min;
  Vec3 m_max;
  Mtx4 m_lightViewMtx;
  Vec3 m_up = Vec3::EY();
  Vec3 m_forward = -Vec3::EZ();
  float m_shadowDist;
  float m_fov;
  float m_aspect;
  float m_near;

  //half of frustum size
  float m_farPlaneHeight, m_farPlaneWidth, m_nearPlaneHeight, m_nearPlaneWidth;
};

#endif
