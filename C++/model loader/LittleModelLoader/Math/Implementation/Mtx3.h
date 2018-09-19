#ifndef _MTX_3_H
#define _MTX_3_H
#pragma once

#include "MathDefines.h"

class Mtx4;
class Vec3;

//row major
class Mtx3 
{
public:
  float data[9];

  //constructors
  Mtx3(float a, float b, float c,
    float d, float e, float f,
    float g, float h, float i);
  Mtx3(const Mtx4& M);
  Mtx3(); //identity
  Mtx3(const Mtx3& M);
  Mtx3(const Vec3& right, const Vec3& up, const Vec3& forward);
  Mtx3& operator=(const Mtx3& M);

  //static functions
  static Mtx3 zero();
  static Mtx3 roll(float degrees);
  static Mtx3 yaw(float degrees);
  static Mtx3 pitch(float degrees);
  static Mtx3 scale(float x, float y, float z);
  
  //member functions
  Mtx3 transpose() const;
  Mtx3 inverse() const;
  float& operator()(int r, int c);
  float operator()(int r, int c) const;
  Vec3 row(int r) const;
  Vec3 col(int c) const;
  Mtx3 operator*(const Mtx3& B) const;
  Mtx3 operator*(const float s) const;
  Mtx3 operator/(const float s) const;
  Vec3 operator*(const Vec3& v) const;
};

#endif
