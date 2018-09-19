/******************************************************************************/
/*!
\file		CameraManager.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <unordered_map>
#include <memory>
#include "CameraBase.h"
#include "Utility\Singleton.h"

using CameraPtr = std::unique_ptr<CameraBase>;
using CameraList = std::unordered_map<std::string, CameraPtr>;

enum class CameraType
{
  Default = 0,
  FPS,
  Editor
};


class CameraManager : public Singleton<CameraManager>
{
public:
  CameraManager();
  void Init();
  void Update();
  void Terminate();

  //add and remove
  CameraBase* AddCamera(const std::string& name, CameraType type);
  void RemoveCamera(const std::string& name);

  //getters
  size_t CameraCount() const;
  CameraBase* GetCamera(const std::string& name);
  CameraBase* GetActiveCamera() const;
  Mtx4 GetCurrentViewMatrix() const;
  Mtx4 GetWaterReflectionMatrix(float waterHeight) const;

  //setters
  void SetActiveCamera(const std::string& name);
  void SetActiveCamera(CameraBase* cam);

private:
  CameraList m_cameras;
  CameraBase* m_activeCam;
};

#endif

