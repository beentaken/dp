/******************************************************************************/
/*!
\file		Vec4.h
\project	MetroManiac
\primary author Sia Hou Ping

All content \@ 2016 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once 
#include <ostream>

class Vec3;

class Vec4{
  public:
  float x,y,z,w;

  static Vec4 R();
  static Vec4 G();
  static Vec4 B();

  Vec4(): x(0),y(0),z(0),w(0){}
  Vec4( const float &a,const float &b = 0,const float &c=0, const float &d=0): x(a),y(b),z(c),w(d){}
  Vec4(const Vec4 &rhs);
  Vec4(std::string);

  //operators
  Vec4& operator+=(const Vec4&);
  Vec4& operator-=(const Vec4&);
  Vec4& operator*=(const float &scale);
  Vec4& operator/=(const float &scale);  
  Vec4& operator=(const Vec4&);
  Vec4 operator+ (const Vec4& rhs)const;
  Vec4 operator- (const Vec4& rhs)const;
  Vec4 operator* (const float &scale)const;
  Vec4 operator/ (const float &scale)const;
  bool operator==(Vec4 const &) const;
  Vec4& operator- ();

  float& operator[](int i);
  float operator[](int i) const;
  float dot(const Vec4& other) const;

  Vec4& Vec4Set(const float &ix=0.f, const float &iy=0.f, const float &iz = 0.f, const float& iw = 0.f);
  Vec3 ToVec3() const;
  //void Deserialize(Json::Value*);
  //void Serialize(Json::Value*);
};


//functions
Vec4 Vec4Normalise(const Vec4 &rhs);
Vec4 Vec4CrossProduct(const Vec4&lhs, const Vec4 &rhs);
float Vec4Length(const Vec4 &rhs);
float Vec4SquareLength(const Vec4 &rhs);
float Vec4Distance(const Vec4 &lhs,const Vec4 &rhs);
float Vec4SquareDistance(const Vec4 &lhs,const Vec4 &rhs);
float Vec4DotProduct(const Vec4 &lhs, const Vec4 &rhs);
float Vec4Angle2DXY(const Vec4&lhs,const  Vec4 &rhs);
float Vec4Angle2DYZ(const Vec4&lhs, const Vec4 &rhs);
float Vec4Angle2DXZ(const Vec4&lhs, const Vec4 &rhs);
float Vec4Angle3D(const Vec4&lhs,  const Vec4 &rhs);

//conversions
Vec3 ToVec3(const Vec4& rhs);

//print vector
std::ostream& operator<<(std::ostream& os, const Vec4& M);