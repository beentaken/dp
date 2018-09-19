#ifndef HAZARD_PTR_H
#define HAZARD_PTR_H
#pragma once

#include <list>
#include <array>
#include <atomic>
#include <utility>
#include <algorithm>
#include "thread_id.h"

namespace Hazard
{
  template <typename T, unsigned N, unsigned K>
  class Record
  {
    //items in retired state
    static thread_local std::list<T*> retire_list;

    //number of items retired
    static thread_local int retireCount;

    //record shared among threads
    std::array<std::atomic<T*>, N*K> HPRecord;

    static Record instance;

    Record()
    {
      for (auto& elem: HPRecord)
        elem.store(nullptr);
    }

    ~Record()
    {
      //Clear();
    }

  public:
    //singleton 
    static Record& get_instance()
    {
      return instance;
    }

    void Clear()
    {
      for (auto& elem : HPRecord)
      {
        elem = nullptr;
      }
    }

    void RetireNode(T* node)
    {
      retire_list.push_back(node);
      ++retireCount;
      int threshold = static_cast<int>(N << 1);
      if (retireCount > threshold)
        Scan();
    }

    std::atomic<T*>& GetHP(int i)
    {
      return HPRecord[Thread::ID * K + i];
    }

    void Scan()
    {
      //Stage 1 : copying all non-nullptrs from HPRecord
      T* p_list[N*K];
      int pCount = 0;
      for (auto& elem : HPRecord)
      {
        auto ptr = elem.load();
        if (ptr)
          p_list[pCount++] = ptr;
      }
      std::sort(p_list, p_list + pCount);

      //Stage 2 : Check if any node to be retired exist in HPRecord, if it does, dont delete
      auto tmplist = std::move(retire_list);
      retireCount = 0;

      while (!tmplist.empty())
      {
        auto& nodeptr = tmplist.front();
        tmplist.pop_front();

        if (!nodeptr)
          continue;

        if (std::binary_search(p_list, p_list + pCount, nodeptr))
        {
          retire_list.push_back(nodeptr);
          ++retireCount;
        }
        else
        {
          delete nodeptr;
        }

      }
    }

  };//end Record


  //static templatized member variables init
  template<typename T, unsigned N, unsigned K>
  thread_local int Hazard::Record<T, N, K>::retireCount = 0;

  template<typename T, unsigned N, unsigned K>
  thread_local std::list<T*> Hazard::Record<T, N, K>::retire_list;

  template<typename T, unsigned N, unsigned K>
  Hazard::Record<T, N, K> Hazard::Record<T, N, K>::instance;


}//end namespace Hazard



#endif

