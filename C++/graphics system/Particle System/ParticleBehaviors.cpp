/******************************************************************************/
/*!
\file		ParticleBehaviors.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Utility\Logging.h"
#include "ParticleBehaviors.h"
#include "Utility\Random.h"

#include "ParticleInitHelpers.cpp"

void DefaultUpdateFunc(Particle& p, const EmitterConfig& ecf, float dt)
{
  //physics update
  p.velocity += p.acceleration * static_cast<float>(dt);
  p.worldPos += p.velocity * dt;

  //reduce lifetime
  p.lifetime -= dt;
}

void StoveParticleFunc(Particle & p, const EmitterConfig & cf, float dt)
{
  static const Vec3 WHITE = Vec3(1.f, 1.f, 1.f);

  Assert("initial life of particle was zero", p.initialLife != 0.f);
  Vec2 scaleToReduce = cf.initialScale / p.initialLife;
  float alphaToReduce = cf.initialAlpha / p.initialLife;

  p.scale -= scaleToReduce * dt;
  p.tintColor.w -= alphaToReduce * dt;

  //physics update
  DefaultUpdateFunc(p, cf, dt);
}

void ChopParticleFunc(Particle & p, const EmitterConfig & cf, float dt)
{
	Assert("initial life of particle was zero", p.initialLife != 0.f);
	float alphaToReduce = cf.initialAlpha / p.initialLife;

	p.tintColor.w -= alphaToReduce * dt;

	//physics update
	DefaultUpdateFunc(p, cf, dt);
}

void BBQParticleFunc(Particle & p, const EmitterConfig & cf, float dt)
{
  Assert("initial life of particle was zero", p.initialLife != 0.f);
  float alphaToReduce = cf.initialAlpha / p.initialLife;

  //random a very small "noise" vector in the x and z directions
  float random_z = Random(-2.f, 2.f);
  p.acceleration += Vec3(0, 0.f, random_z);

  float r_scale = Random(0.1f, 0.2f);
  p.scale += Vec2(r_scale, r_scale);

  p.tintColor.w -= alphaToReduce * dt;

  //physics update
  DefaultUpdateFunc(p, cf, dt);
}

void PanParticleFunc(Particle & p, const EmitterConfig & cf, float dt)
{
  Assert("initial life of particle was zero", p.initialLife != 0.f);
  float alphaToReduce = cf.initialAlpha / p.initialLife;

  //random a very small "noise" vector in the x and z directions
  float random_z = Random(-2.f, 2.f);
  p.acceleration += Vec3(0, 0.f, random_z);

  float r_scale = Random(0.1f, 0.2f);
  p.scale += Vec2(r_scale, r_scale);

  p.tintColor.w -= alphaToReduce * dt;

  //physics update
  DefaultUpdateFunc(p, cf, dt);
}


void CondimentParticleFunc(Particle & p, const EmitterConfig & cf, float dt)
{
	Assert("initial life of particle was zero", p.initialLife != 0.f);
	//Vec2 scaleToReduce = cf.initialScale / p.initialLife;
	//float alphaToReduce = cf.initialAlpha / p.initialLife;
	//
	//p.scale -= scaleToReduce * dt;
	//p.tintColor.w -= alphaToReduce * dt;

	//physics update
	DefaultUpdateFunc(p, cf, dt);
}



void PointInitFunc(Particle& p, EmitterConfig & cf, const Transform & tref)
{
  //randomize horizontal and vertical angles
  float halfAngle = 0.5f * (cf.alphaAngle.y - cf.alphaAngle.x);
  float horz_angle = Random(-halfAngle, halfAngle);
  halfAngle = 0.5f * (cf.betaAngle.y - cf.betaAngle.x);
  float vert_angle = Random(-halfAngle, halfAngle);

  //find normal to emitdirection
  //construct coordinate system based on emitDirection
  Vec3 e_3 = Vec3Normalise(Vec3CrossProduct(cf.emitDir, Vec3::EY()));   //z
  Vec3 e_1 = Vec3Normalise(cf.emitDir);                                 //x
  Vec3 e_2 = Vec3Normalise(Vec3CrossProduct(e_3, e_1));                 //y

 //get final direction using spherical coordinates  
  Vec3 horzDir = Vec3Normalise((cos(horz_angle * DEG2RAD) * e_1) + (sin(horz_angle * DEG2RAD) * e_3));
  Vec3 particleDir = Vec3Normalise((cos(vert_angle * DEG2RAD) * horzDir) + (sin(vert_angle * DEG2RAD) * e_2));

  //init particle values
  p.worldPos = tref.trans_;
  InitForce(p, particleDir, cf);
  InitScale(p, cf);
  InitColor(p, cf);
  InitLifetime(p, cf);
  p.fade_factor = cf.initialFadeFactor;
}


void RingInitFunc(Particle& p, EmitterConfig& cf, const Transform & tref)
{
  static const float two_pi = 2.f * PI;
  CoordinateSystem& basis = cf.ring_src.basis;
  if (basis.e_3 != cf.ring_src.ring_normal)
  {
    //create a new coordinate system using new normal
    Vec3Normalise(cf.ring_src.ring_normal);
    basis.e_3 = cf.ring_src.ring_normal;
    OMath::GenerateOrthonormalBasis(basis.e_3, basis.e_1, basis.e_2);
  }
  float angle = Random(0.f, two_pi);
  
  //find the next spawn point
  Vec3 offset = Vec3Normalise((std::cos(angle) * basis.e_1) + (std::sin(angle) * basis.e_2));
  Vec3 spawnPos = tref.trans_ + (cf.ring_src.radius * offset);

  //init particle properties
  Vec3 particleDir = Vec3Normalise(spawnPos - tref.trans_);
  p.worldPos = spawnPos;
  InitForce(p, particleDir, cf);
  InitScale(p, cf);
  InitColor(p, cf);
  InitLifetime(p, cf);
  p.fade_factor = cf.initialFadeFactor;
}

void LineInitFunc(Particle & p, EmitterConfig & cf, const Transform & tr)
{
  float t = Random(-1.f, 1.f);

  //find a random place on the line to spawn
  Vec3 spawnPos = tr.trans_ + (t * cf.half_line);

  if (cf.emitDir.squaredLength() != 1.f)
    Vec3Normalise(cf.emitDir);

  p.worldPos = spawnPos;
  InitForce(p, cf.emitDir, cf);
  InitScale(p, cf);
  InitColor(p, cf);
  InitLifetime(p, cf);
  p.fade_factor = cf.initialFadeFactor;
}

void DiscInitFunc(Particle & p, EmitterConfig & cf, const Transform & tr)
{
  static const float two_pi = 2.f * PI;
  Vec3 e_1, e_2;
  //create a new coordinate system using new normal
  if(cf.emitDir.squaredLength() != 1.f)
    Vec3Normalise(cf.emitDir);
  OMath::GenerateOrthonormalBasis(cf.emitDir, e_1, e_2);

  float angle = Random(0.f, two_pi);
  float t = Random(0.f, cf.disc_radius);

  //find the next spawn point
  Vec3 offset = Vec3Normalise((std::cos(angle) * e_1) + (std::sin(angle) * e_2));
  Vec3 spawnPos = tr.trans_ + (t * offset);

  p.worldPos = spawnPos;
  InitForce(p, cf.emitDir, cf);
  InitScale(p, cf);
  InitColor(p, cf);
  InitLifetime(p, cf);
  p.fade_factor = cf.initialFadeFactor;
}
