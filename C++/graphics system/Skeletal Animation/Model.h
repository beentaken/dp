/******************************************************************************/
/*!
\file		Model.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef _MODEL_H
#define _MODEL_H
#pragma once

#include <unordered_map>
#include "Graphics\Shader.h"
#include "Mesh.h"

class ModelRenderer;
class Model
{
public:
  Model(const std::string& path, const std::string& file);
  void LoadModel();
  void LoadMaterials();
  void SendToGL();
  bool isAnimated() const;
  void Draw(const Shader& shader);
  void DrawWithoutTexture(const Shader& shader);
  void DrawWireframe(const Shader& shader);
  void Terminate();

  unsigned GetNumBones() const;
  std::string GetDirectory() const;
  std::string GetName() const;
  std::string& GetName();
  std::vector<Mtx4> UpdateBoneMatrices(unsigned animIndex, float elapsed) const;
  std::vector<Mtx4> UpdateBoneMatricesForBlending(unsigned currAnimIndex, unsigned nextAnimIndex, float blendFactor);
  Vec3 GetScaleDifference() const;
  const AnimScene* GetAnimScene() const;

private:
  //model loading and post processing
  void PostProcessModel();
  void SendModelData();
  void InitFromScene(const AnimScene& scene);
  Mesh ProcessMesh(const AMesh& mesh);

  //material loading
  Material LoadMaterial(const AMaterial& material);
  Texture* CreateTextureFromType(const AMaterial& material, const std::string& name);

  //load skeleton
  void LoadBonesInMesh(const AMesh& mesh, std::vector<Vertex>& vertices);
  
  //per model animation and render data
  std::vector<Mesh> m_meshes;
  std::unordered_map<std::string, unsigned> m_AnimToIndexMap;
  AnimScene m_animScene;
  Skeleton m_skeleton;

  //other utility data
  Vec3 min_bounds;
  Vec3 max_bounds;
  std::pair<Vec3, Vec3> aabb;
  std::string directory;
  std::string name;
  bool m_isAnimated;
};

#endif

