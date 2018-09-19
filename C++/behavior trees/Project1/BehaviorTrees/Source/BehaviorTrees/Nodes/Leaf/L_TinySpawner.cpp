#include <Stdafx.h>
#include <Custom.h>
#include <BehaviorTrees\AgentBehaviors.h>
#include "shortcuts.h"

using namespace BT;

#define SPAWNCOUNT 1
#define SPAWNTIME  2.0f

void BT::L_TinySpawner::OnInitial(NodeData * nodedata_ptr)
{
  LeafNode::OnInitial(nodedata_ptr);
}

Status BT::L_TinySpawner::OnEnter(NodeData * nodedata_ptr)
{
  LeafNode::OnEnter(nodedata_ptr);

  //reset to white
  GET_GO->GetTiny().SetDiffuse(1.0f, 1.0f, 1.0f);

  //check if this spawner is alive
  objectID selfID = GET_GO->GetID();
  if (g_spawnmgr.isAlive(selfID) && g_spawnmgr.PlayerAlive())
  {
    //spawn 1 tiny everytime and somemore while cointoss is true
    RandomSpawnType(nodedata_ptr);
    while (g_random.CoinToss())
    {
      RandomSpawnType(nodedata_ptr);
    }
  }
  
  return Status::BT_SUCCESS;
}

void BT::L_TinySpawner::OnExit(NodeData * nodedata_ptr)
{
  LeafNode::OnExit(nodedata_ptr);
}

Status BT::L_TinySpawner::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  LeafNode::OnUpdate(dt, nodedata_ptr);

  return Status::BT_SUCCESS;
}

Status BT::L_TinySpawner::OnSuspend(NodeData * nodedata_ptr)
{
  return LeafNode::OnSuspend(nodedata_ptr);
}

void BT::L_TinySpawner::RandomSpawnType(NodeData * nodedata_ptr)
{
  int r = g_random.RangeInt(0, 9);

  Body& b = GET_BODY;
  D3DXVECTOR3 position = b.GetPos();
  D3DXVECTOR3 color(1.0f, 1.0f, 1.0f);

  //40% chance to spawn spawners if spawner has not reached max count
  if (g_spawnmgr.spawnercount < g_spawnmgr.max_spawner_count && r < 5)
  {
    char spawner[20] = "Spawner";
    g_spawnmgr.SpawnTiny(spawner, "SpawnerTree", position, color, 100, true);
  }
  else
  {
    //50% chance for attackers
    if (r > 4)
    {
      char attacker[20] = "Attacker";
      g_spawnmgr.SpawnTiny(attacker, "AttackerTree", position, color, 100, true);
    }
    else
    {
      char enemy[20] = "Enemy";
      g_spawnmgr.SpawnTiny(enemy, "EnemyTree", position, color, 100, true);
    }
  }
}
