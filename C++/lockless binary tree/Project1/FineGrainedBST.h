#pragma once

#include "container.h"
#include <mutex>
#include <memory>
#include <iostream>
#include <iomanip>

enum NodeType
{
  Internal = 0,
  Leaf
};

template<typename T>
struct BSTNode
{
  //needs to be greater than hash value to support max+2 dummy nodes
  using KeyType = unsigned long long;
  BSTNode() = default;

  void lock()
  {
    lk.lock();
  }

  void unlock()
  {
    lk.unlock();
  }

  KeyType key;
  NodeType mType;
  T mValue;
  std::mutex lk;
  std::shared_ptr<BSTNode> left, right;
  bool mValid;
};

//debug print nodes
template <typename T>
std::ostream& operator<<(std::ostream& os, BSTNode<T>& node)
{
  os << std::boolalpha;
  std::string type = (node.mType == Internal) ? "Internal" : "Leaf";
  os
    << "  Key: " << std::setw(10) << node.key
    << "  Value: " << std::setw(8) << node.mValue
    << "  Type: " << std::setw(8) << type
    << "  Valid: " << std::setw(8) << node.mValid;
  return os;
}

template <typename T, typename Hash = std::hash<T>>
class FineGrainedBST: public Container<T>
{
public:
  using KeyType = typename Container<T>::KeyType;
  using NodePtr = std::shared_ptr<BSTNode<T>>;

  FineGrainedBST(const Hash& fn = Hash());
  BSTNode<T>& get_root();
  bool insert(const T& item) override;
  bool remove(const T& item) override;
  bool find(const T& item) override;
  void Print(std::ostream& os);

private:
  void RecursivePrint(std::ostream& os, NodePtr curr, size_t depth);

  NodePtr make_node(const typename BSTNode<T>::KeyType& key, NodeType type, const T& item = T());
  NodePtr mRoot;
  Hash mHash;
  size_t mNumNodes;
};

#include "FineGrainedBST.cpp"
