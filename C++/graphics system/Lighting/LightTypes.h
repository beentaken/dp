/******************************************************************************/
/*!
\file		LightTypes.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef LIGHTTYPES_H
#define LIGHTTYPES_H

#include "Math\OMath.h"
#include <unordered_map>
#include "Graphics\Shader.h"

using LightID = unsigned;


struct Light
{
  using distance = unsigned;
  using LinearConstant = float;
  using QuadConstant = float;
  using ConstantPair = std::pair<LinearConstant, QuadConstant>;
  using AttentuationTable = std::unordered_map<distance, ConstantPair>;

  enum class Type       //should be same as shader
  {
    Directional = 0,
    Point,
    Spot,
    None
  };

  Light();
  ~Light() = default;
  Light(const Light& data) = default;
  void SendLightDataToGL(const Shader& shader);
  
  //data to serialize
  Vec3 position;
  Vec3 color;
  Vec3 lightDirection;
  Light::Type type;
  LinearConstant linear;
  QuadConstant quadratic;
  float intensity;
  float innerCone;
  float outerCone;
  bool isActive;

  //dont serialize these
  LightID id;
  static AttentuationTable atTable;
  std::string indexedName;
};




#endif


