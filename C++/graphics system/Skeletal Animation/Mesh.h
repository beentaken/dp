/******************************************************************************/
/*!
\file		Mesh.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _MESH_H
#define _MESH_H
#pragma once

#include <unordered_map>
#include "Math\OMath.h"
#include "Graphics\TextureManager.h"
#include "Graphics\Skeletal Animation\Skeleton.h"

struct Material
{
  Texture* diffuseMap;
  Texture* specularMap;
  Texture* normalMap;
  unsigned materialIndex;
};

struct Vertex
{
  Vec3 position;
  Vec3 normal;
  Vec2 texCoords;
  VertexBoneData boneData;
};

class Shader;
class Mesh
{
public:
  //public data
  std::vector<Vertex> m_vertices;
  std::vector<unsigned> m_indices;
  Material m_material;

  //constructor
  Mesh() = default;
  //Mesh(const std::vector<Vertex>& verts,
  //  const std::vector<unsigned>& indices,
  //  const Material& material);
  Mesh(Mesh &&);

  //functions
  void Init();
  void Terminate();
  void Draw(const Shader& shader);
  void DrawWithoutTexture(const Shader& shader);

private:
  //integers used by opengl
  GLuint VBO, VAO, EBO;

};

#endif


