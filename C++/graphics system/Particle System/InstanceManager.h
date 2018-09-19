/******************************************************************************/
/*!
\file		InstanceManager.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef INSTANCE_MGR_H
#define INSTANCE_MGR_H

#include "InstanceRenderer.h"
#include "Components\ParticleEmitter_2.h"
#include "Utility\SharedQueue.h"
#include "Graphics\Shader.h"
#include "Utility/Singleton.h"
#include "Math\OMath.h"
#include <unordered_map>

using TempInstanceList = SharedQueue<EmitterRenderInfo>;
using InstanceList = std::vector<EmitterRenderInfo>;

class InstanceManager : public Singleton<InstanceManager>
{
public:
  void Init();
  void Draw(const Mtx4& proj, const Mtx4& view);
  void AddInstance(const EmitterRenderInfo& inst);

private:
  Shader* m_instanceShader;
  TempInstanceList m_tempInstanceList;
  InstanceList m_instances;
  InstanceRenderer m_renderer;
};

#endif

