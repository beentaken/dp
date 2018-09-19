/******************************************************************************/
/*!
\File name: Vec3.cpp
\Project name: 
\Primary Author: 
All content © 2017 DigiPen (SINGAPORE) Corporation, All rights reserved.
*/
/******************************************************************************/
#include "Vec3.h"
#include "Vec4.h"

Vec3::Vec3() : x(0.f), y(0.f), z(0.f)
{

}

Vec3::Vec3(const Vec3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z)
{

}

Vec3::Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z)
{

}

Vec3::Vec3(const Vec4 & v):
  x(v.x), y(v.y), z(v.z)
{
}

Vec3::Vec3(const aiVector3D & a) : x(a.x), y(a.y), z(a.z)
{
}

Vec3& Vec3::operator=(const Vec3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
}

Vec3& Vec3::operator=(float* x1)
{
	x = * x1;
	y = *(x1 + 1);
	z = *(x1 + 2);

	return *this;
}

Vec3& Vec3::operator+=(const Vec3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

Vec3& Vec3::operator+=(float* x1)
{
	x += *x1;
	y += *(x1 + 1);
	z += *(x1 + 2);

	return *this;
}

Vec3& Vec3::operator-=(const Vec3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

Vec3& Vec3::operator-=(float* x1)
{
	x -= *x1;
	y -= *(x1 + 1);
	z -= *(x1 + 2);

	return *this;
}

Vec3& Vec3::operator*=(float i)
{
	x *= i;
	y *= i;
	z *= i;

	return *this;
}

Vec3& Vec3::operator/=(float i)
{
	x /= i;
	y /= i;
	z /= i;

	return *this;
}


Vec3& Vec3::Vec3Set(const float& a1, const float& b1, const float& c1)
{
	x = a1;
	y = b1;
	z = c1;

	return *this;
}

Vec3 Vec3::operator+(const Vec3& rhs) const
{
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3 Vec3::operator-(const Vec3& rhs) const
{
	return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3 Vec3::operator*(float i) const
{
	return Vec3(x * i, y * i, z * i);
}

Vec3 Vec3::operator/(float i) const
{
	return Vec3(x / i, y / i, z / i);
}

Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

Vec3 Vec3::cross(const Vec3& rhs) const
{
	return Vec3((y * rhs.z - z * rhs.y),
	(z * rhs.x - x * rhs.z),
	(x * rhs.y - y * rhs.x));
}

float& Vec3::operator[](int i)
{
  return (&x)[i];
}

float Vec3::operator[](int i) const
{
  return (&x)[i];
}

float Vec3::dot(const Vec3& rhs) const
{
  return x * rhs.x + y * rhs.y + z * rhs.z;
}

float Vec3::length() const
{
	return sqrt(squaredLength());
}

float Vec3::squaredLength() const
{
	return x * x + y * y + z * z;
}

bool Vec3::sameDir(const Vec3& rhs) const
{
	return dot(rhs) > 0;
}

bool Vec3::perpendicular(const Vec3& rhs) const
{
	return dot(rhs) == 0;
}

void Vec3::normalize()
{
	float mag = length();
	if (mag > FLT_EPSILON)
		*this /= mag;
	else
		*this = Vec3{};
}

void Vec3::print()
{
  std::cout << "Vector3 values: X: " << x << "Y: " << y << "Z: " << z << std::endl;
}

Vec3::~Vec3()
{

}

Vec3 Vec3Normalise(const Vec3 & rhs)
{
  float mag = Vec3Length(rhs);
  if (mag > FLT_EPSILON)
    return (rhs / mag);
  else
    return Vec3{};
}

float Vec3Length(const Vec3 & rhs)
{
  return sqrt(Vec3SquareLength(rhs));
}

float Vec3SquareLength(const Vec3 & rhs)
{
  return rhs.x*rhs.x + rhs.y*rhs.y + rhs.z*rhs.z;
}

float Vec3Distance(const Vec3 & lhs, const Vec3 & rhs)
{
  return sqrt(Vec3SquareDistance(lhs, rhs));
}

float Vec3SquareDistance(const Vec3 & lhs, const Vec3 & rhs)
{
  return (lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y) + (lhs.z - rhs.z) * (lhs.z - rhs.z);
}

float Vec3DotProduct(const Vec3 & lhs, const Vec3 & rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3 Vec3CrossProduct(const Vec3 & lhs, const Vec3 & rhs)
{
  Vec3 temp = { lhs.y * rhs.z - lhs.z * rhs.y,
               -(lhs.x * rhs.z - lhs.z * rhs.x),
                lhs.x * rhs.y - lhs.y * rhs.x 
              };
  return temp;
}

float Vec3GetAngleBetween(const Vec3 & lhs, const Vec3 & rhs)
{
  Vec3 temp = Vec3Normalise(rhs);
  return acos(Vec3DotProduct(lhs, temp) / Vec3Length(lhs));
}

Vec4 ToVec4Vector(const Vec3 & rhs)
{
  return Vec4(rhs.x, rhs.y, rhs.z, 0);
}

std::ostream & operator<<(std::ostream & os, const Vec3 & M)
{
	os << "(X: " << M.x << "   " << "Y: " << M.y << "   " << "Z: " << M.z << ")";
	return os;
}

Vec4 ToVec4Point(const Vec3 & rhs)
{
  return Vec4(rhs.x, rhs.y, rhs.z, 1);
}

Vec3 operator*(float i, const Vec3& rhs)
{
	return Vec3(rhs.x * i, rhs.y * i, rhs.z * i);
}

bool operator==(const Vec3& lhs, const Vec3& rhs)
{
	float eps = std::numeric_limits<float>::epsilon();

	return (std::abs(lhs.x - rhs.x) < eps) &&
	(std::abs(lhs.y - rhs.y) < eps) &&
	(std::abs(lhs.z - rhs.z) < eps);
}

bool operator!=(const Vec3& lhs, const Vec3& rhs)
{
	return !(lhs == rhs);
}

Vec3 Lerp(const Vec3& start, const Vec3& end, float t)
{
  return start + t * (end - start);
}
