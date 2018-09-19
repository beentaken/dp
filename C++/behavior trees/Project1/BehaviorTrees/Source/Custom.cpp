#include "Stdafx.h"
#include "Custom.h"

#define DEFAULT_SCALE 0.3f

bool Custom::SpawnManager::SpawnTiny(char* agentname, const std::string& treename, 
                        D3DXVECTOR3 pos, D3DXVECTOR3 color, int health, bool randompos)
{
  bool created = false;
  //store type before suffixing
  std::string agenttype = agentname;

  if (randompos)
  {
    pos.x = g_random.RangeFloat();
    pos.z = g_random.RangeFloat();
  }
  pos.y = 0.0f;
  
  GameObject* npc = nullptr;
  //check if reached maximum number of tinys
  if (canSpawn())
  {
    //suffix name with spawncount based on type
    if (agenttype == "Spawner")
    {
      sprintf(agentname, "%s%d", agentname, spawnercount);
      ++spawnercount;
    }
    else if (agenttype == "Attacker")
    {
      sprintf(agentname, "%s%d", agentname, attackercount);
      ++attackercount;
    }
    else
    {
      sprintf(agentname, "%s%d", agentname, enemycount);
      ++enemycount;
    }

    //Create game object
    npc = new GameObject(g_database.GetNewObjectID(), OBJECT_NPC, agentname);
    created = true;

    npc->CreateBody(100, pos);
    npc->CreateMovement();

    TinyCreationData& data = Singleton<TinyCreationData>::GetInstance();
    npc->CreateTiny(data.p_multianim, data.p_tinys, data.p_SM, data.getTimeCurrent(),
      color.x, color.y, color.z);
    g_database.Store(*npc);

    g_trees.Register(agentname, treename);
    g_trees.GetAgentData(agentname).InitialTinyBlackBoard(npc);
  }
  else
  {
    //need to recycle dead tiny
    npc = Recycle(agenttype);
    if (npc)
    {
      created = true;
      Body& b = npc->GetBody();
      npc->GetTiny().SetDiffuse(color.x, color.y, color.z);
      b.SetPos(pos);
      b.SetHealth(100);
      b.SetScale(DEFAULT_SCALE);
    }
  }

  if (created)
  {
    objectID id = npc->GetID();
    //add id-data pair to map
    AData temp;
    temp.type = agenttype;
    temp.isAlive = true;
    ++alivecount;
    agentmap[id] = temp;
  }

  return created;
}

void Custom::SpawnManager::RemoveAgent(GameObject * go)
{
  objectID id = go->GetID();

  //reduce size to zero to show destruction
  go->GetBody().SetScale(0.f);
  go->GetBody().SetHealth(0);
  D3DXVECTOR3 graveyardpos(0.f, 0.f, 0.f);
  go->GetBody().SetPos(graveyardpos);

  //mark as dead in mgr
  AgentMapIter it = agentmap.find(id);
  if (it != agentmap.end())
  {
    AData& data = it->second;
    --alivecount;
    data.isAlive = false;
  }
  else
  {
    std::cout << "RemoveAgent:: Agent not found" << std::endl;
  }

}

bool Custom::SpawnManager::isAlive(objectID id)
{
  AgentMapIter it = agentmap.find(id);
  if(it != agentmap.end())
  {
    if (it->second.isAlive) 
      return true;
  }
  else
  {
    std::cout << "isAlive:: Agent not found" << std::endl;
  }
  return false;
}

GameObject * Custom::SpawnManager::Recycle(const std::string& type)
{
  for (auto& elem : agentmap)
  {
    if (elem.second.type == type && !elem.second.isAlive)
    {
      //set the object to alive
      elem.second.isAlive = true;
      //return the gameobject ptr
      GameObject* ptr = g_database.Find(elem.first);
      if (!ptr) 
        std::cout << "Recycle:: Cannot find id in GameObjectList" << std::endl;
      return ptr;
    }
  }
  std::cout << "Recycle:: No dead tinys with required type found!!" << std::endl;
  return nullptr;
}
