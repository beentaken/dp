/******************************************************************************/
/*!
\File name: Vec2.h
\Project name: 
\Primary Author: 
All content © 2017 DigiPen (SINGAPORE) Corporation, All rights reserved.
*/
/******************************************************************************/
#ifndef _VEC2_H
#define _VEC2_H

#pragma warning(disable:4201)

#include <iostream>
#include <cmath>
#include <limits> //numeric_limits

class Vec2
{
public:
  Vec2();
  Vec2(const Vec2&);
  explicit Vec2(float, float);

  Vec2& operator=(const Vec2&);
  Vec2& operator=(float*);
  Vec2& operator+=(const Vec2&);
  Vec2& operator+=(float*);
  Vec2& operator-=(const Vec2&);
  Vec2& operator-=(float*);
  Vec2& operator*=(float);
  Vec2& operator/=(float);
  Vec2& setAllValues(float, float);

  Vec2 operator+(const Vec2&) const;
  Vec2 operator-(const Vec2&) const;
  Vec2 operator*(float) const;
  Vec2 operator/(float) const;
  Vec2 operator-() const;

  float operator[](int);
  float operator[](int) const;
  float dot(const Vec2&) const;
  float length() const;
  float squaredLength() const;

  bool sameDir(const Vec2&) const;
  bool perpendicular(const Vec2&) const;

  void normalize();
  void print();

  ~Vec2();

  friend bool operator==(const Vec2&, const Vec2&);
  friend bool operator!=(const Vec2&, const Vec2&);
  friend Vec2 operator*(float, const Vec2&);

  //private:
  float x;
  float y;
};

Vec2 Vec2Normalise(const Vec2 &rhs);
float Vec2Length(const Vec2 &rhs);
float Vec2SquareLength(const Vec2 &rhs);
float Vec2Distance(const Vec2 &lhs, const Vec2 &rhs);
float Vec2SquareDistance(const Vec2 &lhs, const Vec2 &rhs);
float Vec2DotProduct(const Vec2 &lhs, const Vec2 &rhs);
float Vec2GetAngleBetween(const Vec2&lhs, const Vec2 &rhs);

#endif
