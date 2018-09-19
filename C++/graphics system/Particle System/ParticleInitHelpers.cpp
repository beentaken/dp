/******************************************************************************/
/*!
\file		ParticleInitHelpers.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
void InitForce(Particle & p, const Vec3& dir, const EmitterConfig & cf, bool random)
{
  //if not random take first value of force, dir assumed to be normalised
  float actual_vel = random ? Random(cf.vel_range.x, cf.vel_range.y) : cf.vel_range.x;
  p.velocity = actual_vel * dir;
  p.acceleration = cf.accel;
}

void InitLifetime(Particle & p, const EmitterConfig & cf, bool random)
{
  float actual_life = random ? Random(cf.lifeTime.x, cf.lifeTime.y) : cf.lifeTime.x;
  p.lifetime = p.initialLife = actual_life;
}

void InitColor(Particle & p, const EmitterConfig & cf, bool random)
{
  p.tintColor = Vec4(cf.initialColor.x, cf.initialColor.y, cf.initialColor.z, cf.initialAlpha);
}

void InitScale(Particle & p, const EmitterConfig& cf, bool random)
{
  //random not implemented yet
  p.scale = cf.initialScale;
}


//to implement for random/interpolated color
/*
//depending on colortype, init colors
switch (m_config.colortype)
{
case ColorType::Interpolate:
{
//init with min color
p.color = m_config.minRGBA;

//calculate color delta to reach final color
if (p.lifetime > 0.0001f)
{
p.colorDelta = Vec4(
(m_config.maxRGBA.r - m_config.minRGBA.r / p.lifetime),
(m_config.maxRGBA.g - m_config.minRGBA.g / p.lifetime),
(m_config.maxRGBA.b - m_config.minRGBA.b / p.lifetime),
(m_config.maxRGBA.a - m_config.minRGBA.a / p.lifetime)
);
}
break;
}

case ColorType::Random:
{
//init with random color
p.color = Vec4(Random<float>(m_config.minRGBA.r, m_config.maxRGBA.r),
Random<float>(m_config.minRGBA.g, m_config.maxRGBA.g),
Random<float>(m_config.minRGBA.b, m_config.maxRGBA.b),
Random<float>(m_config.minRGBA.a, m_config.maxRGBA.a));

if (p.lifetime > 0.0001f)
{
p.colorDelta = Vec4(0.0f, 0.0f, 0.0f, -1.0f / p.lifetime);
}
break;
}

case ColorType::None:
{
p.color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
if (p.lifetime > 0.0001f)
{
p.colorDelta = Vec4(0.0f, 0.0f, 0.0f, -1.0f / p.lifetime);
}
break;
}
default:
{
p.color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
if (p.lifetime > 0.0001f)
{
p.colorDelta = Vec4(0.0f, 0.0f, 0.0f, -1.0f / p.lifetime);
}
break;
}
}
*/