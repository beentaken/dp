#include <iostream>
#include <iomanip>
#include <cmath>
#include "Mtx4.h"

Mtx4::Mtx4(float a, float b, float c, float d,
  float e, float f, float g, float h,
  float i, float j, float k, float l,
  float m, float n, float o, float p) 
{
  data[0] = a; data[1] = b; data[2] = c; data[3] = d;
  data[4] = e; data[5] = f; data[6] = g; data[7] = h;
  data[8] = i; data[9] = j; data[10] = k; data[11] = l;
  data[12] = m; data[13] = n; data[14] = o; data[15] = p;
}

Mtx4::Mtx4()
{
  memset(data, 0, sizeof(float) * 16);
  data[0] = data[5] = data[10] = data[15] = 1.0f;
}

Mtx4::Mtx4(const Mtx4& M) 
{
  memcpy(data, M.data, sizeof(float) * 16);
}

Mtx4::Mtx4(const aiMatrix4x4 & a)
{
  data[0] = a.a1;   data[1] = a.a2;   data[2] = a.a3;   data[3] = a.a4;
  data[4] = a.b1;   data[5] = a.b2;   data[6] = a.b3;   data[7] = a.b4;
  data[8] = a.c1;   data[9] = a.c2;   data[10] = a.c3;  data[11] = a.c4;
  data[12] = a.d1;  data[13] = a.d2;  data[14] = a.d3;  data[15] = a.d4;
  //transposed
  /*
  data[0] = a.a1;   data[1] = a.b1;   data[2] = a.c1;   data[3] = a.d1;
  data[4] = a.a2;   data[5] = a.b2;   data[6] = a.c2;   data[7] = a.d2;
  data[8] = a.a3;   data[9] = a.b3;   data[10] = a.c3;  data[11] = a.d3;
  data[12] = a.a4;  data[13] = a.b4;  data[14] = a.c4;  data[15] = a.d4;
  */
}

Mtx4::Mtx4(const Mtx3& L, const Vec3& trans)
{
  //rotation and translation components
  data[0] = L.data[0]; data[1] = L.data[1]; data[2] = L.data[2]; data[3] = trans.x;
  data[4] = L.data[3]; data[5] = L.data[4]; data[6] = L.data[5]; data[7] = trans.y;
  data[8] = L.data[6]; data[9] = L.data[7]; data[10] = L.data[8]; data[11] = trans.z;

  //last row = (0, 0, 0, 1)
  data[12] = 0; data[13] = 0; data[14] = 0; data[15] = 1;
}

Mtx4& Mtx4::operator=(const Mtx4& M) 
{
  memcpy(data, M.data, sizeof(float) * 16);
  return *this;
}

Mtx4 Mtx4::zero() 
{
  return Mtx4(0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f);
}

Mtx4 Mtx4::roll(float deg) 
{
  const float c = cos(deg * DEG2RAD), s = sin(deg * DEG2RAD);
  return Mtx4(c, -s, 0.0f, 0.0f,
    s, c, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
}

Mtx4 Mtx4::yaw(float deg) 
{
  const float c = cos(deg * DEG2RAD), s = sin(deg * DEG2RAD);
  return Mtx4(c, 0.0f, s, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    -s, 0.0f, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
}

Mtx4 Mtx4::pitch(float deg) 
{
  const float c = cos(deg * DEG2RAD), s = sin(deg * DEG2RAD);
  return Mtx4(1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, c, -s, 0.0f,
    0.0f, s, c, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);
}

Mtx4 Mtx4::scale(Vec3 const& v)
{
	return scale(v.x, v.y, v.z);
}

Mtx4 Mtx4::scale(float x, float y, float z) 
{
  return Mtx4(x, 0.0f, 0.0f, 0.0f,
              0.0f, y, 0.0f, 0.0f,
              0.0f, 0.0f, z, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f);
}

Mtx4 Mtx4::translate(float x, float y, float z) 
{
  return Mtx4(1.0f, 0.0f, 0.0f, x,
    0.0f, 1.0f, 0.0f, y,
    0.0f, 0.0f, 1.0f, z,
    0.0f, 0.0f, 0.0f, 1.0f);
}

Mtx4 Mtx4::translate(const Vec3& v) 
{
  return translate(v.x, v.y, v.z);
}

/*
//Assuming that bottom left is (0, 0) and topright is (width, height)
Mtx4 Mtx4::ortho(float width, float height, float N, float F) 
{
  //might not be working
  return Mtx4(2.0f / width, 0.0f, 0.0f, -1.0f,
            0.0f, -2.0f / height, 0.0f, 1.0f,
            0.0f, 0.0f, -2.0f / (N - F), (F + N) / (N - F),
            0.0f, 0.0f, 0.0f, 1.0f);
}
*/

Mtx4 Mtx4::glortho(float left, float right, float bottom, float top, float N, float F)
{
  //glm implementation
  float width = right - left;
  float height = top - bottom;
  float zDiff = F - N;

  return Mtx4(2.f/width, 0.0f,       0.0f,            -(right + left)/ width,
              0.0f,     2.f/height, 0.0f,            -(top + bottom)/ height,
              0.0f,     0.0f,        -2.f / zDiff,    -(F + N) / zDiff,
              0.0f,     0.0f,        0.0f,             1.0f);
}

Mtx4 Mtx4::glperspective(float fovX, float aspect, float n, float f)
{
  //glm implementation

  Mtx4 proj = Mtx4::zero();
  float c = f - n;
  float tanHalfFOV = tan(fovX * 0.5f * DEG2RAD);

  proj.data[0] = 1.0f / (aspect * tanHalfFOV);
  proj.data[5] = 1.0f / tanHalfFOV;
  proj.data[10] = -(f + n) / c;
  proj.data[11] = -(2.f * f * n) / c;
  proj.data[14] = -1.f;

  return proj;
}

Mtx4 Mtx4::glLookAtRH(const Vec3 & eye, const Vec3 & target, const Vec3 & relup)
{
  //get back vector
  Vec3 back(eye - target);
  back.normalize();

  //cross up and back to get the right vector
  Vec3 right = relup.cross(back);
  right.normalize();

  //cross forward and right to get the actual up vector
  Vec3 up = back.cross(right);
  up.normalize();

  //build view matrix 
  Mtx4 view(right.x, right.y, right.z, -right.dot(eye),
    up.x, up.y, up.z, -up.dot(eye),
    back.x, back.y, back.z, -back.dot(eye),
    0, 0, 0, 1);

  return view;
}

Mtx3 Mtx4::getNormalizedCoordinateSystem() const
{
  return Mtx3(getRight(), getUp(), getForward());
}

Vec3 Mtx4::getLengthOfCoordinateVectorsInMatrix() const
{
  Vec3 x_axis = Vec3(data[0], data[4], data[8]);
  Vec3 y_axis = Vec3(data[1], data[5], data[9]);
  Vec3 z_axis = Vec3(data[2], data[6], data[10]);

  return Vec3(x_axis.length(), y_axis.length(), z_axis.length());
}

Vec3 Mtx4::getTranslation() const
{
  return Vec3(data[3], data[7], data[11]);
}

Vec3 Mtx4::getForward() const
{
	Vec3 vec(data[2], data[6], data[10]);
	vec.normalize();
	return vec;
}

Vec3 Mtx4::getBack() const
{
  return -getForward();
}

Vec3 Mtx4::getUp() const
{
	Vec3 vec(data[1], data[5], data[9]);
	vec.normalize();
	return vec;
}

Vec3 Mtx4::getDown() const
{
  return -getUp();
}

Vec3 Mtx4::getRight() const
{
	Vec3 vec(data[0], data[4], data[8]);
	vec.normalize();
	return vec;
}

Vec3 Mtx4::getLeft() const
{
  return -getRight();
}

Mtx4 Mtx4::axisangle(const float & angleInDegrees, const Vec3 & v)
{
  Mtx4 result;
  float a = angleInDegrees * DEG2RAD;
  float cos_a = std::cosf(a);
  float sin_a = std::sinf(a);

  result.data[0] = 1.f + (1.f - cos_a) * (v.x * v.x - 1.f);
  result.data[1] = -v.z * sin_a + (1.f - cos_a) * v.x * v.y;
  result.data[2] = v.y * sin_a + (1.f - cos_a) * v.x * v.z;

  result.data[4] = v.z * sin_a + (1.f - cos_a) * v.x * v.y;
  result.data[5] = 1.f + (1.f - cos_a) * (v.y * v.y - 1.f);
  result.data[6] = -v.x * sin_a + (1 - cos_a) * v.y * v.z;

  result.data[8] = -v.y * sin_a + (1.f - cos_a) * v.x * v.z;
  result.data[9] = v.x * sin_a + (1.f - cos_a) * v.y * v.z;
  result.data[10] = 1.f + (1.f - cos_a) * (v.z * v.z - 1.f);

  return result;
}

Mtx4 Mtx4::Bias()
{
  //used for shadow mapping
  return Mtx4(0.5f, 0.f, 0.f, 0.5f,
              0.f, 0.5f, 0.f, 0.5f,
              0.f, 0.f, 0.5f, 0.5f,
              0.f, 0.f, 0.f, 1.f);
}

Mtx4 Mtx4::transpose() const 
{
  return Mtx4(data[0], data[4], data[8], data[12],
    data[1], data[5], data[9], data[13],
    data[2], data[6], data[10], data[14],
    data[3], data[7], data[11], data[15]);
}

Mtx4 Mtx4::inverse() const 
{
  Mtx4 result;
  const float* m = data;
  float* inv = result.data;

  inv[0] = m[5] * m[10] * m[15] -
    m[5] * m[11] * m[14] -
    m[9] * m[6] * m[15] +
    m[9] * m[7] * m[14] +
    m[13] * m[6] * m[11] -
    m[13] * m[7] * m[10];

  inv[4] = -m[4] * m[10] * m[15] +
    m[4] * m[11] * m[14] +
    m[8] * m[6] * m[15] -
    m[8] * m[7] * m[14] -
    m[12] * m[6] * m[11] +
    m[12] * m[7] * m[10];

  inv[8] = m[4] * m[9] * m[15] -
    m[4] * m[11] * m[13] -
    m[8] * m[5] * m[15] +
    m[8] * m[7] * m[13] +
    m[12] * m[5] * m[11] -
    m[12] * m[7] * m[9];

  inv[12] = -m[4] * m[9] * m[14] +
    m[4] * m[10] * m[13] +
    m[8] * m[5] * m[14] -
    m[8] * m[6] * m[13] -
    m[12] * m[5] * m[10] +
    m[12] * m[6] * m[9];

  inv[1] = -m[1] * m[10] * m[15] +
    m[1] * m[11] * m[14] +
    m[9] * m[2] * m[15] -
    m[9] * m[3] * m[14] -
    m[13] * m[2] * m[11] +
    m[13] * m[3] * m[10];

  inv[5] = m[0] * m[10] * m[15] -
    m[0] * m[11] * m[14] -
    m[8] * m[2] * m[15] +
    m[8] * m[3] * m[14] +
    m[12] * m[2] * m[11] -
    m[12] * m[3] * m[10];

  inv[9] = -m[0] * m[9] * m[15] +
    m[0] * m[11] * m[13] +
    m[8] * m[1] * m[15] -
    m[8] * m[3] * m[13] -
    m[12] * m[1] * m[11] +
    m[12] * m[3] * m[9];

  inv[13] = m[0] * m[9] * m[14] -
    m[0] * m[10] * m[13] -
    m[8] * m[1] * m[14] +
    m[8] * m[2] * m[13] +
    m[12] * m[1] * m[10] -
    m[12] * m[2] * m[9];

  inv[2] = m[1] * m[6] * m[15] -
    m[1] * m[7] * m[14] -
    m[5] * m[2] * m[15] +
    m[5] * m[3] * m[14] +
    m[13] * m[2] * m[7] -
    m[13] * m[3] * m[6];

  inv[6] = -m[0] * m[6] * m[15] +
    m[0] * m[7] * m[14] +
    m[4] * m[2] * m[15] -
    m[4] * m[3] * m[14] -
    m[12] * m[2] * m[7] +
    m[12] * m[3] * m[6];

  inv[10] = m[0] * m[5] * m[15] -
    m[0] * m[7] * m[13] -
    m[4] * m[1] * m[15] +
    m[4] * m[3] * m[13] +
    m[12] * m[1] * m[7] -
    m[12] * m[3] * m[5];

  inv[14] = -m[0] * m[5] * m[14] +
    m[0] * m[6] * m[13] +
    m[4] * m[1] * m[14] -
    m[4] * m[2] * m[13] -
    m[12] * m[1] * m[6] +
    m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] +
    m[1] * m[7] * m[10] +
    m[5] * m[2] * m[11] -
    m[5] * m[3] * m[10] -
    m[9] * m[2] * m[7] +
    m[9] * m[3] * m[6];

  inv[7] = m[0] * m[6] * m[11] -
    m[0] * m[7] * m[10] -
    m[4] * m[2] * m[11] +
    m[4] * m[3] * m[10] +
    m[8] * m[2] * m[7] -
    m[8] * m[3] * m[6];

  inv[11] = -m[0] * m[5] * m[11] +
    m[0] * m[7] * m[9] +
    m[4] * m[1] * m[11] -
    m[4] * m[3] * m[9] -
    m[8] * m[1] * m[7] +
    m[8] * m[3] * m[5];

  inv[15] = m[0] * m[5] * m[10] -
    m[0] * m[6] * m[9] -
    m[4] * m[1] * m[10] +
    m[4] * m[2] * m[9] +
    m[8] * m[1] * m[6] -
    m[8] * m[2] * m[5];

  float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
  return result / det;
}

Mtx4 Mtx4::affineInverse() const
{
  Mtx3 linear(*this);
  Vec3 inverseTrans(-data[3], -data[7], -data[11]);
  return Mtx4(linear.transpose(), inverseTrans);
}


float& Mtx4::operator()(int r, int c)
{
  return data[(r * 4) + c];
}

float Mtx4::operator()(int r, int c) const 
{
  return data[(r * 4) + c];
}

Vec4 Mtx4::row(int r) const
{
  const int i = r * 4;
  return Vec4(data[i], data[i + 1], data[i + 2], data[i + 3]);
}

Vec4 Mtx4::col(int c) const 
{
  return Vec4(data[c], data[c + 4], data[c + 8], data[c + 12]);
}

Mtx4 Mtx4::operator*(const Mtx4& B) const 
{
  //correct version
  Mtx4 D;
  for (int r = 0; r < 4; ++r)
  {
    for (int c = 0; c < 4; ++c)
    {
      D(r, c) = row(r).dot(B.col(c));
    }
  }
  return D;

  /* metromanic matrix multiply
  Mtx4 temp;

  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      for (int k = 0; k <4; ++k)
      {
        temp.data[i * 4 + j] += data[i * 4 + k] *
          B.data[k * 4 + j];
      }
    }
  }
  return temp;
  */
}

Mtx4 Mtx4::operator*(const float s) const 
{
  Mtx4 D;
  for (int i = 0; i < 16; ++i) 
  {
    D.data[i] = data[i] * s;
  }
  return D;
}

Mtx4 Mtx4::operator/(const float s) const 
{
  Mtx4 D;
  for (int i = 0; i < 16; ++i) 
  {
    D.data[i] = data[i] / s;
  }
  return D;
}

Vec4 Mtx4::operator*(const Vec4& v) const 
{
  Vec4 d;
  for (int r = 0; r < 4; ++r) 
  {
    d[r] = row(r).dot(v);
  }
  return d;
}

std::ostream& operator<<(std::ostream& os, const Mtx4& M) 
{
  os << "\nMatrix4x4(";

  for (int r = 0, i = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c, ++i) {
      os << M.data[i];
      if (c < 3) { os << ", "; }
    }
    if (r < 3) { os << ",\n          "; }
  }

  return os << ")\n";
}


