#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
  //node data

  struct D_XDelayData : public NodeAbstractData
  {
    float timer;
  };

  class D_DelayXSeconds : public InterrupterNode
  {
  public:
    // Get custom data.
    D_XDelayData *GetLocalBlackBoard(NodeData *nodedata_ptr);
    // Initial custom data.
    void InitialLocalBlackBoard(NodeData *nodedata_ptr);
    // If time is up.
    bool DelayComplete(float dt, NodeData *nodedata_ptr);

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

