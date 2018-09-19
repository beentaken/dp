#include <utility>
#include <iostream>
#include <cassert>

// #define NO_HASH

template<typename T, typename Hash>
FineGrainedBST<T, Hash>::FineGrainedBST(const Hash& fn):
  mHash(fn),
  mNumNodes(0)
{
  unsigned long long max_plus2 = static_cast<typename BSTNode<T>::KeyType>(FineGrainedBST::max) + 2;

  //empty tree
  mRoot = make_node(max_plus2, Internal);
  mRoot->left = make_node(max_plus2 - 1, Leaf);
  mRoot->right = make_node(max_plus2, Leaf);
}

template<typename T, typename Hash>
BSTNode<T>& FineGrainedBST<T, Hash>::get_root()
{
  return *mRoot;
}

template<typename T, typename Hash>
bool FineGrainedBST<T, Hash>::insert(const T & item)
{
#ifdef NO_HASH
  KeyType k = item; //mHash(item);
#else
  KeyType k = mHash(item);
#endif
  using KeyT = typename BSTNode<T>::KeyType;
  KeyT key = static_cast<KeyT>(k);

  NodePtr curr = mRoot->left;
  NodePtr prev = mRoot;

  prev->lock();
  curr->lock();
  while (curr)
  {
    //if at leaf node, attempt insert
    if (!curr->left && !curr->right)
    {
      assert(curr->mType == Leaf);

      //if duplicate, return not inserted
      if (curr->key == key)
      {
        prev->unlock();
        curr->unlock();
        return false;
      }

      //create the new internal node and link its 2 childs
      bool isGreater = key > curr->key;
      const KeyT& newKey = isGreater ? key : curr->key;
      const T& newValue = isGreater ? item : curr->mValue;

      NodePtr newInternalNode = make_node(newKey, Internal, newValue);
      //check if new item should be inserted as left or right child
      NodePtr& insertPoint = isGreater ? newInternalNode->right : newInternalNode->left;
      insertPoint = make_node(key, Leaf, item);
      insertPoint->mValid = true;

      //copy the old item(current node) as the other child
      NodePtr& otherChild = isGreater ? newInternalNode->left : newInternalNode->right;
      otherChild = curr;

      //connect the new internal node with the 2 childs to the list
      NodePtr& whichChild = (prev->left == curr) ? prev->left : prev->right;
      whichChild = newInternalNode;

      prev->unlock();
      curr->unlock();
      return true;
    }

    //unlock prev ptr before moving to next child
    prev->unlock();
    prev = curr;

    //move to next child
    if (key < curr->key)
      curr = curr->left;
    else
      curr = curr->right;

    //lock after moving to next ptr
    curr->lock();
  }
  
  //should never reach here
  assert(false);
  return false;
}

template<typename T, typename Hash>
bool FineGrainedBST<T, Hash>::remove(const T & item)
{
#ifdef NO_HASH
  KeyType k = item; //mHash(item);
#else
  KeyType k = mHash(item);
#endif
  using KeyT = typename BSTNode<T>::KeyType;
  KeyT key = static_cast<KeyT>(k);

  bool removed = false;
  NodePtr curr, prev, grandParent;
  prev = mRoot->left;
  grandParent = mRoot;

  //lock parents
  grandParent->lock();
  prev->lock();

  //initalize current node
  if (key < prev->key)
    curr = prev->left;
  else
    curr = prev->right;

  //traverse tree if not empty
  while (curr)
  {
    //if at a leaf node
    if (!curr->left && !curr->right)
    {
      assert(curr->mType == Leaf);

      removed = key == curr->key;
      if (removed)
      {
        //get ptr to sibling
        NodePtr& sibling = (prev->left == curr) ? prev->right : prev->left;

        //special case (node to delete is grandchild of root, it is always the left child)
        if (grandParent == mRoot)
          grandParent->left = sibling;
        else
        {
          //figure out linkage of parent
          NodePtr& whichParent = (grandParent->left == prev) ? grandParent->left : grandParent->right;

          //remove the parent of current node and replace linkage with sibling
          whichParent = sibling;
        }
        mNumNodes -= 2;
      }
    }

    //unlock parent and grandparent before moving to next child
    grandParent->unlock();
    grandParent = prev;
    prev = curr;
    prev->lock();

    if (key < curr->key)
      curr = curr->left;
    else
      curr = curr->right;
  }

  //unlock parents
  grandParent->unlock();
  prev->unlock();
  return removed;
}

template<typename T, typename Hash>
bool FineGrainedBST<T, Hash>::find(const T & item)
{
#ifdef NO_HASH
  KeyType k = item; //mHash(item);
#else
  KeyType k = mHash(item);
#endif
  using KeyT = typename BSTNode<T>::KeyType;
  KeyT key = static_cast<KeyT>(k);

  NodePtr curr = mRoot->left;
  NodePtr prev = mRoot;

  prev->lock();
  curr->lock();
  while (curr)
  {
    //if at leaf node and key matches, return true
    if (!curr->left && !curr->right)
    {
      bool result = (key == curr->key);
      prev->unlock();
      curr->unlock();
      return result;
    }

    //unlock prev ptr before moving to next child
    prev->unlock();
    prev = curr;

    //move to next child
    if (key < curr->key)
      curr = curr->left;
    else
      curr = curr->right;

    //lock after moving to next ptr
    curr->lock();
  }

  //should never come here
  assert(false);

  return false;
}

template<typename T, typename Hash>
void FineGrainedBST<T, Hash>::Print(std::ostream& os)
{
  //will never add on right subtree
  os << "--------------------------------------------------------------------------------------------------" << std::endl;
  os << "Total Nodes: " << mNumNodes - 2 << std::endl;
  RecursivePrint(os, mRoot->left, 0);
  os << "--------------------------------------------------------------------------------------------------" << std::endl;
}

template<typename T, typename Hash>
void FineGrainedBST<T, Hash>::RecursivePrint(std::ostream& os, NodePtr curr, size_t depth)
{
  if (!curr)
    return;

  os << "Depth: " << depth << *curr << std::endl;
  RecursivePrint(os, curr->left, depth + 1);
  RecursivePrint(os, curr->right, depth + 1);
}


template<typename T, typename Hash>
typename FineGrainedBST<T, Hash>::NodePtr FineGrainedBST<T, Hash>::
make_node(const typename BSTNode<T>::KeyType & key, NodeType type, const T& item)
{
  NodePtr node = std::make_shared<BSTNode<T>>();
  node->mType = type;
  node->left = nullptr;
  node->right = nullptr;
  node->key = key;
  node->mValue = item;
  node->mValid = false;

  ++mNumNodes;

  return node;
}
