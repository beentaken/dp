#include <Stdafx.h>
#include <BehaviorTrees/Nodes/Leaf/shortcuts.h>

using namespace BT;

void BT::D_AlwaysFail::OnInitial(NodeData * nodedata_ptr)
{
  RepeaterNode::OnInitial(nodedata_ptr);
}

Status BT::D_AlwaysFail::OnEnter(NodeData * nodedata_ptr)
{
  return RepeaterNode::OnEnter(nodedata_ptr);
}

void BT::D_AlwaysFail::OnExit(NodeData * nodedata_ptr)
{
  RepeaterNode::OnExit(nodedata_ptr);
}

Status BT::D_AlwaysFail::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  UNREFERENCED_PARAMETER(dt);

  Status child_status = nodedata_ptr->GetChildStatus(0);

  //always return fail
  if (child_status == BT_SUCCESS)
    return Status::BT_FAILURE;
  else if (child_status == BT_FAILURE)
    return Status::BT_FAILURE;

  return Status::BT_RUNNING;
}

Status BT::D_AlwaysFail::OnSuspend(NodeData * nodedata_ptr)
{
  return RepeaterNode::OnSuspend(nodedata_ptr);
}