#pragma once
#include <atomic>

enum NodeState : unsigned char
{
  Clean = 0,      //00
  iFlag,          //01
  dFlag,          //10
  marked          //11
};

//template needed to convert ptr to 32 or 64 bit depending on machine
template <typename UnsignedType>
struct ConvertTemplate
{
  template <typename Ptr>
  static Ptr mark_ptr(Ptr ptr, NodeState state)
  {
    //getting the correct ptr based on machine
    auto val = reinterpret_cast<UnsignedType>(ptr);

    //editing the last two bits of the ptr: since Node is at minimum aligned to 4 bytes, last 2 bits
    //is guaranteed to be zero(assuming machine is word-addressable), so we can use it to store state
    if (state == iFlag)
    {
      val |= 0x1;
    }
    else if (state == dFlag)
    {
      val |= 0x2;
    }
    else if(state == marked)
    {
      val |= 0x3;
    }

    //dont do anything if state is clean
    return reinterpret_cast<Ptr>(val);
  }

  template <typename Ptr>
  static Ptr remove_mark(Ptr ptr)
  {
    auto val = reinterpret_cast<UnsignedType>(ptr);
    UnsignedType mask = 3;
    mask = ~mask;         //invert mask so everything 1 and last 2 bits is 0
    val &= mask;          //& with the ptr so last 2 bits becomes 0, removing the mark
    return reinterpret_cast<Ptr>(val);
  }

  template <typename Ptr>
  static NodeState get_mark(Ptr ptr)
  {
    auto val = reinterpret_cast<UnsignedType>(ptr);
    UnsignedType mask = 3;
    return static_cast<NodeState>(val & mask);
  }
};

//specializations to act as a wrapper around correct type for ConvertTemplate
template<unsigned>
struct ConvertPtr;

template<>
struct ConvertPtr<4u> : ConvertTemplate<unsigned int>
{};

template<>
struct ConvertPtr<8u> : ConvertTemplate<unsigned long long>
{};


template <typename T>
struct AtomicStateReference
{
private:
  std::atomic<T*> mPtr;
  using Convert = ConvertPtr<sizeof(T*)>;     //checks machine ptr type

public:
  //constructor
  AtomicStateReference(T* ptr = nullptr, NodeState state = Clean):
    mPtr{ Convert::mark_ptr(ptr, state) }
  {}

  //assign ptr
  AtomicStateReference& operator=(const AtomicStateReference& rhs)
  {
    mPtr.store(rhs.mPtr.load());
    return *this;
  }

  //return ptr with mark removed for normal use
  T* getRef() const
  {
    return Convert::remove_mark(mPtr.load());
  }

  T* get(NodeState& s) const
  {
    s = Convert::get_mark(mPtr.load());
    return Convert::remove_mark(mPtr.load());
  }

  NodeState getState() const
  {
    return Convert::get_mark(mPtr.load());
  }

  //compare and set both mark and state atomically
  bool CAS(T* expectedRef, T* newRef, NodeState expectedState, NodeState newState)
  {
    T* expected_ptr = Convert::mark_ptr(expectedRef, expectedState);
    T* new_ptr = Convert::mark_ptr(newRef, newState);

    return mPtr.compare_exchange_weak(expected_ptr, new_ptr);
  }

};
