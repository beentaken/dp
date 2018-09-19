/******************************************************************************/
/*!
\file		Shader.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _SHADER_H
#define _SHADER_H
#pragma once

#include "Math\OMath.h"

class Shader
{
public:
  static unsigned activeShaderID;       //shared between all shaders
  unsigned m_id;
  //constructor
  Shader() : m_id(0) {};
  Shader(const char* vertfile, const char* fragfile);

  //activate this shader for usage if not currently active
  void Use() const;

  //utility uniform functions
  void SetBool(const std::string& attribName, bool value) const;
  void SetMatrix4(const std::string& attribName, const Mtx4& mtx, bool transpose = false) const;
  void SetMatrix3(const std::string& attribName, const Mtx3& mtx, bool transpose = false) const;
  void SetFloat(const std::string& attribName, float value) const;
  void SetInt(const std::string &attribName, int value) const;
  void SetVec4(const std::string & attribName, const Vec4& v) const;
  void SetVec3(const std::string & attribName, const Vec3& v) const;
  void SetVec2(const std::string & attribName, const Vec2& v) const;

private:
  void CheckCompilation(unsigned shader, const std::string& type);
};

#endif
