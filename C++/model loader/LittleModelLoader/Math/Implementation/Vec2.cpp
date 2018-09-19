/******************************************************************************/
/*!
\File name: Vec2.cpp
\Project name: 
\Primary Author: 
All content © 2017 DigiPen (SINGAPORE) Corporation, All rights reserved.
*/
/******************************************************************************/
#include "Vec2.h"

Vec2::Vec2() : x(0.f), y(0.f)
{

}

Vec2::Vec2(const Vec2& rhs) : x(rhs.x), y(rhs.y)
{

}

Vec2::Vec2(float X, float Y) : x(X), y(Y)
{

}

Vec2& Vec2::operator=(const Vec2& rhs)
{
  x = rhs.x;
  y = rhs.y;

  return *this;
}

Vec2& Vec2::operator=(float* x1)
{
  x = *x1;
  y = *(x1 + 1);

  return *this;
}

Vec2& Vec2::operator+=(const Vec2& rhs)
{
  x += rhs.x;
  y += rhs.y;

  return *this;
}

Vec2& Vec2::operator+=(float* x1)
{
  x += *x1;
  y += *(x1 + 1);

  return *this;
}

Vec2& Vec2::operator-=(const Vec2& rhs)
{
  x -= rhs.x;
  y -= rhs.y;

  return *this;
}

Vec2& Vec2::operator-=(float* x1)
{
  x -= *x1;
  y -= *(x1 + 1);

  return *this;
}

Vec2& Vec2::operator*=(float i)
{
  x *= i;
  y *= i;

  return *this;
}

Vec2& Vec2::operator/=(float i)
{
  x /= i;
  y /= i;

  return *this;
}

Vec2& Vec2::setAllValues(float a, float b)
{
  x = a;
  y = b;

  return *this;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
  return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
  return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator*(float i) const
{
  return Vec2(x * i, y * i);
}

Vec2 Vec2::operator/(float i) const
{
  return Vec2(x / i, y / i);
}

Vec2 Vec2::operator-() const
{
  return Vec2(-x, -y);
}

float Vec2::operator[](int i)
{
  return (&x)[i];
}

float Vec2::operator[](int i) const
{
  return (&x)[i];
}

float Vec2::dot(const Vec2& rhs) const
{
  return (x * rhs.x + y * rhs.y);
}

float Vec2::length() const
{
  return sqrt(squaredLength());
}

float Vec2::squaredLength() const
{
  return x * x + y * y;
}


bool Vec2::sameDir(const Vec2& rhs) const
{
  return dot(rhs) > 0;
}

bool Vec2::perpendicular(const Vec2& rhs) const
{
  return dot(rhs) == 0;
}

void Vec2::normalize()
{
  float mag = length();
  if (mag > FLT_EPSILON)
    *this /= mag;
  else
    *this = Vec2{};
}

void Vec2::print()
{
  std::cout << "Vector2 values: X: " << x << "Y: " << y << std::endl;
}

Vec2::~Vec2()
{

}

Vec2 operator*(float i, const Vec2& rhs)
{
  return Vec2(rhs.x * i, rhs.y * i);
}

bool operator==(const Vec2& lhs, const Vec2& rhs)
{
  float eps = std::numeric_limits<float>::epsilon();

  return (std::abs(lhs.x - rhs.x) < eps) &&
    (std::abs(lhs.y - rhs.y) < eps);
}

bool operator!=(const Vec2& lhs, const Vec2& rhs)
{
  return !(lhs == rhs);
}

Vec2 Vec2Normalise(const Vec2 & rhs)
{
  float mag = Vec2Length(rhs);
  if (mag > FLT_EPSILON)
	  return (rhs / mag);
  else
    return Vec2{};
}

float Vec2Length(const Vec2 & rhs)
{
  return sqrt(Vec2SquareLength(rhs));
}

float Vec2SquareLength(const Vec2 & rhs)
{
  return rhs.x*rhs.x + rhs.y*rhs.y;
}

float Vec2Distance(const Vec2 & lhs, const Vec2 & rhs)
{
  return sqrt(Vec2SquareDistance(lhs, rhs));
}

float Vec2SquareDistance(const Vec2 & lhs, const Vec2 & rhs)
{
  return (lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y);
}

float Vec2DotProduct(const Vec2 & lhs, const Vec2 & rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

float Vec2GetAngleBetween(const Vec2 & lhs, const Vec2 & rhs)
{
  Vec2 temp = Vec2Normalise(rhs);
  return acos(Vec2DotProduct(lhs, temp) / Vec2Length(lhs));
}
