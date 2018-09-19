#include <Stdafx.h>

#define DELAY_TIME 0.2f

using namespace BT;

D_XDelayData* D_DelayXSeconds::GetLocalBlackBoard(NodeData *nodedata_ptr)
{
  return nodedata_ptr->GetLocalBlackBoard<D_XDelayData>();
}

void D_DelayXSeconds::InitialLocalBlackBoard(NodeData *nodedata_ptr)
{
  nodedata_ptr->InitialLocalBlackBoard<D_XDelayData>();
}

bool D_DelayXSeconds::DelayComplete(float dt, NodeData *nodedata_ptr)
{
  D_XDelayData *customdata = GetLocalBlackBoard(nodedata_ptr);

  if (customdata->timer < 0.0f)
    return true;

  customdata->timer -= dt;

  return false;
}

void D_DelayXSeconds::OnInitial(NodeData *nodedata_ptr)
{
  InterrupterNode::OnInitial(nodedata_ptr);

  InitialLocalBlackBoard(nodedata_ptr);
}

Status D_DelayXSeconds::OnEnter(NodeData *nodedata_ptr)
{
  // set timer
  D_XDelayData *customdata = GetLocalBlackBoard(nodedata_ptr);
  customdata->timer = DELAY_TIME;

  // run child
  InterrupterNode::OnEnter(nodedata_ptr);

  // tiny idle
  AgentBTData &data = nodedata_ptr->GetAgentData();
  GameObject *self = data.GetGameObject();
  if (self)
  {
    self->SetSpeedStatus(TinySpeedStatus::TS_IDLE);
    SetTinySpeed(self);
  }

  // suspend child
  nodedata_ptr->SetChildStatus(0, Status::BT_SUSPEND);

  return Status::BT_RUNNING;
}

void D_DelayXSeconds::OnExit(NodeData *nodedata_ptr)
{
}

Status D_DelayXSeconds::OnUpdate(float dt, NodeData *nodedata_ptr)
{
  Status child_status = nodedata_ptr->GetChildStatus(0);

  if (child_status == Status::BT_SUSPEND)
  {
    if (DelayComplete(dt, nodedata_ptr))
    {
      // resume child
      nodedata_ptr->SetChildStatus(0, Status::BT_READY);

      return Status::BT_SUSPEND;
    }

    return Status::BT_RUNNING;
  }
  else
    return child_status;
}


Status D_DelayXSeconds::OnSuspend(NodeData *nodedata_ptr)
{
  return InterrupterNode::OnSuspend(nodedata_ptr);
}