/******************************************************************************/
/*!
\file		LightingManager.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef LIGHTING_MANAGER_H
#define LIGHTING_MANAGER_H

#include "Utility\Singleton.h"
#include "LightTypes.h"
#include "Math\OMath.h"
#include <memory>

class Shader;

class LightingManager : public Singleton<LightingManager>
{
public:
  //using LightPtr = std::unique_ptr<Light>;
  using LightPtr = Light*;
  using LightList = std::unordered_map<LightID, LightPtr>;
  
  //Light* AddLight(const Light& light);
  unsigned AddLight(Light* light);
  
  Light* GetLight(LightID id);
  bool CheckIfDirectionalLightExist();
  Light* GetDirectionalLight();
  Vec3 GetSunDirection() const;
  void RemoveLight(LightID id);
  void UpdateLight(LightID id);

  void Init();
  void Update();
  void SendAllLightPropertiesGL(const Shader& shader);
  void Clear();
  void EditorClear();
  void ClearCurrentLights();
  void Terminate();

  std::vector<Light*> GetAllActiveLights() const;

  //some hardcoded lights
  void TestLights();


  std::vector<std::string>& GetLightTypeNames();
  std::string GetLightTypeStr() const;

private:
  LightList m_lights;
  LightList m_bufferLights;
  LightID m_currentID;    
  LightID m_dirLightID;
  std::vector<Light*> m_activeLights;

  std::vector<std::string> m_lightTypeNames;
  std::string m_lightTypeStr;

  bool m_sunExists;
};


#endif

