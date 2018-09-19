#include <utility>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "LockFreeBST.h"

template<typename T, unsigned N, typename Hash>
LockFreeBST<T, N, Hash>::LockFreeBST(const Hash& fn) :
  mHash(fn),
  mNumNodes(0)
{
  using CastType = typename LFNode::KeyT;
  CastType max_plus2 = static_cast<CastType>(LockFreeBST::max) + 2;

  //empty tree
  NodePtr left = new LeafNode(max_plus2 - 1);
  NodePtr right = new LeafNode(max_plus2);
  mRoot = new InternalNode(max_plus2, nullptr, left, right);
  mNumNodes = 3;
}

template<typename T, unsigned N, typename Hash>
LockFreeBST<T, N, Hash>::~LockFreeBST()
{
  Clear();
}

template<typename T, unsigned N, typename Hash>
typename LockFreeBST<T, N, Hash>::LFNode& LockFreeBST<T, N, Hash>::get_root()
{
  return *mRoot;
}

template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::Clear()
{
  std::list<Info*> list;
  if (!mRoot)
    return;

  FreeNode(mRoot, list);

  size_t n = list.size();
  for (auto& elem : list)
  {
    delete elem;
  }
  mRoot = nullptr;
  mNumNodes = 0;
}

template<typename T, unsigned N, typename Hash>
bool LockFreeBST<T, N, Hash>::insert(const T & item)
{
  KeyType k = mHash(item);
  using KType = typename LFNode::KeyT;
  KType key = static_cast<KType>(k);
 
  SearchResult res;
  auto& nodesRecord = NodeHP::get_instance();
  auto& infoRecord = InfoHP::get_instance();

  //loop until insertion completes
  while (true)
  {
    Search(key, res);

    //hazards
#if 1
    nodesRecord.GetHP(0).store(res.node);
    nodesRecord.GetHP(1).store(res.parent);
    infoRecord.GetHP(0).store(res.parent_info.getRef());
#endif

    //if duplicate return
    if (res.node->key == key)
      return false;

    //if parent is not clean, help the other operation to complete
    if (res.parent_info.getState() != Clean)
      Help(res.parent_info);
    //if clean, attempt insert
    else
    {
      //create internal node that holds inserted node and old leaf (res.node)
      LeafPtr newLeaf = new LeafNode(key, item);
      LeafPtr newSibling = new LeafNode(res.node->key, res.node->mValue);
      bool isGreater = (key > res.node->key);
      LeafPtr& left = isGreater ? newSibling : newLeaf;
      LeafPtr& right = isGreater ? newLeaf : newSibling;
      InternalPtr newInternal = new InternalNode(std::max(res.node->key, key), nullptr, left, right);
      InsertInfo* iInfo = new InsertInfo(res.parent, newInternal, res.node);

      //if flagging for insertion is successful
      if (res.parent->infoptr.CAS(res.parent_info.getRef(), iInfo, res.parent_info.getState(), iFlag))
      {
        infoRecord.RetireNode(res.parent_info.getRef());
        InsertHelper(iInfo);
        mNumNodes.fetch_add(2);
        return true;
      }
      //Flagged by another thread, help to complete its operation before trying again
      else
      {
        Help(res.parent->infoptr);
      }
    }
  }

  //should never reach here
  return false;
}

template<typename T, unsigned N, typename Hash>
bool LockFreeBST<T, N, Hash>::remove(const T & item)
{
  KeyType k = mHash(item);
  using KType = typename LFNode::KeyT;
  KType key = static_cast<KType>(k);

  //find the item
  SearchResult res;
  auto& nodesRecord = NodeHP::get_instance();
  auto& infoRecord = InfoHP::get_instance();

  //loop till removal completes
  while (true)
  {
    Search(key, res);

    //hazards
#if 1
    nodesRecord.GetHP(0).store(res.node);
    nodesRecord.GetHP(1).store(res.parent);
    nodesRecord.GetHP(2).store(res.grandParent);
    infoRecord.GetHP(0).store(res.parent_info.getRef());
    infoRecord.GetHP(1).store(res.gP_info.getRef());
#endif

    //if key is not in the tree
    if (res.node->key != key)
      return false;

    //if grandparent or parent is not clean, help the other operation to complete
    if (res.gP_info.getState() != Clean)
      Help(res.gP_info);
    else if (res.parent_info.getState() != Clean)
      Help(res.parent_info);
    //attempt to remove
    else
    {
      DeleteInfo* dInfo = new DeleteInfo(res.grandParent, res.parent, res.node, res.parent_info);

      //try to flag grandparent node for removal
      if (res.grandParent->infoptr.CAS(res.gP_info.getRef(), dInfo, res.gP_info.getState(), dFlag))
      {
        DeleteHelper(dInfo);
        mNumNodes.fetch_sub(2);
        return true;
      }
      //some other thread has flagged it, help the operation
      else
      {
        Help(res.grandParent->infoptr);
      }
    }
  }

  //should never get here
  return false;
}

template<typename T, unsigned N, typename Hash>
bool LockFreeBST<T, N, Hash>::find(const T & item)
{
  KeyType k = mHash(item);
  typename LFNode::KeyT key = static_cast<typename LFNode::KeyT>(k);

  //find the item
  SearchResult res;
  Search(key, res);

  //only return true if parent is not marked for deletion
  NodeState state;
  res.parent_info.get(state);
  bool isItem = res.node->key == key;
  return isItem && (state != marked);
}

template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::Print(std::ostream& os)
{
  //will never add on right subtree
  os << "--------------------------------------------------------------------------------------------------" << std::endl;
  os << "Total Nodes: " << mNumNodes.load() - 2 << std::endl;

  InternalPtr ptr = dynamic_cast<InternalPtr>(mRoot);
  RecursivePrint(os, ptr->left, 0);
  os << "--------------------------------------------------------------------------------------------------" << std::endl;
}

template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::Search(typename LFNode::KeyT key, SearchResult& result)
{
  InternalPtr p, gp;
  NodePtr trav = mRoot;
  AtomPtr copy;
  p = gp = nullptr;

  //might not be valid if tree is "empty"
  result.gP_info = nullptr;

  //while not at a leaf node
  while (trav->mType == Internal)
  {
    gp = p;
    p = dynamic_cast<InternalPtr>(trav);
    assert(p);

    //copy infoptr if its a internal node
    result.gP_info = result.parent_info;
    result.parent_info = p->infoptr;

    copy = (key < p->key) ? p->left.load() : p->right.load();
    trav = copy.load();
  }

  //now at a leaf node, return values
  result.grandParent = gp;
  result.parent = p;
  result.node = dynamic_cast<LeafPtr>(copy.load());

  //maybe need to check if result.node is valid by checking if parent is already marked?
}

template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::Help(const ASR & asr)
{
  NodeState state;
  Info* info = asr.get(state);

  //decides which helper to use based on flag
  if (state == iFlag)
  {
    InsertInfo* iInfo = dynamic_cast<InsertInfo*>(info);
    InsertHelper(iInfo);
  }
  else if (state == dFlag)
  {
    DeleteInfo* dInfo = dynamic_cast<DeleteInfo*>(info);
    DeleteHelper(dInfo);
  }
  else if (state == marked)
  {
    DeleteInfo* dInfo = dynamic_cast<DeleteInfo*>(info);
    MarkedHelper(dInfo);
  }

  //do nothing if clean
}

template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::InsertHelper(InsertInfo* info)
{
  if (!info)
    return;

  auto& nodesRecord = NodeHP::get_instance();

  //change child to new internal node if insert is successful
  if (CAS_Child(info->parent, info->leaf, info->newInternal))
  {
    nodesRecord.RetireNode(info->leaf);
  }

  //attempt to remove insertion flag
  info->parent->infoptr.CAS(info, info, iFlag, Clean);;
}

template<typename T, unsigned N, typename Hash>
bool LockFreeBST<T, N, Hash>::DeleteHelper(DeleteInfo* info)
{
  if (!info)
    return false;

  //parent successfully marked 
  if (info->parent->infoptr.CAS(info->parent_info.getRef(), info, info->parent_info.getState(), marked))
  {
    MarkedHelper(info);
    return true;
  }
  //some other thread has modified. help the other thread
  else
  {
    Help(info->parent->infoptr);

    //remove the deletion flag on grandparent node and try to delete again
    info->grandParent->infoptr.CAS(info, info, dFlag, Clean);
    return false;
  }
}

template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::MarkedHelper(DeleteInfo* info)
{
  if (!info)
    return;

  auto& nodesRecord = NodeHP::get_instance();
  auto& infoRecord = InfoHP::get_instance();

  //get sibling of node
  NodePtr sibling = nullptr;
  NodePtr leaf = nullptr;
  if (info->parent->left == info->leaf)
  {
    sibling = info->parent->right;
    leaf = info->parent->left;
  }
  else
  {
    sibling = info->parent->left;
    leaf = info->parent->right;
  }
  assert(sibling);

  //remove parent and replace with sibling
  if (CAS_Child(info->grandParent, info->parent, sibling))
  {
    nodesRecord.RetireNode(info->parent);
    nodesRecord.RetireNode(leaf);
    infoRecord.RetireNode(info->parent->infoptr.getRef());
  }

  //remove deletion flag on grandparent
  info->grandParent->infoptr.CAS(info, info, dFlag, Clean);
}

template<typename T, unsigned N, typename Hash>
bool LockFreeBST<T, N, Hash>::CAS_Child(InternalPtr parent, NodePtr oldChild, NodePtr newChild)
{
  if (parent && newChild)
  {
    if (newChild->key < parent->key)
      return parent->left.compare_exchange_weak(oldChild, newChild);
    else
      return parent->right.compare_exchange_weak(oldChild, newChild);
  }
  else
    return false;
}

template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::FreeNode(NodePtr curr, std::list<Info*>& list)
{
  if (!curr)
    return;

  InternalPtr p = dynamic_cast<InternalPtr>(curr);
  if (p)
  {
    FreeNode(p->left.load(), list);
    FreeNode(p->right.load(), list);
    list.push_back(p->infoptr.getRef());
  }

  delete curr;
}

  
template<typename T, unsigned N, typename Hash>
void LockFreeBST<T, N, Hash>::RecursivePrint(std::ostream& os, NodePtr curr, size_t depth)
{
  if (!curr)
    return;

  std::string type = (curr->mType == Internal) ? "Internal" : "Leaf";

  os << "Depth: " << depth
    << "  Key: " << std::setw(10) << curr->key
    << "  Value: " << std::setw(8) << curr->mValue
    << "  Type: " << std::setw(8) << type
    << std::endl;

  //stop recursing if at leaf
  InternalPtr ptr = dynamic_cast<InternalPtr>(curr);
  if (ptr)
  {
    RecursivePrint(os, ptr->left, depth + 1);
    RecursivePrint(os, ptr->right, depth + 1);
  }

}


