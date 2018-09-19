#pragma once
#include <memory>
#include <iostream>
#include <iomanip>
#include "ASR.h"
#include "container.h"
#include "hazardptr.h"

template <typename T, unsigned N = 16, typename Hash = std::hash<T>>
class LockFreeBST : public Container<T>
{
  enum NodeType
  {
    Internal = 0,
    Leaf
  };

  struct LFNode
  {
    using KeyT = unsigned long long;
    LFNode(KeyT k, NodeType t, const T& val = T()) :
      key(k),
      mValue(val),
      mType(t)
    {}
    virtual ~LFNode() {};

    //greater than hash value for dummy nodes(therefore ULL)
    KeyT key;
    T mValue;
    NodeType mType;
  };

  //base class that store data shared among threads to help complete insert/delete operations
  struct Info 
  {
    virtual ~Info(){}
  };

  using KeyType = typename Container<T>::KeyType;
  using AtomPtr = std::atomic<LFNode*>;
  using NodePtr = LFNode * ;
  using ASR = AtomicStateReference<Info>;
  using NodeHP = Hazard::Record<LFNode, N, 3>;
  using InfoHP = Hazard::Record<Info, N, 2>;

  //internal and leaf nodes havea different type as they store diferent data
  struct InternalNode : LFNode
  {
    InternalNode(typename LFNode::KeyT k, Info* info = nullptr, 
      const NodePtr& l = nullptr, const NodePtr& r = nullptr):
      LFNode(k, Internal),
      infoptr(info),
      left(l),
      right(r)
    {}
    ASR infoptr;
    AtomPtr left, right;
  };

  struct LeafNode : LFNode
  {
    LeafNode(typename LFNode::KeyT k , const T& v = T()) :
      LFNode(k, Leaf, v)
    {}
  };

  using InternalPtr = InternalNode * ;
  using LeafPtr = LeafNode * ;

  struct InsertInfo : Info
  {
    InsertInfo(const InternalPtr& p = nullptr, const InternalPtr& newI = nullptr, const LeafPtr& l = nullptr):
      parent(p),
      newInternal(newI),
      leaf(l)
    {}
    InternalPtr parent, newInternal;
    LeafPtr leaf;
  };

  struct DeleteInfo : Info
  {
    DeleteInfo(const InternalPtr& gp = nullptr, const InternalPtr& p = nullptr, const LeafPtr& l = nullptr,
      const ASR& asr = ASR()) :
      parent(p),
      grandParent(gp),
      leaf(l)
    {
      parent_info = asr;
    }

    InternalPtr parent, grandParent;
    LeafPtr leaf;
    ASR parent_info;
  };

  struct SearchResult
  {
    InternalPtr grandParent = nullptr;
    InternalPtr parent = nullptr;
    LeafPtr node = nullptr;
    ASR parent_info;
    ASR gP_info;
  };

public:
  LockFreeBST(const Hash& fn = Hash());
  ~LockFreeBST();
  LFNode& get_root();
  void Clear();
  bool insert(const T& item) override;
  bool remove(const T& item) override;
  bool find(const T& item) override;
  void Print(std::ostream& os);

  void CheckSize(std::ostream& os)
  {
    os << "Size of node: " << sizeof(LFNode) << std::endl;
    os << "Size of internal: " << sizeof(InternalNode) << std::endl;
    os << "Size of leaf: " << sizeof(LeafNode) << std::endl;
    os << "Size of insertinfo: " << sizeof(InsertInfo) << std::endl;
    os << "Size of deleteinfo: " << sizeof(DeleteInfo) << std::endl;
    os << "Size of ASR: " << sizeof(AtomicStateReference<Info>) << std::endl;
  }

private:
  void Search(typename LFNode::KeyT key, SearchResult& result);
  void Help(const ASR& asr);
  void InsertHelper(InsertInfo* info);
  bool DeleteHelper(DeleteInfo* info);
  void MarkedHelper(DeleteInfo* info);
  bool CAS_Child(InternalPtr parent, NodePtr oldChild, NodePtr newChild);
  void FreeNode(NodePtr curr, std::list<Info*>& list);
  void RecursivePrint(std::ostream& os, NodePtr curr, size_t depth);
  NodePtr mRoot;
  Hash mHash;
  std::atomic<size_t> mNumNodes;

};

#include "LockFreeBST.cpp"

