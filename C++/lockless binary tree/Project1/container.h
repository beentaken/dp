#pragma once

#include <functional>
#include <limits>
#include "ASR.h"

template <typename T>
struct Container
{
  using KeyType = typename std::hash<T>::result_type;
  static constexpr KeyType min = std::numeric_limits<KeyType>::min();
  static constexpr KeyType max = std::numeric_limits<KeyType>::max();

  virtual bool insert(const T&) = 0;
  virtual bool remove(const T&) = 0;
  virtual bool find(const T&) = 0;
};
