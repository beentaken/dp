/******************************************************************************/
/*!
\file		Vec4.cpp
\project	MetroManiac
\primary author Sia Hou Ping

All content \@ 2016 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "Vec4.h"
#include "Vec3.h"
#include <string>

//Functions of Vector 4

Vec4 Vec4::R()
{
  return Vec4(1.f, 0.f, 0.f, 1.f);
}

Vec4 Vec4::G()
{
  return Vec4(0.f, 1.f, 0.f, 1.f);
}

Vec4 Vec4::B()
{
  return Vec4(0.f, 0.f, 1.f, 1.f);
}

Vec4::Vec4(const Vec4 &rhs)
{
  *this = rhs; 
}

Vec4::Vec4(std::string vecname)
{
	size_t xval = vecname.find_first_of("x");
	size_t yval = vecname.find_first_of("y");
	x = std::stof(vecname.substr(xval+1, yval-1));
	size_t zval = vecname.find_first_of("z");
	if (zval != std::string::npos)
	{
		y = std::stof(vecname.substr(yval + 1, zval - 1));
		size_t wval = vecname.find_first_of("w");
		if (wval != std::string::npos)
		{
			z = std::stof(vecname.substr(zval + 1, wval - 1));
			w = std::stof(vecname.substr(wval+1));

		}
		else
		{
			z = std::stof(vecname.substr(zval+1));
		}
	}
	else
	{
		y = std::stof(vecname.substr(yval + 1));
	}

}

Vec4& Vec4::operator+=(const Vec4& rhs)
{
  x+=rhs.x;
  y+=rhs.y;
  z+=rhs.z;
  w+=rhs.w;
  return *this;
}

Vec4& Vec4::operator-=(const Vec4& rhs)
{
  x-=rhs.x;
  y-=rhs.y;
  z-=rhs.z;
  w-=rhs.w;
  return *this;
}

Vec4& Vec4::operator*=(const float &scale)
{
  x*=scale;
  y*=scale;
  z*=scale;
  w *= scale;
  return *this;
}

Vec4& Vec4::operator/=(const float &scale)
{
  x/=scale;
  y/=scale;
  z/=scale;
  w /= scale;
  return *this;
}

Vec4& Vec4::operator=(const Vec4& rhs)
{
  if(this != &rhs)
  {
    x =rhs.x;
    y =rhs.y;
    z =rhs.z;
    w =rhs.w;
  }
 
  return *this;
}

Vec4 Vec4::operator+ (const Vec4 &rhs)const
{
  Vec4 temp = *this;
  return  temp += rhs;
}

Vec4 Vec4::operator- (const Vec4 &rhs)const
{
  Vec4 temp = *this;
  return  temp -= rhs;
}

Vec4 Vec4::operator* (const float &scale)const
{
  Vec4 temp = *this;
  return  temp *= scale;
}

Vec4 Vec4::operator/ (const float &scale)const
{
  Vec4 temp = *this;
  return  temp /= scale;
}

bool Vec4::operator==(Vec4 const & rhs) const
{
	return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
}

Vec4 & Vec4::operator-()
{
  float a = -1;
  return (*this) *= a;
}

float & Vec4::operator[](int i)
{
  return (&x)[i];
}

float Vec4::operator[](int i) const
{
  return (&x)[i];
}

float Vec4::dot(const Vec4 & other) const
{
  return x * other.x + y * other.y + z * other.z + w* other.w;
}

Vec4& Vec4::Vec4Set( const float& ix,const float& iy, const float& iz, const float& iw)
{
   x = ix;
   y = iy;
   z = iz;
   w = iw;
   
   return *this;
}

Vec4 Vec4Normalise(const Vec4 &rhs)
{
  float mag = Vec4Length(rhs);
  if (mag > FLT_EPSILON)
	  return (rhs / mag);
  else
	  return Vec4{};
}

float Vec4Length(const Vec4 &rhs)
{
  return sqrt(Vec4SquareLength(rhs));
}

float Vec4SquareLength(const Vec4 &rhs)
{
  return rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z;
}

float Vec4Distance(const Vec4 &lhs,const Vec4 &rhs)
{
  return sqrt(Vec4SquareDistance(lhs,rhs));
}

float Vec4SquareDistance(const Vec4 &lhs,const Vec4 &rhs)
{
  return (lhs.x-rhs.x) * (lhs.x - rhs.x) + (lhs.y-rhs.y) * (lhs.y - rhs.y) + (lhs.z-rhs.z) * (lhs.z - rhs.z);
}

float Vec4DotProduct(const Vec4 &lhs, const Vec4 &rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec4 Vec4CrossProduct(const Vec4&lhs, const Vec4 &rhs)
{
  Vec4 temp = {lhs.y * rhs.z - lhs.z* rhs.y,
               -(lhs.x * rhs.z - lhs.z *rhs.x),
               lhs.x *rhs.y - lhs.y*rhs.x};
  return temp;
}


// angle between x and y axis
float Vec4Angle2DXY(const Vec4&lhs, const Vec4 &rhs)
{
  Vec4 normal = rhs/sqrt(rhs.x*rhs.x + rhs.y*rhs.y);
  float dotproduct = lhs.x * normal.x + lhs.y * normal.y;
  Vec4 temp = lhs;
  temp.z = 0;
  float length = Vec4Length(lhs);
  if (length)
	  return acos(dotproduct / length);
  else
	  return 0.0f;
}

// angle between y and z axis
float Vec4Angle2DYZ(const Vec4&lhs, const Vec4 &rhs)
{
  Vec4 normal = rhs/sqrt(rhs.z*rhs.z + rhs.y*rhs.y);
  float dotproduct = lhs.z * normal.z + lhs.y * normal.y;
  Vec4 temp = lhs;
  temp.x = 0;
  return acos(dotproduct/ Vec4Length(lhs));
}

// angle between x and z axis
float Vec4Angle2DXZ(const Vec4&lhs,const Vec4 &rhs)
{
  Vec4 normal = rhs/sqrt(rhs.z*rhs.x + rhs.z*rhs.y);
  float dotproduct = lhs.x * normal.x + lhs.z * normal.z;
  Vec4 temp = lhs;
  temp.y = 0;
   return acos(dotproduct/ Vec4Length(lhs));
}

//please use 3d angle for 2d game
float Vec4Angle3D(const Vec4&lhs, const Vec4 &rhs)
{
  Vec4 temp = Vec4Normalise(rhs);
  return acos(Vec4DotProduct(lhs,temp)/Vec4Length(lhs));
}

Vec3 ToVec3(const Vec4 & rhs)
{
  return Vec3(rhs.x, rhs.y, rhs.z);
}

Vec3 Vec4::ToVec3() const
{
  return Vec3(x, y, z);
}

std::ostream & operator<<(std::ostream & os, const Vec4 & M)
{
	os << "(X: " << M.x << "   " << "Y: " << M.y << "   " << "Z: " << M.z << "   " << "W: " << M.w << ")";
	return os;
}