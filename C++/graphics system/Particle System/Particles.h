/******************************************************************************/
/*!
\File name: ParticleStructs.h
\Project name: 
\Primary Author: Chan Yu Hong
All content © 2017-2018 DigiPen (SINGAPORE) Corporation, All rights reserved.
*/
/******************************************************************************/
#ifndef PARTICLE_STRUCTS_H
#define PARTICLE_STRUCTS_H

#include "Math\OMath.h"

#define MAX_PARTICLES 10000


struct Particle
{
  Particle(const Vec3& position = Vec3(), const Vec3& vel = Vec3(), const Vec4& tint = Vec4(), float lifetime = 0.0f)
    :
    worldPos(position),
    velocity(vel),
    tintColor(tint),
    lifetime(lifetime),
    initialLife(0.f),
    fade_factor(0.f),
    scale(Vec2(1.f, 1.f))
  {}

  Vec4 tintColor;
  Vec3 worldPos;
  Vec3 velocity;
  Vec3 acceleration;
  Vec2 scale;
  float lifetime;
  float initialLife;
  float fade_factor;
};

enum class EmitterType
{
  Point = 0,
  Line,
  Plane,
  Ring,
  Custom
};

enum class ColorType
{
  Interpolate = 0,
  Random,
  None
};

enum class ParticleBehavior   
{
  Normal = 0,
  Spiral,                    //not implemented
  Custom
};

struct CoordinateSystem
{
  Vec3 e_1, e_2, e_3;
};

struct RingSource
{
  Vec3  ring_normal = Vec3(0.f, 1.f, 0.f);
  float radius = 0.f;
  int   num_points = 0;
  float theta = 0.f;                              //tracks the current theta
  CoordinateSystem basis;
};

struct EmitterConfig
{
  RingSource ring_src;
  Vec3 emitDir = Vec3::EY();
  Vec2 alphaAngle = Vec2(0.f, 360.f);       //horizontal angle (x-z plane)
  Vec2 betaAngle = Vec2(0.f, 360.f);        //vertical angle(xz-y plane)
  Vec3 initialColor = Vec3(1.f, 1.f, 1.f);
  Vec2 initialScale = Vec2(1.f, 1.f);
  Vec2 lifeTime = Vec2(1.f, 1.f);
  Vec3 accel = Vec3();
  Vec2 vel_range = Vec2(1.f, 1.f);
  Vec3 half_line = Vec3::EX();
  float disc_radius = 0.f;
  float tintFactor = 0.f;
  float initialAlpha = 1.f;
  float initialFadeFactor = 1.f;
  float emitRate = 1.f;
  ParticleBehavior behavior;
  EmitterType type = EmitterType::Point;
};




#endif

