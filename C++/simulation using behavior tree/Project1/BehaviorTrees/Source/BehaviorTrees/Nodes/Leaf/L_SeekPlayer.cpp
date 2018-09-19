#include <Stdafx.h>
#include <Custom.h>
#include "shortcuts.h"
#include <BehaviorTrees\AgentBehaviors.h>

#define SEEK_TIME 10.0f

using namespace BT;

L_SeekData* L_SeekPlayer::GetLocalBlackBoard(NodeData *nodedata_ptr)
{
  return nodedata_ptr->GetLocalBlackBoard<L_SeekData>();
}

void L_SeekPlayer::InitialLocalBlackBoard(NodeData *nodedata_ptr)
{
  nodedata_ptr->InitialLocalBlackBoard<L_SeekData>();
}

void BT::L_SeekPlayer::OnInitial(NodeData * nodedata_ptr)
{
  LeafNode::OnInitial(nodedata_ptr);
  INIT_LOCAL_BB(L_SeekData);
}

Status BT::L_SeekPlayer::OnEnter(NodeData * nodedata_ptr)
{
  LeafNode::OnEnter(nodedata_ptr);

  L_SeekData* data = GET_LOCAL_BB(L_SeekData);
  data->elapsed = 0.0f;

  GameObject* self = GET_GO;
  self->SetSpeedStatus(TinySpeedStatus::TS_WALK);
  SetTinySpeed(self);

  return Status::BT_RUNNING;
}

void BT::L_SeekPlayer::OnExit(NodeData * nodedata_ptr)
{
  LeafNode::OnExit(nodedata_ptr);
}

Status BT::L_SeekPlayer::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  LeafNode::OnUpdate(dt, nodedata_ptr);
  GameObject* self = GET_GO;
  GameObject* player = g_database.Find(g_spawnmgr.playerID);
  L_SeekData* data = GET_LOCAL_BB(L_SeekData);

  //check if times up
  data->elapsed += dt;
  if (data->elapsed > SEEK_TIME)
  {
    return Status::BT_SUCCESS;
  }

  //check if alive
  if (g_spawnmgr.isAlive(self->GetID()) && g_spawnmgr.PlayerAlive())
  {
    //get current velocity
    D3DXVECTOR3 temp = (self->GetMovement().GetTarget()) - (self->GetBody().GetPos());
    float mag = D3DXVec3Length(&temp);
    D3DXVECTOR3 currentvel;
    D3DXVec3Normalize(&currentvel, &temp);
    D3DXVec3Scale(&currentvel, &currentvel, mag);

    //get desired velocity
    temp = (player->GetBody().GetPos()) - (self->GetBody().GetPos());
    mag = D3DXVec3Length(&temp);
    if (mag < 0.05f)
    {
      //damage the player by lowering count before lost
      --g_spawnmgr.losecount;
      //select random position
      return Status::BT_SUCCESS;
    }
    D3DXVECTOR3 desiredvel;
    D3DXVec3Normalize(&desiredvel, &temp);
    D3DXVec3Scale(&desiredvel, &desiredvel, mag);

    //steering  = desired velocity - current velocity
    D3DXVECTOR3 result;
    D3DXVec3Subtract(&result, &desiredvel, &currentvel);

    D3DXVECTOR3 newtarget;
    D3DXVec3Add(&newtarget, &(self->GetMovement().GetTarget()), &result);

    //move towards player
    self->GetTiny().SetDiffuse(1.f, 0.f, 0.f);
    self->GetMovement().SetTarget(newtarget);

    return Status::BT_RUNNING;
  }
  else
  {
    return Status::BT_SUCCESS;
  }
}

Status BT::L_SeekPlayer::OnSuspend(NodeData * nodedata_ptr)
{
  return LeafNode::OnSuspend(nodedata_ptr);
}