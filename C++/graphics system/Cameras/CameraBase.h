/******************************************************************************/
/*!
\file		CameraBase.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef CAMERABASE_H
#define CAMERABASE_H

#include "Math\OMath.h"
#include "Input\InputManager.h"
//notes on camera
//owned by graphics
//right handed
//default position is (0, 0, 0)
//looking down -zaxis so forward is (0, 0, -1), i.e(back vector is +zaxis)

class GameWindow;

class CameraBase
{
public:
  CameraBase();
  
  //called by Graphics
  void Init();
  void Update();

  //setters
  void SetPosition(const Vec3&);
  void SetPosition(float, float, float);
  void SetCamSpeed(float);
  void SetSensitivity(float);
  void SetCamRotation(const Vec3& euler);
  void SetFOV(float fov);
  void SetCamForward(const Vec3& f);              //camera's forward vector
  void SetCamRight(const Vec3& r);                //camera's x-axis (right vector)
  void SetViewMatrix(const Mtx4& view);

  //getters
  Vec3 GetPosition() const;
  Vec3 GetCamForward() const;
  Vec3 GetCamRight() const;
  Vec3 GetCamRotation() const;
  Mtx4 GetInverseCamRotationAsMatrix() const;
  float GetCamSpeed() const;
  float GetFOV() const;
  float GetSensitivity() const;
  Mtx4 GetViewMatrix() const;

  //returns a matrix so that the camera moves under the water and renders objects above water
  Mtx4 GetMatrixForWaterReflection(float waterHeight) const;

  //movement
  void MoveForward();
  void MoveBack();
  void MoveLeft();
  void MoveRight();
  void MoveUp();
  void MoveDown();

  // zx editorcamera
  void SetPositionWithZoom(Vec3 const&);
  Vec3 GetFrontPosOfCam();

  //locks/unlocks mouse
  void LockMouse();
  void UnlockMouse();

protected:
  //virtual functions to override
  virtual void UpdateMovement() {}
  virtual void UpdateZoom() {}
  virtual void UpdateView();

  void RecalculateVectors();
  void Constraint();
  void ResetOrientation();

  float m_fov;
  float m_zoom;
  float m_yaw;           //horizontal angle around y-axis in degrees
  float m_pitch;         //vertical angle around x-axis in degrees
  float m_roll;          //angle around z-axis in degrees

  //movement
  Vec3 m_position;
  Vec3 m_forward;
  Vec3 m_worldUp;
  Vec3 m_up;
  Vec3 m_right;
  
  float m_cam_speed;
  Mtx4 m_viewMatrix;
  float m_sensitivity;
  InputManager* m_input;
  Quaternion orientation;
  Vec2 mouse_delta;

private:
  void GetMouseInput();
  float curr_dt;

  //input
  Vec2 screen_center;
  GameWindow* m_window;
  bool m_isMouseLocked;
};

#endif
