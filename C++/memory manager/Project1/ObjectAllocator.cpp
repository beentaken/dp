/************************************************************************/
/*!
\file ObjectAllocator.cpp
\author Chan Yu Hong
\par email: yuhong.chan\@digipen.edu
\par DigiPen login: yuhong.chan
\par Course: CS280
\par Assignment #1
\date 02-02-2017
\brief
This file contains the implementation of ObjectAllocator

 Public Operations include:
  Constructor
  Destructor
  Allocate
  Free
  DumpMemoryInUse
  ValidatePages
  ImplementedExtraCredit
  FreeEmptyPages
  SetDebugState
  GetPageList
  GetFreeList
  GetConfig
  GetStats
  
 Hours spent on this assignment: 30hrs
 
 Specific portions that gave you the most trouble: 
  Understanding what to do
  memory signatures
  external headers

*/
/************************************************************************/
#include "ObjectAllocator.h"
#include <cstring> //memset, strlen, strcpy

/******************************************************************************/
/*!
  \brief
     Creates the ObjectManager per the specified values
     Throws an exception if the construction fails. (Memory allocation problem)
*/
/******************************************************************************/
// 
ObjectAllocator::ObjectAllocator(size_t ObjectSize, 
                                 const OAConfig & config) throw(OAException)
  :
  config_(config),    //use implicitly declared copy constructor
  freeList_(nullptr),
  pageList_(nullptr),
  offsetBetwObjs_(0),
  offsetToFirstObj_(0),
  offsetToHeaderFromObj_(0)
{
  //update object size from input
  stats_.ObjectSize_ = ObjectSize;
  
  //prevent warnings for conversions
  size_t padBytes = static_cast<size_t>(config_.PadBytes_);
  size_t alignment = static_cast<size_t>(config_.Alignment_);
  
  //calculate left and inter alignment if there is alignment required
  if (alignment > 1)
  {
    //add the size of the pagePtr + first header + first padding 
    size_t leftBlockSize = sizeof(void*) + config_.HBlockInfo_.size_ + 
                            padBytes;
    
    //if leftblock size is not a multiple of alignment
    if (leftBlockSize % alignment)
    {
      //if leftblock is greater than alignment, use modulus to calculate
      size_t leftalign = (leftBlockSize > alignment) ?
                          alignment - (leftBlockSize % alignment) :
      //else its alignment required - leftblock
                           alignment - leftBlockSize;
      config_.LeftAlignSize_ = static_cast<unsigned>(leftalign);
    }

    size_t midBlockSize = (2 * padBytes) + config_.HBlockInfo_.size_ 
                            + stats_.ObjectSize_;
    //if midblock size is not a multiple of alignment
    if (midBlockSize % alignment)
    {
      size_t interalign = (midBlockSize > alignment) ?
                          alignment - (midBlockSize % alignment) :
                          alignment - midBlockSize;

      config_.InterAlignSize_ = static_cast<unsigned>(interalign);
    }
  }
  
  //initialize other values in stats to zero
  stats_.FreeObjects_ = 0;
  stats_.ObjectsInUse_ = 0;
  stats_.PagesInUse_ = 0;
  stats_.MostObjects_ = 0;
  stats_.Allocations_ = 0;
  stats_.Deallocations_ = 0;

  //calculate number of bytes to reach first object
  offsetToFirstObj_ = sizeof(void*) + config_.LeftAlignSize_
    + config_.HBlockInfo_.size_ + config_.PadBytes_;

  //calculate the offset between each object in the page
  offsetBetwObjs_ = config_.HBlockInfo_.size_ + (config_.PadBytes_ * 2)
                    + stats_.ObjectSize_ + config_.InterAlignSize_;

  //calculate offset to header from object 
  offsetToHeaderFromObj_ = config_.PadBytes_ + config_.HBlockInfo_.size_;

  //calculate pagesize 
  stats_.PageSize_ = sizeof(void*) + config_.LeftAlignSize_ 
      + (config_.ObjectsPerPage_ * offsetBetwObjs_) - config_.InterAlignSize_;
  
  //make a new page, construct the freelist in the page and add page to pagelist
  CreateNewPage();
}

/******************************************************************************/
/*!
  \brief
     Destroys the ObjectManager (never throws)
*/
/******************************************************************************/
ObjectAllocator::~ObjectAllocator() throw()
{
  GenericObject* pageTrav = pageList_;
  //deallocate each page in pageList
  while (pageTrav)
  {
    //store the next page
    GenericObject* next = pageTrav->Next;
    //cast to char* for delete
    char* ptrToDelete = reinterpret_cast<char*>(pageTrav);
    //free the memory
    delete[] ptrToDelete;
    //move to next page
    pageTrav = next;
  }
}

/******************************************************************************/
/*!
  \brief
   Take an object from the free list and give it to the client (simulates new)
    Throws an exception if the object can't be allocated. 
   (Memory allocation problem)
*/
/******************************************************************************/
void* ObjectAllocator::Allocate(const char * label) throw(OAException)
{
  //if OA is disabled
  if (config_.UseCPPMemManager_)
  {
    //just dynamically allocate memory using C++ new
    char* obj;
    try
    {
      obj = new char[stats_.ObjectSize_];
      //update stats
      UpdateStatsForAllocation();
      return obj;
    }
    catch (std::bad_alloc&)
    {
      throw OAException(OAException::E_NO_MEMORY, 
      "allocate_new_page: No system memory available.");
    }
  }

  //the memory to return to the client
  GenericObject* memBlock = nullptr;
  //check if there are still free blocks in freelist
  if (!freeList_)
  {
    //make new page and allocate memory
    //throws a exception if reached max pages or not enough system memory
    CreateNewPage();
  }
  //take the first block
  memBlock = freeList_;

  //move the freeList to the next block
  freeList_ = freeList_->Next;

  //set this block as allocated memory
  char* ptr = reinterpret_cast<char*>(memBlock);
  SetMemorySignature(ptr, ALLOCATED_PATTERN, stats_.ObjectSize_);
  
  //update stats
  UpdateStatsForAllocation();

  //update the header for allocation
  UpdateHeaderForAllocation(ptr, label);

  //return a free block of memory to the client
  return memBlock;
}

/******************************************************************************/
/*!
  \brief
     Returns an object to the free list for the client (simulates delete)
     Throws an exception if the the object can't be freed. (Invalid object)
*/
/******************************************************************************/
void ObjectAllocator::Free(void* Object) throw(OAException)
{
  //if OA is disabled
  if (config_.UseCPPMemManager_)
  {
    //just delete using C++ new
    char* ptrTodelete = reinterpret_cast<char*>(Object);
    delete[] ptrTodelete;
    //update stats
    UpdateStatsForFree();
    //return back to client
    return;
  }

  if (config_.DebugOn_)
  {
    //check for double free
    if (isOnFreeList(reinterpret_cast<GenericObject*>(Object)))
    {
      throw OAException(OAException::E_MULTIPLE_FREE, 
      "FreeObject: Object has already been freed.");
    }
    
    //check for bad boundary
    //if (!CheckObjectAlignment(reinterpret_cast<GenericObject*>(Object)))
    if (!isOnBoundary(reinterpret_cast<GenericObject*>(Object)))
    {
      //throw exception : not on boundary
      throw OAException(OAException::E_BAD_BOUNDARY, 
      "validate_object: Object not on a boundary.");
    }

    //check corruption
    if (isBlockCorrupted(reinterpret_cast<char*>(Object)))
    {
      //throw exception : corrupted memory
      throw OAException(OAException::E_CORRUPTED_BLOCK, 
      "A heap has been corrupted.");
    }
  }
  
  //set memory signature of the object to unallocated
  char* ptr = reinterpret_cast<char*>(Object);
  SetMemorySignature(ptr, FREED_PATTERN, stats_.ObjectSize_);

  //update stats
  UpdateStatsForFree();

  //update the header for free
  UpdateHeaderForFree(ptr);

  //object that was freed is now the head of the list
  GenericObject* newhead = reinterpret_cast<GenericObject*>(ptr);

  newhead->Next = freeList_;
  freeList_ = newhead;
}

/******************************************************************************/
/*!
  \brief
     Calls the callback fn for each block still in use
*/
/******************************************************************************/
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
  //if using C++ new delete
  if (config_.UseCPPMemManager_)
  {
    return 0;
  }

  //to count how many blocks in use
  unsigned count = 0;

  //go through the pageList
  GenericObject* trav = pageList_;
  while (trav)
  {
    //for each page, get to first obj into page
    char* objPtr = reinterpret_cast<char*>(trav);
    objPtr += offsetToFirstObj_;
    //go to every obj in the page;
    for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
    {
      //check if this object is not in the freelist
      if (!isOnFreeList(reinterpret_cast<GenericObject*>(objPtr)) )
      {
        //increment count
        ++count;
        //call the callback function 
        fn(objPtr, stats_.ObjectSize_);
      }
      //go to next object
      objPtr += offsetBetwObjs_;
    }
    //go to next page
    trav = trav->Next;
  }

  return count;
}
/******************************************************************************/
/*!
  \brief
     Calls the callback fn for each block that is potentially corrupted
*/
/******************************************************************************/
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
  //if debug is not on or padding 0
  if (config_.DebugOn_ == false || config_.PadBytes_ == 0)
  {
    //no need to check;
    return 0;
  }

  //to count how many blocks corrupted
  unsigned count = 0;

  //go through the pageList
  GenericObject* trav = pageList_;
  while (trav)
  {
    //for each page, get to first obj into page
    char* objPtr = reinterpret_cast<char*>(trav);
    objPtr += offsetToFirstObj_;
    //go to every obj in the page;
    for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
    {
      //check the padding of this object
      if (isBlockCorrupted(objPtr) )
      {
        //increment count
        ++count;
        //call the callback function 
        fn(objPtr, stats_.ObjectSize_);
      }
      //go to next object
      objPtr += offsetBetwObjs_;
    }
    //go to next page
    trav = trav->Next;
  }

  return count;
}
/******************************************************************************/
/*!
  \brief
    Frees all empty pages (extra credit)   
*/
/******************************************************************************/
unsigned ObjectAllocator::FreeEmptyPages(void)
{
  //count number of blocks free
  unsigned count = 0;

  //go through pagelist
  GenericObject* trav = pageList_;

  while (trav)
  {
    //keeps track of how many blocks is free
    unsigned freeCount = 0;
    char* objPtr = reinterpret_cast<char*>(trav);
    
    //get to first obj
    objPtr += offsetToFirstObj_;
    
    //check if each block is on the freelist
    for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
    {
      if (isOnFreeList(reinterpret_cast<GenericObject*>(objPtr)) )
      {
        //increment number of free blocks for this page
        ++freeCount;
      }
      //go to next obj
      objPtr += offsetBetwObjs_;
    }

    //if all on the freelist, go through the page
    if (freeCount == config_.ObjectsPerPage_)
    {
      objPtr = reinterpret_cast<char*>(trav);
      //get to first object
      objPtr += offsetToFirstObj_;
      //go through each block, remove each block from freelist
      for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
      {
        RemoveFromFreeList(objPtr);
        //go to next obj
        objPtr += offsetBetwObjs_;
        //decrement number of free objects
        --stats_.FreeObjects_;
      }
      //Free this  page, getting back the ptr to the next page
      trav = RemovePage(trav);
      //increment number of pages freed
      ++count;
      //reduce number of pages in use
      --stats_.PagesInUse_;
      //do not go to the next page, go straight to next iteration
      continue;
    }

    //go to next page
    trav = trav->Next;
  }

  //return number of blocks freed
  return count;
}
////////////////////////////////////////////////////////////////////////////////
//getters and other functions
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/*!
  \brief
    Sets the debug state in the object allocator
*/
/******************************************************************************/
void ObjectAllocator::SetDebugState(bool State)
{
  config_.DebugOn_ = State;
}
/******************************************************************************/
/*!
  \brief
    Returns a pointer to the freeList in the object allocator
*/
/******************************************************************************/
const void * ObjectAllocator::GetFreeList(void) const
{
  return freeList_;
}
/******************************************************************************/
/*!
  \brief
    Returns a pointer to the pagelist in the object allocator
*/
/******************************************************************************/
const void * ObjectAllocator::GetPageList(void) const
{
  return pageList_;
}
/******************************************************************************/
/*!
  \brief
    Returns a OAConfig object that contains the config of the
    current object allocator
*/
/******************************************************************************/
OAConfig ObjectAllocator::GetConfig(void) const
{
  return config_;
}
/******************************************************************************/
/*!
  \brief
    Return a OAStats object that contains the current statistics
    of the object allocator
*/
/******************************************************************************/
OAStats ObjectAllocator::GetStats(void) const
{
  return stats_;
}
/******************************************************************************/
/*!
  \brief
    Sets memory signature on a block of bytes.
*/
/******************************************************************************/
void ObjectAllocator::SetMemorySignature(char* ptr, unsigned char signature, 
                                         size_t noOfbytes)
{
  //if debug is on
  if (config_.DebugOn_)
  {
    //write the memory signature
    memset(ptr, signature, noOfbytes);
  }
}
/******************************************************************************/
/*!
  \brief
    Update the needed stats whenever the ObjectAllocator allocates
    a block of memory for the client
*/
/******************************************************************************/
void ObjectAllocator::UpdateStatsForAllocation()
{
  //update stats
  ++stats_.Allocations_;
  ++stats_.ObjectsInUse_;

  if (stats_.FreeObjects_ > 0)
  {
    --stats_.FreeObjects_;
  }

  //check if number of objects in use exceed maximum
  if (stats_.ObjectsInUse_ > stats_.MostObjects_)
  {
    stats_.MostObjects_ = stats_.ObjectsInUse_;
  }
}
/******************************************************************************/
/*!
  \brief
    Update the needed stats whenever the ObjectAllocator frees
    a block of memory for the client 
*/
/******************************************************************************/
void ObjectAllocator::UpdateStatsForFree()
{
  //update stats
  ++stats_.Deallocations_;

  if (stats_.ObjectsInUse_ > 0)
  {
    --stats_.ObjectsInUse_;
  }
  
  ++stats_.FreeObjects_;
}
/******************************************************************************/
/*!
  \brief
    Checks if a object is in the free list.
    Returns true if it is in the free list.
*/
/******************************************************************************/
bool ObjectAllocator::isOnFreeList(GenericObject* obj) const
{
  GenericObject* traverse = freeList_;
  while (traverse)
  {
    if (obj == traverse)
    {
      return true;
    }
    traverse = traverse->Next;
  }
  return false;
}
/******************************************************************************/
/*!
  \brief
  Checks if the ptr given by the client is on the object boundary
  Returns true if it is
*/
/******************************************************************************/
bool ObjectAllocator::isOnBoundary(GenericObject* obj) const
{
  //the head of the pageList
  GenericObject* pageTrav = pageList_;
  bool onBounds = false;

  //cast the given object to a char* for pointer comparison
  char* objToCheck = reinterpret_cast<char*>(obj);

  //check alignment by looking at each page
  while (pageTrav)
  {
    //check if ptr within page
    char* start = reinterpret_cast<char*>(pageTrav);
    char* end = start + stats_.PageSize_;
    //get a ptr to the first object in the page to compare with
    char* firstObj = start + offsetToFirstObj_;
    //if ptr is within bounds of this page
    if (objToCheck >= start && objToCheck < end)
    {
      //check if aligned with object
      unsigned ptr_diff = static_cast<unsigned>(objToCheck - firstObj);
      //if the object is on the boundary
      if (ptr_diff % offsetBetwObjs_ == 0)
      {
        onBounds = true;
        break;
      }
    }
    //move to the next page
    pageTrav = pageTrav->Next;
  }
  return onBounds;
}
/******************************************************************************/
/*!
  \brief
  Checks if the padding on the left and right of a object
  is corrupted
*/
/******************************************************************************/
bool ObjectAllocator::isBlockCorrupted(char* objPtr) const
{
  //if debugon is false or padding size is 0, return false
  if (config_.PadBytes_ == 0)
    return false;

  //cast to unsigned char to compare
  unsigned char* ptr = reinterpret_cast<unsigned char*>(objPtr);

  //start of left padding
  unsigned char* pLeftPad = ptr - config_.PadBytes_;
  //start of right right padding
  unsigned char* pRightPad = ptr + stats_.ObjectSize_;
  for (unsigned i = 0; i < config_.PadBytes_; ++i)
  {
    //check left padding
    if (*pLeftPad != PAD_PATTERN)
    {
      return true;
    }
    //check right padding
    if (*pRightPad != PAD_PATTERN)
    {
      return true;
    }
    ++pLeftPad;
    ++pRightPad;
  }
  
  //if both padding is not corrupted
  return false;
}
/******************************************************************************/
/*!
  \brief
    When creating a new page, initialize the entire page
    with memory signatures if Debug is on.
*/
/******************************************************************************/
void ObjectAllocator::InitializeMemorySignatures(char* ptr)
{
  //offset by the size of the page ptr to get to the left alignment
  ptr += sizeof(void*);
  //set memory signature for left alignment
  if(config_.DebugOn_)
    memset(ptr, ALIGN_PATTERN, config_.LeftAlignSize_);
  //offset by left alignment size to get to the first header
  ptr += config_.LeftAlignSize_;

  //set memory signatures for padding and alignment in the entire page
  for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i)
  {
    //for initialize, the header is all zero(nullptr if external header)
    memset(ptr, 0, config_.HBlockInfo_.size_);

    //offset by header size to get to left padding
    ptr += config_.HBlockInfo_.size_;

    //memset for left padding
    if (config_.DebugOn_)
      memset(ptr, PAD_PATTERN, config_.PadBytes_);

    //offset by padding size to get to object
    ptr += config_.PadBytes_;

    //memset for object(UNALLOCATED)
    if (config_.DebugOn_)
      memset(ptr, UNALLOCATED_PATTERN, stats_.ObjectSize_);

    //offset by object size to get to right padding
    ptr += stats_.ObjectSize_;

    //memset for right padding
    if (config_.DebugOn_)
      memset(ptr, PAD_PATTERN, config_.PadBytes_);

    //offset by padding size to get to internal alignment
    ptr += config_.PadBytes_;

    //only do internal alignment if not at last object
    if (i != config_.ObjectsPerPage_ - 1)
    {
      //memset for internal alignment
      if (config_.DebugOn_)
        memset(ptr, ALIGN_PATTERN, config_.InterAlignSize_);

      //offset by internal alignment size to get to next header
      ptr += config_.InterAlignSize_;
    }

  }//end for loop

}
/******************************************************************************/
/*!
  \brief
    Updates the header in the memory block when a block of memory is allocated
    to the client.
*/
/******************************************************************************/
void ObjectAllocator::UpdateHeaderForAllocation(char* objPtr, const char* label)
{
  //offset by the number of bytes between the header and the object
  char* headerPtr = objPtr - offsetToHeaderFromObj_;

  //check the type of the header 
  switch (config_.HBlockInfo_.type_)
  {
    //do nothing if no headers
    case OAConfig::hbNone:
    {
      break;
    }
    //for basic header blocks
    case OAConfig::hbBasic:
    {
      //update allocation counter
      unsigned* ptr = reinterpret_cast<unsigned*>(headerPtr);
      *ptr = stats_.Allocations_;
      //update the flag
      headerPtr += sizeof(unsigned);
      memset(headerPtr, 0x01, sizeof(char));
      break;
    }
    //extended header
    case OAConfig::hbExtended:
    {
      //skip the user defined bytes
      headerPtr += config_.HBlockInfo_.additional_;
      //increment the use count by one since we are allocating this block
      short* pUseCount = reinterpret_cast<short*>(headerPtr);
      ++*pUseCount;
      //move the ptr by the size of the use_count
      headerPtr += sizeof(short);
      //update allocation counter
      unsigned* ptr = reinterpret_cast<unsigned*>(headerPtr);
      *ptr = stats_.Allocations_;
      //update the flag
      headerPtr += sizeof(unsigned);
      memset(headerPtr, 0x01, sizeof(char));
      break;
    }
    //externalheader
    case OAConfig::hbExternal:
    {
      //create a new external header
      MemBlockInfo* newHeader;
      try
      {
        //allocate memory for external header
        newHeader = new MemBlockInfo();
        //configure external header
        newHeader->alloc_num = stats_.Allocations_;
        newHeader->in_use = true;
        newHeader->label = nullptr;
        //dynamically allocate the label in the header
        if (label)
        {
          newHeader->label = new char[std::strlen(label) + 1];
          //copy in the label
          std::strcpy(newHeader->label, label);
        }
        //get a ptr to the ptr to the header
        MemBlockInfo** ppHeader = reinterpret_cast<MemBlockInfo**>(headerPtr);
        //dereference the ptr created to write the address of the memory block 
        //createdinto the actual page that contains the ptr to the header
        *ppHeader = newHeader;
      }
      catch (std::bad_alloc&)
      {
        throw OAException(OAException::E_NO_MEMORY, 
        "allocate_new_page: No system memory available.");
      }
      break;
    }
    //default : do nothing
    default:
    {
      break;
    }
  }
}
/******************************************************************************/
/*!
  \brief
    Updates the header of the object when a block of memory is freed by 
    the client.
*/
/******************************************************************************/
void ObjectAllocator::UpdateHeaderForFree(char * objPtr)
{
  //offset by the number of bytes between the header and the object
  char* headerPtr = objPtr - offsetToHeaderFromObj_;

  //check the type of the header 
  switch (config_.HBlockInfo_.type_)
  {
    //do nothing if no headers
    case OAConfig::hbNone:
    {
      break;
    }
    //for basic header blocks
    case OAConfig::hbBasic:
    {
      //update allocation counter
      unsigned* ptr = reinterpret_cast<unsigned*>(headerPtr);
      *ptr = 0;
      //update the flag
      headerPtr += sizeof(unsigned);
      memset(headerPtr, 0x00, sizeof(char));
      break;
    }
    //extended header
    case OAConfig::hbExtended:
    {
      //skip the user defined bytes and use count
      headerPtr += config_.HBlockInfo_.additional_ + sizeof(short);
      //memset the rest to zero
      memset(headerPtr, 0x00, sizeof(char) + sizeof(unsigned) );
      break;
    }
    //externalheader
    case OAConfig::hbExternal:
    {
      //free the label in the header 
      MemBlockInfo** ppHeader = reinterpret_cast<MemBlockInfo**>(headerPtr);
      //delete the label 
      delete[] (*ppHeader)->label;
      //delete the header
      delete *ppHeader;
      //set the ptr back to zeros (nullptr)
      memset(headerPtr, 0x00, OAConfig::EXTERNAL_HEADER_SIZE);
      break;
    }
    //default : do nothing
    default:
    {
      break;
    }
  }
}
/******************************************************************************/
/*!
  \brief
  Creates a new page and connects it to the pagelist.
*/
/******************************************************************************/
void ObjectAllocator::CreateNewPage()
{
  //check if we reached maximum number of pages. 0 means unlimited pages
  if (config_.MaxPages_)
  {
    if (stats_.PagesInUse_ == config_.MaxPages_)
    {
      //throw exception : out of pages
      throw OAException(OAException::E_NO_PAGES, 
      "Maximum number of pages reached");
    }
  }
  
  //try to allocate memory for a new page
  char* newPage;
  GenericObject* pagePtr;
  try
  {
    newPage = new char[stats_.PageSize_];
    pagePtr = reinterpret_cast<GenericObject*>(newPage);

    //initialize entire page with memory signatures 
    InitializeMemorySignatures(newPage);

    //create the freelist in the page
    CreateFreeList(newPage);

    //add the new page in front of the current page
    pagePtr->Next = pageList_;
    pageList_ = pagePtr;
    //increment number of pages allocated
    ++stats_.PagesInUse_;
  }
  catch (std::bad_alloc&)
  {
    throw OAException(OAException::E_NO_MEMORY, 
    "allocate_new_page: No system memory available.");
  }
}
/******************************************************************************/
/*!
\brief
  Removes a node from the freelist
*/
/******************************************************************************/
void ObjectAllocator::RemoveFromFreeList(char* objPtr)
{
  //if freelist does not exist return
  if (!freeList_)
    return;

  GenericObject* objNode = reinterpret_cast<GenericObject*>(objPtr);
  GenericObject* trav = freeList_;
  //keep track of previous node
  GenericObject* prev = trav;

  //check if the first object is the obj to be removed
  if (trav == objNode)
  {
    //remove the first node
    freeList_ = trav->Next;
  }

  //go through the freelist
  while (trav)
  {
    //if we found the object to delete
    if (trav == objNode)
    {
      //connect previous node to next node,
      //effectively removing this node
      prev->Next = trav->Next;
      //exit the loop
      break;
    }
    prev = trav;
    trav = trav->Next;
  }
}
/******************************************************************************/
/*!
\brief
  Frees a page from the pageList
*/
/******************************************************************************/
GenericObject* ObjectAllocator::RemovePage(GenericObject * pagePtr)
{
  char* pageToDelete;
  GenericObject* temp;

  //if page to be freed is the first page
  if (pageList_ == pagePtr)
  {
    //get a ptr to the next page
    temp = pageList_->Next;

    //delete the page
    pageToDelete = reinterpret_cast<char*>(pageList_);
    delete[] pageToDelete;

    //pageList now points to the next page
    pageList_ = temp;

    //return the next page
    return temp;
  }
  else //not the first page, need to find the page
  {
    GenericObject* trav = pageList_;
    GenericObject* prev = trav;
    //go through all the pages
    while (trav)
    {
      //if we found the page
      if (trav == pagePtr)
      {
        //get a ptr to the next page
        temp = trav->Next;
        //delete the page
        pageToDelete = reinterpret_cast<char*>(trav);
        delete[] pageToDelete;
        //connect previous page to next page
        prev->Next = temp;
        //found the page so return the next page
        return temp;
      }
      prev = trav;
      //go to the next page
      trav = trav->Next;
    }
  }

  //if no page was deleted, return back the original page(suppress warnings)
  return pagePtr;
}
/******************************************************************************/
/*!
  \brief
    Creates a new free list in a empty page. 
*/
/******************************************************************************/
void ObjectAllocator::CreateFreeList(char* emptypage)
{
  //get a pointer to the head of the freelist
  char* nextNode = emptypage + offsetToFirstObj_;
  //to return as the head of the list
  GenericObject* head = nullptr;
  //create nodes equal to the number of objects per page
  for (unsigned i = 0; i < config_.ObjectsPerPage_; ++i, 
                       nextNode += offsetBetwObjs_)
  {
    //get a new node to the next free block
    GenericObject* tempNode = reinterpret_cast<GenericObject*>(nextNode);
    //connect the new free block to the previous free block
    tempNode->Next = head;
    //move the head ptr to the new free block
    head = tempNode;
  }

  //update number of objects on freelist
  stats_.FreeObjects_ = config_.ObjectsPerPage_;

  //connect the new freelist
  freeList_ = head;
}
/******************************************************************************/
/*!
  \brief
  Returns true if extra credit is attempted. 
*/
/******************************************************************************/
bool ObjectAllocator::ImplementedExtraCredit(void)
{
  return true;
}