#include "quaternion.h"
#include "Mtx4.h"
#include "Math\OMath.h"
//#include "EulerAngle.h"

//references: http://www.songho.ca/math/quaternion/quaternion.html#rotation
//          : http://www.euclideanspace.com/maths/
//and also assimp's quaternion implementation


Quaternion Quaternion::identity()
{
  return Quaternion(1, 0, 0, 0);
}

Quaternion Quaternion::AngleAxis(float degrees, Vec3 axis)
{
  return Quaternion(degrees, axis);
}

Quaternion Quaternion::Euler(float rotX, float rotY, float rotZ)
{
  return Quaternion(rotX, rotY, rotZ);
}

Quaternion Quaternion::Euler(const Vec3 & eulerAnglesInDegrees)
{
  return Quaternion(eulerAnglesInDegrees.x, eulerAnglesInDegrees.y, eulerAnglesInDegrees.z);
}


Quaternion::Quaternion(const aiQuaternion & a):
  w(a.w), x(a.x), y(a.y), z(a.z)
{
}

//angle given in degrees
Quaternion::Quaternion(float angle, Vec3 axis)
{
  axis.normalize();
  float a = angle * 0.5f * DEG2RAD;
  float sin_a = sin(a);
  w = cos(a);
  x = axis.x * sin_a;
  y = axis.y * sin_a;
  z = axis.z * sin_a;
}

Quaternion::Quaternion(const Mtx4 & rot)
{
  //calculate trace of matrix
  float tr = rot.data[0] + rot.data[5] + rot.data[10];

  // if trace is positive and not zero
  if (tr > 0.f)
  {
    float s = std::sqrt(1 + tr) * 2.f;
    x = (rot.data[9] - rot.data[6]) / s;
    y = (rot.data[2] - rot.data[8]) / s;
    z = (rot.data[4] - rot.data[1]) / s;
    w = 0.25f * s;
  } 
  // else we have to check several cases (check which diagonal has greatest value)
  else if (rot.data[0] > rot.data[5] && rot.data[0] > rot.data[10])
  {
    float s = std::sqrt(1.f + rot.data[0] - rot.data[5] - rot.data[10]) * 2.f;
    x = 0.25f * s;
    y = (rot.data[4] + rot.data[1]) / s;
    z = (rot.data[2] + rot.data[8]) / s;
    w = (rot.data[9] - rot.data[6]) / s;
  }
  else if (rot.data[5] > rot.data[10])
  {
    float s = std::sqrt(1.f + rot.data[5] - rot.data[0] - rot.data[10]) * 2.f;
    x = (rot.data[4] + rot.data[1]) / s;
    y = 0.25f * s;
    z = (rot.data[9] + rot.data[6]) / s;
    w = (rot.data[2] - rot.data[8]) / s;
  }
  else
  { 
    float s = std::sqrt(1.f + rot.data[10] - rot.data[0] - rot.data[5]) * 2.f;
    x = (rot.data[2] + rot.data[8]) / s;
    y = (rot.data[9] + rot.data[6]) / s;
    z = 0.25f * s;
    w = (rot.data[4] - rot.data[1]) / s;
  }
}

Quaternion::Quaternion(float rotX, float rotY, float rotZ) : Quaternion(Vec3(rotX, rotY, rotZ))
{
#if 0
  float yaw = rotY;
  float pitch = rotX;
  float roll = rotZ;

  //std::cout << "yaw: " << yaw << std::endl;
  //std::cout << "pitch: " << pitch << std::endl;
  //std::cout << "roll: " << roll << std::endl;

  float cYaw = cos(yaw * DEG2RAD * 0.5f);
  float cPitch = cos(pitch * DEG2RAD * 0.5f);
  float cRoll = cos(roll * DEG2RAD * 0.5f);
  float sYaw = sin(yaw * DEG2RAD * 0.5f);
  float sPitch = sin(pitch * DEG2RAD * 0.5f);
  float sRoll = sin(roll * DEG2RAD * 0.5f);

  w = static_cast<float>(cYaw * cPitch * cRoll - sYaw * sPitch * sRoll);
  x = static_cast<float>(sYaw * sPitch * cRoll + cYaw * cPitch * sRoll);
  y = static_cast<float>(sYaw * cPitch * cRoll + cYaw * sPitch * sRoll);
  z = static_cast<float>(cYaw * sPitch * cRoll - sYaw * cPitch * sRoll);
#endif
}

Quaternion::Quaternion(const Vec3 & eulerAngleInDegrees)
{
  Vec3 radVec = eulerAngleInDegrees * DEG2RAD * 0.5f;
  Vec3 c = Vec3(std::cosf(radVec.x), std::cosf(radVec.y), std::cosf(radVec.z));
  Vec3 s = Vec3(std::sinf(radVec.x), std::sinf(radVec.y), std::sinf(radVec.z));

  w = c.x * c.y * c.z + s.x * s.y * s.z;
  x = s.x * c.y * c.z - c.x * s.y * s.z;
  y = c.x * s.y * c.z + s.x * c.y * s.z;
  z = c.x * c.y * s.z - s.x * s.y * c.z;
}

Vec3 Quaternion::operator*(const Vec3 & vec) const
{
  //uses efficient method of v' = q*v*q_inv
  Vec3 q_vec = Vec3(x, y, z);
  Vec3 temp = Vec3CrossProduct(q_vec, vec);
  Vec3 temp2 = Vec3CrossProduct(q_vec, temp);
  temp *= (2.f * w);
  temp2 *= 2.f;
  return vec + temp + temp2;
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
  return Quaternion(w*q.w - x*q.x - y*q.y - z*q.z,
                    w*q.x + x*q.w + y*q.z - z*q.y,
                    w*q.y + y*q.w + z*q.x - x*q.z,
                    w*q.z + z*q.w + x*q.y - y*q.x);
}

Quaternion & Quaternion::operator*=(const Quaternion & q)
{
  w = w*q.w - x*q.x - y*q.y - z*q.z;
  x = w*q.x + x*q.w + y*q.z - z*q.y;
  y = w*q.y + y*q.w + z*q.x - x*q.z;
  z = w*q.z + z*q.w + x*q.y - y*q.x;

  return *this;
}

Mtx4 Quaternion::RotateAboutPoint(const Vec3 & pt) const
{
  Mtx4 result;

  Quaternion q(*this);
  q.Normalize();
  float sqw = q.w*q.w;
  float sqx = q.x*q.x;
  float sqy = q.y*q.y;
  float sqz = q.z*q.z;

  result.data[0] = sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz =1
  result.data[5] = -sqx + sqy - sqz + sqw;
  result.data[10] = -sqx - sqy + sqz + sqw;

  float tmp1 = q.x*q.y;
  float tmp2 = q.z*q.w;
  result.data[1] = 2.f * (tmp1 + tmp2);
  result.data[4] = 2.f * (tmp1 - tmp2);

  tmp1 = q.x*q.z;
  tmp2 = q.y*q.w;
  result.data[2] = 2.f * (tmp1 - tmp2);
  result.data[8] = 2.f * (tmp1 + tmp2);

  tmp1 = q.y*q.z;
  tmp2 = q.x*q.w;
  result.data[6] = 2.f * (tmp1 + tmp2);
  result.data[9] = 2.f * (tmp1 - tmp2);

  float a1, a2, a3;
  if (pt == Vec3()) 
  {
    a1 = a2 = a3 = 0;
  }
  else 
  {
    a1 = pt.x;
    a2 = pt.y;
    a3 = pt.z;
  }

  //the translation part
  result.data[3] = a1 - a1 * result.data[0] - a2 * result.data[1] - a3 * result.data[2];
  result.data[7] = a2 - a1 * result.data[4] - a2 * result.data[5] - a3 * result.data[6];
  result.data[11] = a3 - a1 * result.data[8] - a2 * result.data[9] - a3 * result.data[10];

  return result;
}

Quaternion Quaternion::Slerp(const Quaternion & qstart, const Quaternion & qend, float t)
{
  //assimp's implementation
  Quaternion result;

  // calc cosine omega using dot product
  float cos_om = qstart.x * qend.x + qstart.y * qend.y + qstart.z * qend.z + qstart.w * qend.w;

  // Reverse signs if necessary
  Quaternion end = qend;
  if (cos_om < 0.f)
  {
    cos_om = -cos_om;
    end.x = -end.x;  
    end.y = -end.y;
    end.z = -end.z;
    end.w = -end.w;
  }

  // Calculate coefficients
  float scalar_p, scalar_q;
  if ((1.f - cos_om) > 0.0001f) // 0.0001 -> some epsillon
  {
    // Standard case (slerp)
    float omega, sin_om;
    omega = std::acos(cos_om); // extract theta from dot product's cos theta
    sin_om = std::sin(omega);
    scalar_p = std::sin((1.f - t) * omega) / sin_om;
    scalar_q = std::sin(t * omega) / sin_om;
  }
  else
  {
    // if its very close, do linear interpolation because more efficient
    scalar_p = 1.f - t;
    scalar_q = t;
  }

  result.w = scalar_p * qstart.w + scalar_q * end.w;
  result.x = scalar_p * qstart.x + scalar_q * end.x;
  result.y = scalar_p * qstart.y + scalar_q * end.y;
  result.z = scalar_p * qstart.z + scalar_q * end.z;
  
  return result.Normalize();
}

Quaternion Quaternion::FromToRotation(Vec3 start, Vec3 end)
{
  start.normalize();
  end.normalize();

  float cos_a = Vec3DotProduct(start, end);
  Vec3 axis;

  if (cos_a < -1.f + EPSILON)
  {
    // special case when vectors in opposite directions:
    // there is no "ideal" rotation axis
    // So guess one; any will do as long as it's perpendicular to start
    axis = Vec3CrossProduct(Vec3(0.0f, 0.0f, 1.0f), start);

    //parallel vectors, find another vector
    if (Vec3SquareLength(axis) < 0.01f)
      axis = Vec3CrossProduct(Vec3(1.0f, 0.0f, 0.0f), start);
    
    axis.normalize();
    return Quaternion(180.f, axis);
  }

  axis = Vec3CrossProduct(start, end);
  float s = std::sqrtf((1.f + cos_a) * 2.f);
  float inv_s = 1.f / s;

  return Quaternion(s * 0.5f,
    axis.x * inv_s,
    axis.y * inv_s,
    axis.z * inv_s);
}

bool Quaternion::Equals(const Quaternion & q, float epsilon) const
{
  return
    std::abs(x - q.x) <= epsilon &&
    std::abs(y - q.y) <= epsilon &&
    std::abs(z - q.z) <= epsilon &&
    std::abs(w - q.w) <= epsilon;
}

Mtx4 Quaternion::ToMatrix() const
{
  Mtx4 result;

  result.data[0] = 1.f - 2.f * (y * y + z * z);
  result.data[1] = 2.f * (x * y - z * w);
  result.data[2] = 2.f * (x * z + y * w);

  result.data[4] = 2.f * (x * y + z * w);
  result.data[5] = 1.f - 2.f * (x * x + z * z);
  result.data[6] = 2.f * (y * z - x * w);
  
  result.data[8] = 2.f * (x * z - y * w);
  result.data[9] = 2.f * (y * z + x * w);
  result.data[10] = 1.f - 2.f * (x * x + y * y);

  return result;
}

float Quaternion::Dot(const Quaternion & q) const
{
  return w*q.w + x*q.x + y*q.y + z*q.z;
}

float Quaternion::GetRoll() const
{
  return std::atan2(2.f * (x*y + w*z), 
      w*w + x*x - y*y - z*z);
}

float Quaternion::GetPitch() const
{
  float temp_y = 2.f * (y*z + w*x);
  float temp_x = w*w - x*x - y*y + z*z;

  if (temp_y == 0.f && temp_x == 0.f) //avoid atan2(0,0) - handle singularity
    return 2.f * std::atan2(x, w);

  return std::atan2(temp_y, temp_x);
}

float Quaternion::GetYaw() const
{
  return std::asin(
    OMath::clamp(-2.f * (x*z- w*y), -1.f, 1.f)
  );
}

Vec3 Quaternion::GetEulerAngles() const
{
  float sqw = w*w;
  float sqx = x*x;
  float sqy = y*y;
  float sqz = z*z;

  float roll = std::atan2(2.f * (x*y + w*z), sqw + sqx - sqy - sqz);
  
  float temp_y = 2.f * (y*z + w*x);
  float temp_x = sqw - sqx - sqy + sqz;

  float pitch = 0.f;
  if (temp_y == 0.f && temp_x == 0.f) //avoid atan2(0,0) - handle singularity
    pitch =  2.f * std::atan2(x, w);
  else
    pitch = std::atan2(temp_y, temp_x);

  float yaw = std::asin( OMath::clamp(-2.f * (x*z - w*y), -1.f, 1.f)  );

  return Vec3(pitch * RAD2DEG, yaw * RAD2DEG, roll * RAD2DEG);
}

//alternate implementation that tackles singularities
#if 0
Vec3 Quaternion::GetEulerAngles() const
{

}
#endif
Quaternion & Quaternion::operator+=(const Quaternion & rhs)
{
  w += rhs.w;
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Quaternion & Quaternion::operator-=(const Quaternion & rhs)
{
  w -= rhs.w;
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Quaternion Quaternion::operator+(const Quaternion & rhs) const
{
  Quaternion result(*this);
  return result += rhs;
}

Quaternion Quaternion::operator-(const Quaternion & rhs) const
{
  Quaternion result(*this);
  return result -= rhs;
}

Quaternion & Quaternion::operator*=(float scalar)
{
  w *= scalar;
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

Quaternion Quaternion::operator*(float k) const
{
  Quaternion result(*this);
  return result *= k;
}

bool Quaternion::operator==(const Quaternion & rhs) const
{
  return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

bool Quaternion::operator!=(const Quaternion & rhs) const
{
  return !(*this == rhs);
}

Quaternion & Quaternion::Conjugate()
{
  x = -x;
  y = -y;
  z = -z;
  return *this;
}

Quaternion& Quaternion::Inverse()
{
  x = -x; y = -y; z = -z;
  float sqmag = x*x + y*y + z*z + w*w;
  if(sqmag != 1.f && sqmag != 0.f)    //if unit quaternion, inverse == conjugate
  {
    float inv_sqmag = 1.f/sqmag;
    w *= inv_sqmag;
    x *= inv_sqmag;
    y *= inv_sqmag;
    z *= inv_sqmag;
  }
  return *this;
}

Quaternion & Quaternion::Normalize()
{
  float mag = Norm();
  //prevent division by zero and normalizing a unit quaternion
  if (mag != 1.f && mag != 0.f)
  {
    float invMag = 1.f/mag;
    x *= invMag;
    y *= invMag;
    z *= invMag;
    w *= invMag;
  }
  return *this;
}

float Quaternion::Norm() const
{
  return std::sqrtf(w*w + x*x + y*y + z*z);
}

