#pragma once

#include <BehaviorTrees/BehaviorTreesShared.h>

namespace BT
{
  //node data
  class L_TinySpawner : public LeafNode
  {
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
  private:
    void RandomSpawnType(NodeData * nodedata_ptr);
  };
}
