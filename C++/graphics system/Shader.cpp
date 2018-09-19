/******************************************************************************/
/*!
\file		Shader.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Graphics/ogl.h"

static const std::string DIRECTORY = "Data/ShaderFiles";

unsigned Shader::activeShaderID = 0;

//definitions
Shader::Shader(const char* vertfile, const char* fragfile)
{
  //use filestream to get shader code
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  //concatentate directory + filename 
  std::string vertShader = DIRECTORY + "/" + std::string(vertfile);
  std::string fragShader = DIRECTORY + "/" + std::string(fragfile);

  //exception handling
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try
  {
    //load shader files and convert to std::string
    vShaderFile.open(vertShader);
    fShaderFile.open(fragShader);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vShaderFile.close();
    fShaderFile.close();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  }
  catch (std::ifstream::failure e)
  {
    std::cout << "ERROR READING SHADER FILE" << std::endl;
  }

  const char* vShaderCode = vertexCode.c_str();
  const char * fShaderCode = fragmentCode.c_str();

  //compile vertex and fragment shader
  unsigned int vertex, fragment;

  // vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  CheckCompilation(vertex, "VERTEX");

  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  CheckCompilation(fragment, "FRAGMENT");

  // link vertex and fragment into shader program
  m_id = glCreateProgram();
  glAttachShader(m_id, vertex);
  glAttachShader(m_id, fragment);
  glLinkProgram(m_id);
  CheckCompilation(m_id, "PROGRAM");

  //after linking, cleanup
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::Use() const
{
  if (m_id != Shader::activeShaderID)
  {
    glUseProgram(m_id);
    Shader::activeShaderID = m_id;
  }
}

void Shader::SetBool(const std::string & attribName, bool value) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniform1i(loc, (int)value);
}

void Shader::SetMatrix4(const std::string & attribName, const Mtx4& mtx, bool transpose) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniformMatrix4fv(loc, 1, transpose, (GLfloat*)mtx.data);
}

void Shader::SetMatrix3(const std::string & attribName, const Mtx3 & mtx, bool transpose) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniformMatrix3fv(loc, 1, transpose, (GLfloat*)mtx.data);
}

void Shader::SetFloat(const std::string & attribName, float value) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniform1f(loc, value);
}

void Shader::SetInt(const std::string & attribName, int value) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniform1i(loc, value);
}

void Shader::SetVec4(const std::string & attribName, const Vec4 & v) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void Shader::SetVec3(const std::string & attribName, const Vec3& v) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniform3f(loc, v.x, v.y, v.z);
}

void Shader::SetVec2(const std::string & attribName, const Vec2 & v) const
{
  Use();
  GLint loc = glGetUniformLocation(m_id, attribName.c_str());
  //Assert("Unable to find uniform attribute in shader: " + attribName, loc != -1, ErrorLevel::Warning);
  glUniform2f(loc, v.x, v.y);
}

void Shader::CheckCompilation(unsigned shader, const std::string & type)
{
  int success;
  char infoLog[1024];
  if (type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "SHADER COMPILATION ERROR " << type << "\n" << infoLog <<
        "\n -- --------------------------------------------------- -- " << std::endl;
    }

#ifdef SHADERDEBUG_ON
    else
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "SHADER COMPILE INFO" << type << "\n" << infoLog <<
        "\n -- --------------------------------------------------- -- " << std::endl;
    }
#endif

  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "PROGRAM LINKING ERROR " << type << "\n" << infoLog <<
        "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
}