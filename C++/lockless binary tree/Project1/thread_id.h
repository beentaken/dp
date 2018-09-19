#ifndef THREAD_ID_H
#define THREAD_ID_H
#pragma once

#include <atomic>

namespace Thread
{
  thread_local int ID;
}

int GetThreadID(bool reset = false)
{
  static std::atomic<int> id{ 0 };
  if (reset)
  {
    id.store(0);
    return 0;
  }
  Thread::ID = id++;
  return Thread::ID;
}
#endif
