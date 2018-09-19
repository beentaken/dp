#include "Stdafx.h"
#include "shortcuts.h"
#include <Custom.h>

#define MAX_SCALE 3.0f
#define MAX_KILL_COUNT 200.f
#define SCALE_FACTOR 1.0f
#define PLAYER_INITIAL_SCALE 0.5f;

using namespace BT;

L_IncreaseSizeData *L_IncreaseSize::GetLocalBlackBoard(NodeData *nodedata_ptr)
{
  return nodedata_ptr->GetLocalBlackBoard<L_IncreaseSizeData>();
}

void L_IncreaseSize::InitialLocalBlackBoard(NodeData *nodedata_ptr)
{
  nodedata_ptr->InitialLocalBlackBoard<L_IncreaseSizeData>();
}

void BT::L_IncreaseSize::OnInitial(NodeData * nodedata_ptr)
{
  LeafNode::OnInitial(nodedata_ptr);
  INIT_LOCAL_BB(L_IncreaseSizeData);
}

Status BT::L_IncreaseSize::OnEnter(NodeData * nodedata_ptr)
{
  LeafNode::OnEnter(nodedata_ptr);
  L_IncreaseSizeData* p_data = GET_LOCAL_BB(L_IncreaseSizeData);

  //calculate target scale based on number of kills
  float scale = (static_cast<float>(g_spawnmgr.killcount) / MAX_KILL_COUNT) * MAX_SCALE;
  scale += PLAYER_INITIAL_SCALE;
  //check does not exceed max scale
  if (scale > MAX_SCALE)
    scale = MAX_SCALE;

  //if player is dead
  if (!g_spawnmgr.PlayerAlive())
  {
    //set to black
    GET_GO->GetTiny().SetDiffuse(0.0f, 0.0f, 0.0f);
    scale = PLAYER_INITIAL_SCALE;
  }

  p_data->targetScale = scale;

  return Status::BT_RUNNING;
}

void BT::L_IncreaseSize::OnExit(NodeData * nodedata_ptr)
{
  LeafNode::OnExit(nodedata_ptr);
}

Status BT::L_IncreaseSize::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  //shows the debug info in the game world (which node is running)
  LeafNode::OnUpdate(dt, nodedata_ptr);

  L_IncreaseSizeData* p = GET_LOCAL_BB(L_IncreaseSizeData);
  Body& b = GET_BODY;

  float currScale = b.GetScale();

  //if player is alive
  if (g_spawnmgr.PlayerAlive())
  {
    if (currScale <= p->targetScale)
    {
      currScale += (SCALE_FACTOR * dt);
      b.SetScale(currScale);
      return Status::BT_RUNNING;
    }
    else
    {
      return Status::BT_SUCCESS;
    }
  }
  else //player is dead
  {
    if (currScale >= p->targetScale)
    {
      currScale -= (SCALE_FACTOR * dt);
      b.SetScale(currScale);
      return Status::BT_RUNNING;
    }
    else
    {
      return Status::BT_SUCCESS;
    }
  }

  

}

Status BT::L_IncreaseSize::OnSuspend(NodeData * nodedata_ptr)
{
  return LeafNode::OnSuspend(nodedata_ptr);
}
