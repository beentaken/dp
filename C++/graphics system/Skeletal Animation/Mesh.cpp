/******************************************************************************/
/*!
\file		Mesh.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Mesh.h"
#include "Graphics\Shader.h"
#include "Graphics\ogl.h"
#include "Graphics\Lighting\ShadowMapping.h"
#include <cstddef>
#include <string>

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

//Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned>& indices,
//  const Material& material) :
//  m_vertices(verts),
//  m_indices(indices),
//  m_material(material)
//{
//}

Mesh::Mesh(Mesh && rhs) : m_vertices(std::move(rhs.m_vertices)), m_indices(rhs.m_indices), m_material(std::move(rhs.m_material)) {}

void Mesh::Init()
{
  //generate buffers and objects
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  //initialize VAO
  glBindVertexArray(VAO);
  //VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // structs and classes have memory layout is sequential 
  // pass a pointer to the struct and it translates perfectly to a structure of 8 floats
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
  //EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned), m_indices.data(), GL_STATIC_DRAW);

  //vertex positions
  glEnableVertexAttribArray(POSITION_LOCATION);
  glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  //vertex normals
  glEnableVertexAttribArray(NORMAL_LOCATION);
  glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));     //offset =  3*sizeof(GLfloat)

  //vertex texture coords
  glEnableVertexAttribArray(TEX_COORD_LOCATION);
  glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));  //offset = 6*sizeof(GLfloat)

  //bone ids (integers)
  glEnableVertexAttribArray(BONE_ID_LOCATION);
  glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneData.boneIDs));  //offset = 8*sizeof(GLfloat)

  //bone weights
  glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
  glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneData.weights));  //offset = 8*sizeof(GLfloat) + 4* sizeof(unsigned)

  //unbind VAO (dont unbind EBO)
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Mesh::Draw(const Shader & shader)
{
  //texture unit 0 = diffuse
  //texture unit 1 = specular
  //texture unit 2 = normal
  //texture unit 3 = height
  //texture unit 4 = shadow

  //bind diffuse map to texture unit 0
  glActiveTexture(GL_TEXTURE0);
  shader.SetInt("u_material.texture_diffuse", 0);           //texture sampler names must follow the shader
  if (m_material.diffuseMap)
  {
    glBindTexture(GL_TEXTURE_2D, m_material.diffuseMap->id);
  }

  //bind specular map to texture unit 1
  glActiveTexture(GL_TEXTURE0 + 1);
  shader.SetInt("u_material.texture_specular", 1);
  if (m_material.specularMap)
  {
    glBindTexture(GL_TEXTURE_2D, m_material.specularMap->id);
  }

  //bind normal map to texture unit 2
  glActiveTexture(GL_TEXTURE0 + 2);
  shader.SetInt("u_material.texture_normal", 2);
  if (m_material.normalMap)
  {
    glBindTexture(GL_TEXTURE_2D, m_material.normalMap->id);
  }

  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::DrawWithoutTexture(const Shader & shader)
{
  shader.Use();
  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::Terminate()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}


