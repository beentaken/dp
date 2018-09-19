/******************************************************************************/
/*!
\File name: Vec3.h
\Project name: 
\Primary Author: 
All content © 2017 DigiPen (SINGAPORE) Corporation, All rights reserved.
*/
/******************************************************************************/
#ifndef _VEC3_H
#define _VEC3_H
#include <iostream>
#include <cmath>
#include <limits> //numeric_limits
#include "assimp\vector3.h"

class Vec4;

class Vec3
{
public:
  Vec3();
  Vec3(const Vec3&);
  Vec3(float, float, float);
  ~Vec3();

  inline static Vec3 EX() { return Vec3(1, 0, 0); }
  inline static Vec3 EY() { return Vec3(0, 1, 0); }
  inline static Vec3 EZ() { return Vec3(0, 0, 1); }

  explicit Vec3(const Vec4& v);
  Vec3(const aiVector3D& a);

  Vec3& operator=(const Vec3&);
  Vec3& operator=(float*);
  Vec3& operator+=(const Vec3&);
  Vec3& operator+=(float*);
  Vec3& operator-=(const Vec3&);
  Vec3& operator-=(float*);
  Vec3& operator*=(float);
  Vec3& operator/=(float);

  Vec3 operator+(const Vec3&) const;
  Vec3 operator-(const Vec3&) const;
  Vec3 operator*(float) const;
  Vec3 operator/(float) const;
  Vec3 operator-() const;
  float& operator[](int);
  float operator[](int) const;

  Vec3 cross(const Vec3&) const;
  float dot(const Vec3&) const;
  float length() const;
  float squaredLength() const;
  bool sameDir(const Vec3&) const;
  bool perpendicular(const Vec3&) const;
  void normalize();
  void print();


  friend bool operator==(const Vec3&, const Vec3&);
  friend bool operator!=(const Vec3&, const Vec3&);
  friend Vec3 operator*(float, const Vec3&);

  Vec3& Vec3Set(const float &ix = 0.f, const float &iy = 0.f, const float &iz = 0.f);
  //void Deserialize(Json::Value*);
  //void Serialize(Json::Value*);

//variables
  float x;
  float y;
  float z;
};

Vec3 Vec3Normalise(const Vec3 &rhs);
Vec3 Vec3CrossProduct(const Vec3&lhs, const Vec3 &rhs);
Vec3 Lerp(const Vec3& start, const Vec3& end, float t);
float Vec3Length(const Vec3 &rhs);
float Vec3SquareLength(const Vec3 &rhs);
float Vec3Distance(const Vec3 &lhs, const Vec3 &rhs);
float Vec3SquareDistance(const Vec3 &lhs, const Vec3 &rhs);
float Vec3DotProduct(const Vec3 &lhs, const Vec3 &rhs);
float Vec3GetAngleBetween(const Vec3&lhs, const Vec3 &rhs);

//conversions
Vec4 ToVec4Point(const Vec3& rhs);
Vec4 ToVec4Vector(const Vec3& rhs);

//print vector
std::ostream& operator<<(std::ostream& os, const Vec3& M);

#endif
