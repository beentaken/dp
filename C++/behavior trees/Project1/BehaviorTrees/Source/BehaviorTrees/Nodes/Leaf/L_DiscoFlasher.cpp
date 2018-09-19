#include <Stdafx.h>
#include <Custom.h>
#include "shortcuts.h"

using namespace BT;

void BT::L_DiscoFlasher::OnInitial(NodeData * nodedata_ptr)
{
  LeafNode::OnInitial(nodedata_ptr);
}

Status BT::L_DiscoFlasher::OnEnter(NodeData * nodedata_ptr)
{
  LeafNode::OnEnter(nodedata_ptr);
  GameObject* go = GET_GO;
  
  //random a color
  D3DXVECTOR3 color;
  color.x = g_random.RangeFloat();
  color.y = g_random.RangeFloat();
  color.z = g_random.RangeFloat();

  go->GetTiny().SetDiffuse(color.x, color.y, color.z);

  return Status::BT_SUCCESS;
}

void BT::L_DiscoFlasher::OnExit(NodeData * nodedata_ptr)
{
  LeafNode::OnExit(nodedata_ptr);
}

Status BT::L_DiscoFlasher::OnUpdate(float dt, NodeData * nodedata_ptr)
{
  LeafNode::OnUpdate(dt, nodedata_ptr);
  return Status::BT_SUCCESS;
}

Status BT::L_DiscoFlasher::OnSuspend(NodeData * nodedata_ptr)
{
  return LeafNode::OnSuspend(nodedata_ptr);
}
