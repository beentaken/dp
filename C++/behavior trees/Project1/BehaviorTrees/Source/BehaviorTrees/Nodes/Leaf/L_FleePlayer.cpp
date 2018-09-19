#include <Stdafx.h>
#include <Custom.h>
#include "shortcuts.h"

#define FLEE_TIME 2.0f

using namespace BT;


L_FleeData* L_FleePlayer::GetLocalBlackBoard(NodeData *nodedata_ptr)
{
  return nodedata_ptr->GetLocalBlackBoard<L_FleeData>();
}

void L_FleePlayer::InitialLocalBlackBoard(NodeData *nodedata_ptr)
{
  nodedata_ptr->InitialLocalBlackBoard<L_FleeData>();
}


void BT::L_FleePlayer::OnInitial(NodeData * nodedata_ptr)
{
  LeafNode::OnInitial(nodedata_ptr);
  INIT_LOCAL_BB(L_FleeData);
}

Status BT::L_FleePlayer::OnEnter(NodeData * nodedata_ptr)
{
  LeafNode::OnEnter(nodedata_ptr);

  L_FleeData* data = GET_LOCAL_BB(L_FleeData);
  data->elapsed = 0.0f;

  GameObject* self = GET_GO;
  self->SetSpeedStatus(TinySpeedStatus::TS_WALK);
  SetTinySpeed(self);

  return Status::BT_RUNNING;
}

void BT::L_FleePlayer::OnExit(NodeData * nodedata_ptr)
{
  LeafNode::OnExit(nodedata_ptr);
}

Status BT::L_FleePlayer::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  LeafNode::OnUpdate(dt, nodedata_ptr);
  GameObject* self = GET_GO;
  GameObject* player = g_database.Find(g_spawnmgr.playerID);
  L_FleeData* data = GET_LOCAL_BB(L_FleeData);

  //check if times up
  data->elapsed += dt;
  if (data->elapsed > FLEE_TIME)
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
    D3DXVec3Scale(&currentvel, &currentvel, mag * dt);

    //get desired velocity (self-player gives a vector leading away from the player)
    temp = (self->GetBody().GetPos()) - (player->GetBody().GetPos());
    mag = D3DXVec3Length(&temp);
    D3DXVECTOR3 desiredvel;
    D3DXVec3Normalize(&desiredvel, &temp);
    D3DXVec3Scale(&desiredvel, &desiredvel, mag * dt);

    //steering  = desired velocity - current velocity
    D3DXVECTOR3 result;
    D3DXVec3Subtract(&result, &desiredvel, &currentvel);

    D3DXVECTOR3 newtarget;
    D3DXVec3Add(&newtarget, &(self->GetMovement().GetTarget()), &result);

    //keep them within square
    if (newtarget.x < 0.0f) newtarget.x = 0.0f;
    if (newtarget.z < 0.0f) newtarget.z = 0.0f;
    if (newtarget.x > 1.0f) newtarget.x = 1.0f;
    if (newtarget.z > 1.0f) newtarget.z = 1.0f;

    //move away from player
    self->GetTiny().SetDiffuse(1.f, 1.f, 1.f);
    self->GetMovement().SetTarget(newtarget);

    return Status::BT_RUNNING;
  }
  else
  {
    return Status::BT_SUCCESS;
  }
}

Status BT::L_FleePlayer::OnSuspend(NodeData * nodedata_ptr)
{
  return LeafNode::OnSuspend(nodedata_ptr);
}