/************************************************************************/
/*!
\file ObjectAllocator.h
\author Chan Yu Hong
\par email: yuhong.chan\@digipen.edu
\par DigiPen login: yuhong.chan
\par Course: CS280
\par Assignment #1
\date 02-02-2016
\brief
This file contains the interface of the ObjectAllocator

 Hours spent on this assignment: 30hrs
 
 Specific portions that gave you the most trouble: 
  Understanding what to do
  memory signatures
  external headers

*/
/************************************************************************/
//---------------------------------------------------------------------------
#ifndef OBJECTALLOCATORH
#define OBJECTALLOCATORH
//---------------------------------------------------------------------------

#ifdef _MSC_VER
// suppress warning: C++ Exception Specification ignored
#pragma warning( disable : 4290 ) 
#endif

#include <string>
#include <iostream>

// If the client doesn't specify these:
static const int DEFAULT_OBJECTS_PER_PAGE = 4;  
static const int DEFAULT_MAX_PAGES = 3;
/******************************************************************************/
/*!
  \class OAException
  \brief  
    Exception class for the ObjectAllocator.
    When a exception is thrown by the ObjectAllocator, this class
    is constructed and thrown.
*/
/******************************************************************************/
class OAException
{
  public:
      // Possible exception codes
    enum OA_EXCEPTION 
    {
      // out of physical memory (operator new fails)
      E_NO_MEMORY,
      // out of logical memory (max pages has been reached)      
      E_NO_PAGES,
      // block address is on a page, but not on any block-boundary      
      E_BAD_BOUNDARY,
      // block has already been freed      
      E_MULTIPLE_FREE,  
      // block has been corrupted (pad bytes have been overwritten)
      E_CORRUPTED_BLOCK 
    };

    OAException(OA_EXCEPTION ErrCode, const std::string& Message) : 
      error_code_(ErrCode), 
      message_(Message) 
      {};

    virtual ~OAException() {
    }

    OA_EXCEPTION code(void) const { 
      return error_code_; 
    }

    virtual const char *what(void) const {
      return message_.c_str();
    }
  private:  
    OA_EXCEPTION error_code_;
    std::string message_;
};
/******************************************************************************/
/*!
  \class OAConfig
  \brief 
    Configuration parameters of the ObjectAllocator
    Includes header type, size, padding size, max pages allowed,
    whether debugging is enabled and size of alignment
*/
/******************************************************************************/
struct OAConfig
{
  // allocation number + flags
  static const size_t BASIC_HEADER_SIZE = sizeof(unsigned) + 1;
  // just a pointer  
  static const size_t EXTERNAL_HEADER_SIZE = sizeof(void*);     

  enum HBLOCK_TYPE{hbNone, hbBasic, hbExtended, hbExternal};
  struct HeaderBlockInfo
  {
    HBLOCK_TYPE type_;
    size_t size_;
    size_t additional_;
    HeaderBlockInfo(HBLOCK_TYPE type = hbNone, unsigned additional = 0) : 
      type_(type), 
      size_(0), 
      additional_(additional)
    {
      if (type_ == hbBasic)
        size_ = BASIC_HEADER_SIZE;
       // alloc # + use counter + flag byte + user-defined
      else if (type_ == hbExtended)
        size_ = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(char) 
        + additional_;
      else if (type_ == hbExternal)
        size_ = EXTERNAL_HEADER_SIZE;
    };
  };

  OAConfig(bool UseCPPMemManager = false,
           unsigned ObjectsPerPage = DEFAULT_OBJECTS_PER_PAGE, 
           unsigned MaxPages = DEFAULT_MAX_PAGES, 
           bool DebugOn = false, 
           unsigned PadBytes = 0,
           const HeaderBlockInfo &HBInfo = HeaderBlockInfo(),
           unsigned Alignment = 0) : UseCPPMemManager_(UseCPPMemManager),
                                     ObjectsPerPage_(ObjectsPerPage), 
                                     MaxPages_(MaxPages), 
                                     DebugOn_(DebugOn), 
                                     PadBytes_(PadBytes),
                                     HBlockInfo_(HBInfo),
                                     Alignment_(Alignment)
  {
    HBlockInfo_ = HBInfo;
    LeftAlignSize_ = 0;  
    InterAlignSize_ = 0;
  }
  // by-pass the functionality of the OA and use new/delete
  bool UseCPPMemManager_;
  // number of objects on each page  
  unsigned ObjectsPerPage_; 
  // maximum number of pages the OA can allocate (0=unlimited)
  unsigned MaxPages_;
  // enable/disable debugging code (signatures, checks, etc.)  
  bool DebugOn_;
  // size of the left/right padding for each block  
  unsigned PadBytes_;
  // size of the header for each block (0=no headers)  
  HeaderBlockInfo HBlockInfo_;
  // address alignment of each block  
  unsigned Alignment_;      
  // number of alignment bytes required to align first block
  unsigned LeftAlignSize_;
  // number of alignment bytes required between remaining blocks  
  unsigned InterAlignSize_; 
};
/******************************************************************************/
/*!
  \class OAStats
  \brief  
    Tracks stats of the ObjectAllocator
    Includes size of object, size of page, number of allocations/deallocations,
    free objects, most objects, number of pages in use.
*/
/******************************************************************************/
// ObjectAllocator statistical info
struct OAStats
{
  OAStats(void) : 
    ObjectSize_(0), 
    PageSize_(0), 
    FreeObjects_(0), 
    ObjectsInUse_(0), 
    PagesInUse_(0),
    MostObjects_(0), Allocations_(0), Deallocations_(0) {};

  size_t ObjectSize_;      // size of each object
  size_t PageSize_;        // size of a page including all headers, padding
  unsigned FreeObjects_;   // number of objects on the free list
  unsigned ObjectsInUse_;  // number of objects in use by client
  unsigned PagesInUse_;    // number of pages allocated
  unsigned MostObjects_;   // most objects in use by client at one time
  unsigned Allocations_;   // total requests to allocate memory
  unsigned Deallocations_; // total requests to free memory
};
/******************************************************************************/
/*!
  \class GenericObject
  \brief  
    Just a struct with a pointer to help with casting and
    creation of the pagelist and freelist.
    This allows us to easily treat raw objects as nodes in a linked list
*/
/******************************************************************************/
struct GenericObject
{
  GenericObject* Next;
};
/******************************************************************************/
/*!
  \class MemBlockInfo
  \brief  
    A external header that contains some statistics about the current block.
*/
/******************************************************************************/
struct MemBlockInfo
{
  bool in_use;        // Is the block free or in use?
  unsigned alloc_num; // The allocation number (count) of this block
  char *label;        // A dynamically allocated NUL-terminated string
};
/******************************************************************************/
/*!
  \class ObjectAllocator
  \brief  
    Functions include:
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

*/
/******************************************************************************/
// This memory manager class 
class ObjectAllocator
{
  public:
      // Defined by the client (pointer to a block, size of block)
    typedef void (*DUMPCALLBACK)(const void *, size_t);
    typedef void (*VALIDATECALLBACK)(const void *, size_t);

      // Predefined values for memory signatures
    static const unsigned char UNALLOCATED_PATTERN = 0xAA;
    static const unsigned char ALLOCATED_PATTERN = 0xBB;
    static const unsigned char FREED_PATTERN = 0xCC;
    static const unsigned char PAD_PATTERN = 0xDD;
    static const unsigned char ALIGN_PATTERN = 0xEE;

      // Creates the ObjectManager per the specified values
      // Throws an exception if the construction fails. 
      //(Memory allocation problem)
    ObjectAllocator(size_t ObjectSize, const OAConfig& config) 
                    throw(OAException);

      // Destroys the ObjectManager (never throws)
    ~ObjectAllocator() throw();

      // Take an object from the free list and give it to the client 
      //(simulates new)
      // Throws an exception if the object can't be allocated. 
      //(Memory allocation problem)
    void *Allocate(const char *label = 0) throw(OAException);

      // Returns an object to the free list for the client (simulates delete)
      // Throws an exception if the the object can't be freed. (Invalid object)
    void Free(void *Object) throw(OAException);

      // Calls the callback fn for each block still in use
    unsigned DumpMemoryInUse(DUMPCALLBACK fn) const;

      // Calls the callback fn for each block that is potentially corrupted
    unsigned ValidatePages(VALIDATECALLBACK fn) const;

      // Frees all empty pages (extra credit)
    unsigned FreeEmptyPages(void);

      // Returns true if FreeEmptyPages and alignments are implemented
    static bool ImplementedExtraCredit(void);

      // Testing/Debugging/Statistic methods
      
    // true=enable, false=disable
    void SetDebugState(bool State);
    // returns a pointer to the internal free list    
    const void *GetFreeList(void) const;
    // returns a pointer to the internal page list    
    const void *GetPageList(void) const;
    // returns the configuration parameters    
    OAConfig GetConfig(void) const;
    // returns the statistics for the allocator    
    OAStats GetStats(void) const;         

  private:

    // Make private to prevent copy construction and assignment
    ObjectAllocator(const ObjectAllocator &oa);
    ObjectAllocator &operator=(const ObjectAllocator &oa);
    
    // Other private fields and methods...

    //private variables
    OAConfig config_;
    OAStats stats_;
    GenericObject* freeList_;
    GenericObject* pageList_;
    size_t offsetBetwObjs_;
    size_t offsetToFirstObj_;
    size_t offsetToHeaderFromObj_;

    //private functions
    void CreateNewPage();
    void RemoveFromFreeList(char* objPtr);
    GenericObject* RemovePage(GenericObject* pagePtr);
    void CreateFreeList(char* emptypage);
    void SetMemorySignature(char* ptr, unsigned char signature, 
                            size_t noOfbytes);
    void UpdateStatsForAllocation();
    void UpdateStatsForFree();
    void InitializeMemorySignatures(char* page);
    void UpdateHeaderForAllocation(char* objPtr, const char* label);
    void UpdateHeaderForFree(char* objPtr);
    void InitializeHeaders(char* headerPtr);
    bool isOnFreeList(GenericObject* obj) const;
    bool isOnBoundary(GenericObject* obj) const;
    bool isBlockCorrupted(char* objPtr) const;


};

#endif
