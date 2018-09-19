/******************************************************************************/
/*!
\file		ParticleBehaviors.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef PARTICLE_BEHAVIORS_H
#define PARTICLE_BEHAVIORS_H

#include "Particles.h"
#include "Components\Transform.h"

using ParticleUpdateFunc = void(*)(Particle&, const EmitterConfig&, float);
using ParticleInitFunc = void(*)(Particle&, EmitterConfig&, const Transform&);

//different particle update behavior
void DefaultUpdateFunc(Particle& p, const EmitterConfig& cf, float dt);
void StoveParticleFunc(Particle& p, const EmitterConfig& cf, float dt);
void ChopParticleFunc(Particle& p, const EmitterConfig& cf, float dt);
void BBQParticleFunc(Particle& p, const EmitterConfig& cf, float dt);
void PanParticleFunc(Particle& p, const EmitterConfig& cf, float dt);

//helpers to init particle properties
void InitForce(Particle& p, const Vec3& dir, const EmitterConfig& cf, bool random = true);    
void InitLifetime(Particle& p, const EmitterConfig& cf, bool random = true);
void InitColor(Particle& p, const EmitterConfig& cf, bool random = true);     //random not implemented
void InitScale(Particle& p, const EmitterConfig& cf, bool random = true);


//different particle spawn behavior to determine position
void PointInitFunc(Particle& p, EmitterConfig& cf, const Transform& tr);
void RingInitFunc(Particle& p, EmitterConfig& cf, const Transform& tr);
void LineInitFunc(Particle& p, EmitterConfig& cf, const Transform& tr);
void DiscInitFunc(Particle& p, EmitterConfig& cf, const Transform& tr);

#endif

