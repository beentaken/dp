#include <Stdafx.h>
#include <BehaviorTrees\Nodes\Leaf\shortcuts.h>

#define REPEAT_COUNT 5

using namespace BT;

void BT::D_RepeatXTimes::OnInitial(NodeData * nodedata_ptr)
{
  RepeaterNode::OnInitial(nodedata_ptr);
  INIT_LOCAL_BB(D_RepeatData);
}

Status BT::D_RepeatXTimes::OnEnter(NodeData * nodedata_ptr)
{
  D_RepeatData* data = GET_LOCAL_BB(D_RepeatData);
  data->count = 0;
  return Status::BT_RUNNING;
}

void BT::D_RepeatXTimes::OnExit(NodeData * nodedata_ptr)
{
  RepeaterNode::OnExit(nodedata_ptr);
}

Status BT::D_RepeatXTimes::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  UNREFERENCED_PARAMETER(dt);

  D_RepeatData* data = GET_LOCAL_BB(D_RepeatData);
  Status child_status = nodedata_ptr->GetChildStatus(0);

  if (data->count < REPEAT_COUNT)
  {
    ResetChildReturnStatus(nodedata_ptr);
    RunChild(true, false, nodedata_ptr);
    ++data->count;
    return Status::BT_RUNNING;
  }

  return Status::BT_SUCCESS;
}

Status BT::D_RepeatXTimes::OnSuspend(NodeData * nodedata_ptr)
{
  Status child_status = nodedata_ptr->GetChildStatus(0);

  if ((child_status == BT_SUCCESS) || (child_status == BT_FAILURE))
    return Status::BT_RUNNING;

  return Status::BT_SUSPEND;
}