#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
  //node data

  struct L_IncreaseSizeData : public NodeAbstractData
  {
    float targetScale;
  };

  class L_IncreaseSize : public LeafNode
  {
  public:
    // Get custom data.
    L_IncreaseSizeData *GetLocalBlackBoard(NodeData *nodedata_ptr);
    // Initial custom data.
    void InitialLocalBlackBoard(NodeData *nodedata_ptr);
  protected:
    // Only run when initializing the node
    virtual void OnInitial(NodeData *nodedata_ptr) override;
    // Only run when entering the node
    virtual Status OnEnter(NodeData *nodedata_ptr) override;
    // Only run when exiting the node
    virtual void OnExit(NodeData *nodedata_ptr) override;
    // Run every frame
    virtual Status OnUpdate(float dt, NodeData *nodedata_ptr) override;
    // Only run when node is in suspended
    virtual Status OnSuspend(NodeData *nodedata_ptr) override;
  };
}


