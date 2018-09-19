#include <Stdafx.h>
#include <Custom.h>
#include <BehaviorTrees\AgentBehaviors.h>
#include "shortcuts.h"

using namespace BT;

void BT::L_PlayerCollision::OnInitial(NodeData * nodedata_ptr)
{
  LeafNode::OnInitial(nodedata_ptr);
}

Status BT::L_PlayerCollision::OnEnter(NodeData * nodedata_ptr)
{
  LeafNode::OnEnter(nodedata_ptr);

  return Status::BT_RUNNING;
}

void BT::L_PlayerCollision::OnExit(NodeData * nodedata_ptr)
{
  LeafNode::OnExit(nodedata_ptr);
}

Status BT::L_PlayerCollision::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  //dont check for collision if player not alive
  if (g_spawnmgr.PlayerAlive())
  {
    bool collided = false;
    LeafNode::OnUpdate(dt, nodedata_ptr);
    GameObject* self = GET_GO;
    objectID selfid = self->GetID();
    Body& body = GET_BODY;
    D3DXVECTOR3 selfpos = body.GetPos();
    self->GetTiny().SetDiffuse(0.0f, 1.0f, 0.0f);

    dbCompositionList list;
    g_database.ComposeList(list, OBJECT_NPC);
    for (dbCompositionList::iterator it = list.begin(); it != list.end(); ++it)
    {
      GameObject* other = *it;
      if (selfid == other->GetID())
      {
        continue;
      }
      D3DXVECTOR3 otherpos = other->GetBody().GetPos();
      if (IsNear(selfpos, otherpos, 0.05f))
      {
        collided = true;

        //change to red
        self->GetTiny().SetDiffuse(1.0f, 0.0f, 0.0f);

        //"destroy object"
        ++g_spawnmgr.killcount;
        g_spawnmgr.RemoveAgent(other);
      }
    }
  }
  return Status::BT_RUNNING;
}

Status BT::L_PlayerCollision::OnSuspend(NodeData * nodedata_ptr)
{
  return LeafNode::OnSuspend(nodedata_ptr);
}
