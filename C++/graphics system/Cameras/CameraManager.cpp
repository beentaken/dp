/******************************************************************************/
/*!
\file		CameraManager.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "CameraManager.h"
#include "FPSCamera.h"
#include "EditorCamera.h"

CameraManager::CameraManager() : m_activeCam(nullptr)
{
}

void CameraManager::Init()
{
  //default cameras
  AddCamera("FPS", CameraType::FPS);
  AddCamera("Editor", CameraType::Editor);
}

void CameraManager::Update()
{
}

void CameraManager::Terminate()
{
}

CameraBase* CameraManager::AddCamera(const std::string & name, CameraType type)
{
  CameraList::iterator it = m_cameras.find(name);
  //this is a hack
  if (it != m_cameras.end())
  {
	  return it->second.get();
  }
  //Assert("Camera with this name already exists", it == m_cameras.end(), ErrorLevel::SystemCrash);
  
  //add the appropriate camera and return
  switch (type)
  {
    case CameraType::Default:
    {
      auto& result = m_cameras.emplace(std::make_pair(name, std::make_unique<CameraBase>()));
      Assert("Failed to create camera", result.second, ErrorLevel::SystemCrash);
      return result.first->second.get();
    }
    case CameraType::FPS:
    {
      auto& result = m_cameras.emplace(std::make_pair(name, std::make_unique<FPSCamera>()));
      Assert("Failed to create camera", result.second, ErrorLevel::SystemCrash);
      return result.first->second.get();
    }
    case CameraType::Editor:
    {
      auto& result = m_cameras.emplace(std::make_pair(name, std::make_unique<EditorCamera>()));
      Assert("Failed to create camera", result.second, ErrorLevel::SystemCrash);
      return result.first->second.get();
    }
    default:
    {
      Assert("Not a valid camera type", false, ErrorLevel::SystemCrash);
      break;
    }
  }

  //should never get here (suppress warnings)
  return nullptr;
}

void CameraManager::RemoveCamera(const std::string & name)
{
  CameraList::iterator it = m_cameras.find(name);
  if (it != m_cameras.end())
  {
    m_cameras.erase(it);
    return;
  }
  //else
  Assert("Unable to find camera to remove", false, ErrorLevel::Warning);
}

CameraBase* CameraManager::GetCamera(const std::string & name) 
{
  CameraList::iterator it = m_cameras.find(name);
  Assert("Unable to find camera", it != m_cameras.end(), ErrorLevel::SystemCrash);
  return it->second.get();
}

CameraBase * CameraManager::GetActiveCamera() const
{
  Assert("Active camera is a nullptr", m_activeCam != nullptr, ErrorLevel::SystemCrash);
  return m_activeCam;
}

size_t CameraManager::CameraCount() const
{
  return m_cameras.size();
}

void CameraManager::SetActiveCamera(const std::string & name)
{
  CameraList::iterator it = m_cameras.find(name);
  Assert("Unable to find camera", it != m_cameras.end(), ErrorLevel::SystemCrash);
  m_activeCam = it->second.get();
}

void CameraManager::SetActiveCamera(CameraBase * cam)
{
  m_activeCam = cam;
}

Mtx4 CameraManager::GetCurrentViewMatrix() const
{
  Assert("Active camera is a nullptr", m_activeCam != nullptr, ErrorLevel::SystemCrash);
  return m_activeCam->GetViewMatrix();
}

Mtx4 CameraManager::GetWaterReflectionMatrix(float waterHeight) const
{
  Assert("Active camera is a nullptr", m_activeCam != nullptr, ErrorLevel::SystemCrash);
  return m_activeCam->GetMatrixForWaterReflection(waterHeight);
}
