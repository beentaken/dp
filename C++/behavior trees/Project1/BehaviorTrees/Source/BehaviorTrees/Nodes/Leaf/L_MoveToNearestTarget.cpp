#include <Stdafx.h>
#include <Custom.h>
#include "shortcuts.h"

using namespace BT;

void BT::L_MoveToNearestTarget::OnInitial(NodeData * nodedata_ptr)
{
  LeafNode::OnInitial(nodedata_ptr);
}

Status BT::L_MoveToNearestTarget::OnEnter(NodeData * nodedata_ptr)
{
  LeafNode::OnEnter(nodedata_ptr);

  GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();
  GameObject *nearest = GetClosestAgent(self);

  if (nearest)
  {
    self->SetTargetPOS(nearest->GetBody().GetPos());
    self->SetSpeedStatus(TinySpeedStatus::TS_WALK);
    SetTinySpeed(self);

    return Status::BT_RUNNING;
  }
  else
    return Status::BT_FAILURE;
}

void BT::L_MoveToNearestTarget::OnExit(NodeData * nodedata_ptr)
{
  LeafNode::OnExit(nodedata_ptr);
}

Status BT::L_MoveToNearestTarget::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  LeafNode::OnUpdate(dt, nodedata_ptr);

  GameObject *self = nodedata_ptr->GetAgentData().GetGameObject();

  if (IsNear(self->GetBody().GetPos(), self->GetTargetPOS()))
    return Status::BT_SUCCESS;

  return Status::BT_RUNNING;
}

Status BT::L_MoveToNearestTarget::OnSuspend(NodeData * nodedata_ptr)
{
  return LeafNode::OnSuspend(nodedata_ptr);
}
