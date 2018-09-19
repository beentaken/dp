#include <iostream>
#include <iomanip>
#include <cmath>
#include "Mtx3.h"
#include "Mtx4.h"

Mtx3::Mtx3(float a, float b, float c,
  float d, float e, float f,
  float g, float h, float i) 
{
  data[0] = a; data[1] = b; data[2] = c;
  data[3] = d; data[4] = e; data[5] = f;
  data[6] = g; data[7] = h; data[8] = i;
}

/** Takes the upper-left 3x3 submatrix */
Mtx3::Mtx3(const Mtx4& M) 
{
  //row 1
  data[0] = M.data[0]; data[1] = M.data[1]; data[2] = M.data[2];
  //row 2
  data[3] = M.data[4]; data[4] = M.data[5]; data[5] = M.data[6];
  //row 3
  data[6] = M.data[8]; data[7] = M.data[9]; data[8] = M.data[10];
}

/** initializes to the identity matrix */
Mtx3::Mtx3() 
{
  memset(data, 0, sizeof(float) * 9);
  data[0] = data[4] = data[8] = 1.0f;
}

Mtx3::Mtx3(const Mtx3& M) 
{
  memcpy(data, M.data, sizeof(float) * 9);
}

Mtx3::Mtx3(const Vec3 & right, const Vec3 & up, const Vec3 & forward)
{
  data[0] = right.x; data[1] = up.x; data[2] = forward.x;
  data[3] = right.y; data[4] = up.y; data[5] = forward.y;
  data[6] = right.z; data[7] = up.z; data[8] = forward.z;
}

Mtx3& Mtx3::operator=(const Mtx3& M) 
{
  memcpy(data, M.data, sizeof(float) * 9);
  return *this;
}

Mtx3 Mtx3::zero()
{
  return Mtx3(0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f);
}

Mtx3 Mtx3::roll(float deg)
{
  const float c = cos(deg * DEG2RAD), s = sin(deg * DEG2RAD);
  return Mtx3(c, -s, 0.0f,
    s, c, 0.0f,
    0.0f, 0.0f, 1.0f);
}

Mtx3 Mtx3::yaw(float deg)
{
  const float c = cos(deg * DEG2RAD), s = sin(deg * DEG2RAD);
  return Mtx3(c, 0.0f, s,
    0.0f, 1.0f, 0.0f,
    -s, 0.0f, c);
}

Mtx3 Mtx3::pitch(float deg)
{
  const float c = cos(deg * DEG2RAD), s = sin(deg * DEG2RAD);
  return Mtx3(1.0f, 0.0f, 0.0f,
    0.0f, c, -s,
    0.0f, s, c);
}

Mtx3 Mtx3::scale(float x, float y, float z)
{
  return Mtx3(x, 0.0f, 0.0f,
    0.0f, y, 0.0f,
    0.0f, 0.0f, z);
}

Mtx3 Mtx3::transpose() const 
{
  return Mtx3(data[0], data[3], data[6],
    data[1], data[4], data[7],
    data[2], data[5], data[8]);
}


Mtx3 Mtx3::inverse() const 
{
  const Mtx3& m(*this);
  const float det = m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) -
    m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
    m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));

  return Mtx3(
    (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) / det,
    (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) / det,
    (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) / det,

    (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) / det,
    (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) / det,
    (m(1, 0) * m(0, 2) - m(0, 0) * m(1, 2)) / det,

    (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) / det,
    (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) / det,
    (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) / det);
}

float& Mtx3::operator()(int r, int c) 
{
  return data[r * 3 + c];
}

float Mtx3::operator()(int r, int c) const 
{
  return data[r * 3 + c];
}

Vec3 Mtx3::row(int r) const 
{
  const int i = r * 3;
  return Vec3(data[i], data[i + 1], data[i + 2]);
}

Vec3 Mtx3::col(int c) const 
{
  return Vec3(data[c], data[c + 3], data[c + 6]);
}

Mtx3 Mtx3::operator*(const Mtx3& B) const 
{
  Mtx3 D;
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      D(r, c) = row(r).dot(B.col(c));
    }
  }
  return D;
}

Mtx3 Mtx3::operator*(const float s) const 
{
  Mtx3 D;
  for (int i = 0; i < 9; ++i) {
    D.data[i] = data[i] * s;
  }
  return D;
}

Mtx3 Mtx3::operator/(const float s) const 
{
  Mtx3 D;
  for (int i = 0; i < 9; ++i) {
    D.data[i] = data[i] / s;
  }
  return D;
}

Vec3 Mtx3::operator*(const Vec3& v) const 
{
  Vec3 d;
  for (int r = 0; r < 3; ++r) 
  {
    d[r] = row(r).dot(v);
  }
  return d;
}

std::ostream& operator<<(std::ostream& os, const Mtx3& M) 
{
  os << "\nMatrix3x3(";

  for (int r = 0, i = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c, ++i) {
      os << M.data[i];
      if (c < 2) { os << ", "; }
    }
    if (r < 2) { os << ",\n          "; }
  }

  return os << ")\n";
}