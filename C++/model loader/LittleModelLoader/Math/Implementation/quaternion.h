#ifndef _QUATERNION_H
#define _QUATERNION_H

//basic quaternion class mostly used for 3D rotations

#include "Vec3.h"
#include "assimp\quaternion.h"

class Mtx4;

class Quaternion
{
public:
  static Quaternion identity();
  static Quaternion AngleAxis(float degrees, Vec3 axis);
  static Quaternion Euler(float rotX, float rotY, float rotZ);
  static Quaternion Euler(const Vec3& eulerAnglesInDegrees);
  static Quaternion FromToRotation(Vec3 from, Vec3 to);

  //spherical interpolation (where qstart and qend represents the absolute--NOT RELATIVE rotation)
  static Quaternion Slerp(const Quaternion& qstart, const Quaternion& qend, float t);

  //constructors
  Quaternion() : w(1.f), x(), y(), z() {}
  Quaternion(float W, float X, float Y, float Z) : w(W), x(X), y(Y), z(Z) {}
  Quaternion(const aiQuaternion& a);

  //construct from axis-angle rotation
  Quaternion(float angleinDegrees, Vec3 axis);

  //construct from rotation matrix only
  Quaternion(const Mtx4& rotmat);

  //construct from eulerangles
  Quaternion(float rotX, float rotY, float rotZ);
  Quaternion(const Vec3& eulerAngleInDegrees);
  Quaternion& Inverse();

  //operator* overloaded for rotation 
  Vec3 operator*(const Vec3& vec) const;

  Quaternion& Normalize();
  Quaternion& Conjugate();

  //returns a matrix that rotates about a point
  Mtx4 RotateAboutPoint(const Vec3& pt) const;

  //equivalence using tolerance
  bool Equals(const Quaternion& q, float epsilon = EPSILON) const;

  //convert to rotaton matrix
  Mtx4 ToMatrix() const;

  float Dot(const Quaternion& q) const;
  
  //get rotation as eulerangles
  float GetYaw() const;             //rotate around y
  float GetPitch() const;           //rotate around x
  float GetRoll() const;            //rotate z
  Vec3 GetEulerAngles() const;

  //operators not related to rotation
  Quaternion& operator+=(const Quaternion& rhs);
  Quaternion& operator-=(const Quaternion& rhs);
  Quaternion operator+(const Quaternion& rhs) const;
  Quaternion operator-(const Quaternion& rhs) const;
  Quaternion operator*(const Quaternion& q) const;
  Quaternion& operator*=(float scalar);
  Quaternion& operator*=(const Quaternion& rhs);
  Quaternion operator*(float scalar) const;
  //equivlaence operators
  bool operator==(const Quaternion& rhs) const;
  bool operator!=(const Quaternion& rhs) const;

  float w;
  float x;
  float y;
  float z;

private:
  
  float Norm() const;

};
#endif

