#ifndef _O_MATH_H
#define _O_MATH_H
#include <algorithm>
#include "MathDefines.h"
#include "Implementation\quaternion.h"
#include "Implementation\Mtx4.h"
#include "Implementation\Mtx3.h"
#include "Implementation\Vec2.h"
#include "Implementation\Vec3.h"
#include "Implementation\Vec4.h"
#include <iomanip> //setprecision

//utility
namespace OMath
{
  template <typename T>
  T clamp(T x, T lower , T upper)
  {
    return (std::min)((std::max)(x, lower), upper);
  }

  template <typename T>
  T normalizeValue(T value, T min, T max)
  {
    int diff = max - min;
    return (diff == static_cast<T>(0)) ? value : (value - min) / (max - min);
  }

  template <typename T>
  std::string to_string_with_precision(const T a_value, const int n = 6)
  {
	  std::ostringstream out;
	  out << std::setprecision(n) << a_value;
	  return out.str();
  }

}


//glm foward declarations

//#include "glm/fwd.hpp"

#endif

