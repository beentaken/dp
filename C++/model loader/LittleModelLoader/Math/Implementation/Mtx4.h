#ifndef _MTX_4_H
#define _MTX_4_H
#pragma once

#include "assimp\matrix4x4.h"

class Vec4;
class Vec3;
class Mtx3;
//row-major

struct Transform;
class Mtx4
{
public:
  float data[16];

  //constructors
  Mtx4(float a, float b, float c, float d,
    float e, float f, float g, float h,
    float i, float j, float k, float l,
    float m, float n, float o, float p);

  Mtx4(); //identity
  Mtx4(const Mtx4& M);
  Mtx4(const aiMatrix4x4& aM);
  Mtx4(const Mtx3& L, const Vec3& trans = Vec3());
  Mtx4& operator=(const Mtx4& M);

  //static functions
  static Mtx4 zero();
  static Mtx4 roll(float degrees);
  static Mtx4 yaw(float degrees);
  static Mtx4 pitch(float degrees);
  static Mtx4 scale(Vec3 const & v);
  static Mtx4 scale(float x, float y, float z);
  static Mtx4 translate(float x, float y, float z);
  static Mtx4 translate(const Vec3& v);
  static Mtx4 axisangle(const float& angleInDegrees, const Vec3& axis);
  static Mtx4 world(const Transform& trans);
  static Mtx4 Bias();
  static Mtx4 glortho(float left, float right, float bottom, float top, float nearZ, float farZ);
  static Mtx4 glperspective(float fovX, float aspect, float n, float f);
  static Mtx4 glLookAtRH(const Vec3& eye, const Vec3& target, const Vec3& relup);

  //assuming that matrix is a transformation (aka affine/orthonormal) matrix only
  Mtx3 getNormalizedCoordinateSystem() const;
  Vec3 getLengthOfCoordinateVectorsInMatrix() const;
  Vec3 getTranslation() const;
  Vec3 getForward() const;
  Vec3 getBack() const;
  Vec3 getUp() const;
  Vec3 getDown() const;
  Vec3 getRight() const;
  Vec3 getLeft() const;

  //member functions
  Mtx4 transpose() const;
  Mtx4 inverse() const;
  Mtx4 affineInverse() const;
  float& operator()(int r, int c);
  float operator()(int r, int c) const;
  Vec4 row(int r) const;
  Vec4 col(int c) const;
  Mtx4 operator*(const Mtx4& B) const;
  Mtx4 operator*(const float s) const;
  Mtx4 operator/(const float s) const;
  Vec4 operator*(const Vec4& v) const;
};

//print matrix
std::ostream& operator<<(std::ostream& os, const Mtx4& M);

#endif

