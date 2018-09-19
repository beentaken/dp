/******************************************************************************/
/*!
\file		LightingManager.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "LightingManager.h"
#include "Graphics\ogl.h"

/*
Light* LightingManager::AddLight(const Light & light)
{
  //store directional light id
  if (light.type == Light::Type::Directional)
  {
    Assert("Directional light already exists", !m_sunExists, ErrorLevel::SystemCrash);
    m_dirLightID = m_currentID;
    m_sunExists = true;
  }

  auto& result = m_lights.emplace(std::make_pair(m_currentID, std::make_unique<Light>(light)));
  Assert("Failed to create light", result.second, ErrorLevel::SystemCrash);
  result.first->second.get()->id = m_currentID;
  m_currentID++;
  return result.first->second.get();
}
*/

unsigned LightingManager::AddLight(Light* light)
{
  auto& result = m_lights.emplace(m_currentID, light);
  Assert("Failed to create light", result.second, ErrorLevel::SystemCrash);
  result.first->second->id = m_currentID++;

  return result.first->second->id;
}

void LightingManager::UpdateLight(LightID id)
{
	//store directional light id
	Light* light = m_lights[id];
	if (m_dirLightID == id && light->type != Light::Type::Directional)
	{
		m_sunExists = false;
	}
	if (light->type == Light::Type::Directional)
	{
		if (!Assert("Directional light already exists", !m_sunExists, ErrorLevel::Illegal))
		{
			light->type = Light::Type::None;
			return;
		}
		m_dirLightID = id;
		m_sunExists = true;
	}

}

Light* LightingManager::GetLight(LightID id)
{
  LightList::iterator it = m_lights.find(id);
  Assert("unable to find light with id", it != m_lights.end(), ErrorLevel::SystemCrash);
  //return it->second.get();
  return it->second;
}

bool LightingManager::CheckIfDirectionalLightExist()
{
	return( m_lights.find(m_dirLightID) != m_lights.end());
}

Light * LightingManager::GetDirectionalLight()
{
  auto& it = m_lights.find(m_dirLightID);

  Assert("Unable to find Directional Light", it != m_lights.end(), ErrorLevel::SystemCrash);
  //return it->second.get();
  return it->second;
}

Vec3 LightingManager::GetSunDirection() const
{
  auto& it = m_lights.find(m_dirLightID);
  Assert("Unable to find Directional Light", it != m_lights.end(), ErrorLevel::SystemCrash);
  //return it->second.get()->lightDirection;
  return it->second->lightDirection;
}

void LightingManager::RemoveLight(LightID id)
{
	if (m_lights.find(id) != m_lights.end())
	{
		if (m_lights[id]->type == Light::Type::Directional)
			m_sunExists = false;
		m_lights.erase(id);
	}
  
}

void LightingManager::Init()
{
  m_sunExists = false;
  m_currentID = 0;
  m_dirLightID = -1;

  m_lightTypeNames.emplace_back("Directional");
  m_lightTypeNames.emplace_back("Point");
  m_lightTypeNames.emplace_back("Spot");
  m_lightTypeNames.emplace_back("None");
  m_lightTypeStr = "";

  for (auto& str : m_lightTypeNames)
  {
	  m_lightTypeStr += str;
	  m_lightTypeStr += '\0';
  }
}

void LightingManager::Update()
{
  m_activeLights.clear();
  m_activeLights.reserve(m_lights.size());

  for (auto& elem : m_lights)
  {
    if (elem.second->isActive)
    {
      m_activeLights.push_back(elem.second);
    }
  }

  //send all active light data to gl
  size_t active_count = m_activeLights.size();
  Assert("Game can only have a maximum of 100 lights, ask yuhong if need to change", active_count <= 100, ErrorLevel::SystemCrash);

  for (size_t i = 0; i < active_count; ++i)
  {
    //concatenate name of array in shader + [index] so uniform int can be set
    m_activeLights[i]->indexedName = std::string("u_lights") + "[" + std::to_string(i) + "].";
  }
}

void LightingManager::SendAllLightPropertiesGL(const Shader & shader)
{
  //send all active light data to gl
  size_t active_count = m_activeLights.size();
  Assert("Game can only have a maximum of 100 lights, ask yuhong if need to change", active_count <= 100, ErrorLevel::SystemCrash);

  for (size_t i = 0; i < active_count; ++i)
  {
    //concatenate name of array in shader + [index] so uniform int can be set
    m_activeLights[i]->SendLightDataToGL(shader);
  }

  shader.SetInt("activeLightCount", active_count);
}

void LightingManager::Clear()
{
	m_lights.clear();
	m_sunExists = false;
}

void LightingManager::EditorClear()
{
	auto itr = m_lights.begin();

	while (itr != m_lights.end())
	{
		m_bufferLights.emplace(itr->first, itr->second);
		++itr;
	}

	m_lights.clear();

	m_sunExists = false;
}

void LightingManager::ClearCurrentLights()
{
	m_lights.clear();
	m_sunExists = false;

	auto itr = m_bufferLights.begin();

	while (itr != m_bufferLights.end())
	{
		AddLight(itr->second);
		++itr;
	}

	m_bufferLights.clear();
}


void LightingManager::Terminate()
{
	EditorClear();
}

std::vector<Light*> LightingManager::GetAllActiveLights() const
{
  return m_activeLights;
}


//hard coded light test until components are serialized
static const Vec3 cubePos(0.f, 100.f, 0.f);
static const Vec3 spotLightPos(800.f, 100.f, 100.f);
void LightingManager::TestLights()
{
  //directional light
  //Light dirLight;
  Light* temp = new Light();
  Light& dirLight = *temp;
  dirLight.type = Light::Type::Directional;
  dirLight.color = Vec3(1.0f, 1.0f, 1.0f);
  dirLight.lightDirection = Vec3(-0.8f, -1.0f, 0.0f);
  dirLight.intensity = 0.4f;
  dirLight.isActive = true;

  //AddLight(dirLight);
  AddLight(temp);
  UpdateLight(temp->id);
  ////Light pointLight1;
  //temp = new Light();
  //Light& pointLight1 = *temp;
  //pointLight1.type = Light::Type::Point;
  //Vec3 lightpos = spotLightPos;
  //lightpos.y -= 50.f;
  //lightpos.x -= 200.f;
  //pointLight1.position = lightpos;
  //pointLight1.color = Vec3(0.f, 0.f, 1.f);
  //pointLight1.intensity = 1.f;
  //pointLight1.isActive = true;

  ////AddLight(pointLight1);
  //AddLight(temp);

  ////Light pointLight2;
  //temp = new Light();
  //Light& pointLight2 = *temp;
  //pointLight2.type = Light::Type::Point;
  //Vec3 lightpos2 = cubePos;
  //lightpos2.y -= 50.f;
  //lightpos2.x += 400.f;
  //pointLight2.position = lightpos2;
  //pointLight2.color = Vec3(1.f, 0.f, 0.f);
  //pointLight2.intensity = 1.f;
  //pointLight2.isActive = true;

  ////AddLight(pointLight2);
  //AddLight(temp);

  ////Light spotLight1;
  //temp = new Light();
  //Light& spotLight1 = *temp;
  //spotLight1.type = Light::Type::Spot;
  //spotLight1.position = spotLightPos;
  //spotLight1.color = Vec3(0.f, 1.f, 0.f);
  //spotLight1.intensity = 1.f;
  //spotLight1.lightDirection = Vec3(0.f, -1.f, 0.f);
  //spotLight1.innerCone = 32.5f;
  //spotLight1.outerCone = 47.5f;
  //spotLight1.isActive = true;

  ////AddLight(spotLight1);
  //AddLight(temp);
}

std::vector<std::string>& LightingManager::GetLightTypeNames()
{
	return m_lightTypeNames;
}

std::string LightingManager::GetLightTypeStr() const
{
	return m_lightTypeStr;
}
