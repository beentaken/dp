/******************************************************************************/
/*!
\file		LightTypes.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "LightTypes.h"
#include "Graphics\Shader.h"
#include "Utility\Logging.h"

//dont change the values in this map
Light::AttentuationTable Light::atTable
{
  {7u,      { 0.7f,     1.8f } },
  {13u,     { 0.35f,    0.44f } },
  {20u,     { 0.22f,    0.20f } },
  {32u,     { 0.14f,    0.07f } },
  {50u,     { 0.09f,    0.032f } },
  {65u,    { 0.07f,    0.017f } },
  {100u,    { 0.045f,   0.0075f } },
  {160u,    { 0.027f,   0.0028f } },
  {200u,    { 0.022f,   0.0019f } },
  {325u,    { 0.014f,   0.0007f } },
  {600u,   { 0.007f,   0.0002f } },
  {3250u,     { 0.0014f,  0.000007f } }
};


Light::Light() :
  position(Vec3()),
  color(Vec3(1.f, 1.f, 1.f)),
  lightDirection(Vec3()),
  type(Light::Type::None),
  linear(0.1f),
  quadratic(0.01f),
  intensity(0.f),
  innerCone(0.f),
  outerCone(0.f),
  isActive(false)
{
}

void Light::SendLightDataToGL(const Shader & shader)
{
  //send type to shader
  shader.SetInt(indexedName + "type", static_cast<int>(type));

  //send light parameters based on type
  switch (type)
  {
    case Light::Type::Directional:
    {
      shader.SetVec3(indexedName + "direction", lightDirection);
      shader.SetVec3(indexedName + "diffuse", intensity * color);
      break;
    }

    case Light::Type::Spot:
    {
      shader.SetFloat(indexedName + "linearFactor", linear);
      shader.SetFloat(indexedName + "quadFactor", quadratic);
      shader.SetVec3(indexedName + "direction", lightDirection);
      shader.SetFloat(indexedName + "innerCone", std::cosf(innerCone * DEG2RAD));
      shader.SetFloat(indexedName + "outerCone", std::cosf(outerCone * DEG2RAD));
      //fall through
    }

    case Light::Type::Point:
    {
      shader.SetFloat(indexedName + "linearFactor", linear);
      shader.SetFloat(indexedName + "quadFactor", quadratic);
      shader.SetVec3(indexedName + "diffuse", intensity * color);
      shader.SetVec3(indexedName + "position", position);
      break;
    }

    //does nothing
    default:
    {
      break;
    }
  }

}
