/******************************************************************************/
/*!
\file		Model.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include <limits>
#include "Model.h"
#include "Utility\Logging.h"
#include "Utility\FrameRateController.h"
#include "ModelDeserialization.h"

#define MAX_BONES 128         //should be same as shader

Model::Model(const std::string& path, const std::string& file) : m_isAnimated(false), directory(path), name(file) {}

void Model::LoadModel()
{
  std::string path = directory;
  //check type of file
  std::string type = name.substr(name.find_first_of(".") + 1);
  directory = path.substr(0, path.find_last_of('/'));
  // name = file;
  Assert("Engine only accepts lmf file type as models", type == "lmf", ErrorLevel::SystemCrash);

  //open file
  std::ifstream inFile(path, std::ios_base::in);
  if (!inFile)
    std::cout << "Opening file for input failed" << std::endl;

  m_animScene.SetMeshes(std::move(DeserializeMeshes(inFile)));
  m_animScene.SetMaterials(std::move(DeserializeVector<AMaterial>(inFile)));

  ANode* node = nullptr;

  //check if animations exist
  char test = inFile.peek();
  if (test == 'A')
  {
    m_animScene.SetAnimationData(std::move(DeserializeAnimations(inFile)));

    std::string check;
    inFile >> check;
    if (check != "Animation_Hierarchy:")
      assert(false);

    m_animScene.SetRootNode(DeserializeTree(inFile));
  }

  inFile.close();

  InitFromScene(m_animScene);

  //do post processing
  PostProcessModel();
}

void Model::LoadMaterials()
{
  // Process materials
  const std::vector<AMaterial>& materials = m_animScene.GetMaterials();
  for (auto & elem : m_meshes)
  {
    unsigned matid = elem.m_material.materialIndex;
    if (matid >= 0)
    {
      elem.m_material = LoadMaterial(materials[matid]);
      elem.m_material.materialIndex = matid;
    }
  }
}

void Model::SendToGL()
{
  //Send mesh data to gl
  SendModelData();
}

void Model::Draw(const Shader & shader)
{
  //call draw on each mesh
  for (size_t i = 0; i < m_meshes.size(); ++i)
  {
    m_meshes[i].Draw(shader);
  }
}

void Model::DrawWithoutTexture(const Shader & shader)
{
  //call draw on each mesh
  for (size_t i = 0; i < m_meshes.size(); ++i)
  {
    m_meshes[i].DrawWithoutTexture(shader);
  }
}

void Model::DrawWireframe(const Shader & shader)
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glLineWidth(3);
  //call draw on each mesh
  for (size_t i = 0; i < m_meshes.size(); ++i)
  {
    m_meshes[i].DrawWithoutTexture(shader);
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

std::vector<Mtx4> Model::UpdateBoneMatrices(unsigned animIndex, float elapsed) const
{
  //update bone transforms
  return m_skeleton.UpdateBoneTransforms(elapsed, &m_animScene, animIndex);
}

std::vector<Mtx4> Model::UpdateBoneMatricesForBlending(unsigned currAnimIndex, unsigned nextAnimIndex, float blendFactor)
{
  return m_skeleton.UpdateBoneTransformsForBlending(blendFactor, &m_animScene, currAnimIndex, nextAnimIndex);
}

void Model::InitFromScene(const AnimScene & scene)
{
  //set the transformation that goes from mesh space to model space
  const ANode* rootNode = scene.GetRootNode();
  if (rootNode)
  {
    m_skeleton.m_globalInverseTransform = (scene.GetRootNode()->m_transformation).inverse();
  }

  //get total number of bones
  unsigned boneCount = 0;
  const std::vector<AMesh>& meshes = scene.GetMeshes();
  size_t numMesh = meshes.size();
  for (size_t i = 0; i < numMesh; ++i)
  {
    size_t numBones = meshes[i].m_bones.size();
    boneCount += numBones;
  }
  m_skeleton.m_boneOffsetMatrices.reserve(boneCount);

  //add all meshes using move construct
  m_meshes.reserve(numMesh);
  for (unsigned i = 0; i < numMesh; ++i)
  {
    m_meshes.emplace_back(std::move(ProcessMesh(meshes[i])));
  }

  const std::vector<AnimationData>& anims = scene.GetAnimationData();
  size_t numAnims = anims.size();

  //if scene has animations, scene is animated
  if (numAnims > 0)
    m_isAnimated = true;
}

Mesh Model::ProcessMesh(const AMesh & mesh)
{
  Mesh result;
  std::vector<Vertex> & vertices = result.m_vertices;
  std::vector<unsigned> & indices = result.m_indices;

  //copy vertices from assimp
  size_t numVerts = mesh.m_vertices.size();
  result.m_vertices.reserve(numVerts);
  for (size_t i = 0; i < numVerts; ++i)
  {
    Vertex vertex;
    //copy vertex positions and normals
    vertex.position = mesh.m_vertices[i];
    vertex.normal = mesh.m_normals[i];

    //if mesh contains texture coordinates, copy the first one (assimp allows 8 texture coordinate per vertex)
    size_t numUVs = mesh.m_textureCoords.size();
    if (numUVs > 0)
      vertex.texCoords = mesh.m_textureCoords[i];
    else
      vertex.texCoords = Vec2();

    //add this vertex to mesh
    vertices.push_back(vertex);
  }

  //load bones
  LoadBonesInMesh(mesh, vertices);

  size_t numFaces = mesh.m_faces.size();
  result.m_indices.reserve(numFaces);
  for (unsigned i = 0; i < numFaces; ++i)
  {
    const AFace& Face = mesh.m_faces[i];
    //Assert("Model::ProcessMesh, Mesh is not triangulated properly", Face.mNumIndices == 3);
    indices.push_back(Face.index1);
    indices.push_back(Face.index2);
    indices.push_back(Face.index3);
  }

  //process materials later
  result.m_material.materialIndex = mesh.m_materialIndex;

  return result;
}

void Model::LoadBonesInMesh(const AMesh & mesh, std::vector<Vertex>& vertices)
{
  size_t numBones = mesh.m_bones.size();
  if (numBones <= 0)
    return;

  Assert("Number of bones in a mesh exceed maximum number of bones",  numBones <= MAX_BONES, ErrorLevel::SystemCrash);

  //for each bone, find the vertex affected by it and add its data.
  for (unsigned i = 0; i < numBones; ++i)
  {
    unsigned boneIndex = 0;
    std::string boneName = mesh.m_bones[i].bone_name;
    BoneMapping& boneMap = m_skeleton.m_boneMapping;
    //if a bonemap doesnt exist already, add it
    if (boneMap.find(boneName) == boneMap.end())
    {
      boneIndex = m_skeleton.m_numBones++;
      //m_skeleton.m_boneOffsetMatrices.push_back(mesh->mBones[i]->mOffsetMatrix);
      m_skeleton.m_boneOffsetMatrices.push_back(mesh.m_bones[i].boneOffsetMtx);
      boneMap[boneName] = boneIndex;
    }
    else
    {
      boneIndex = boneMap[boneName];
    }

    //now that we have the boneIndex, we can add the bone data for each vertex
    size_t numWeights = mesh.m_bones[i].m_boneWeights.size();
    for (unsigned j = 0; j < numWeights; ++j)
    {
      //find the vertex id for this mesh (pair.first)
      unsigned vertexID = mesh.m_bones[i].m_boneWeights[j].first;
      vertices[vertexID].boneData.add(boneIndex, mesh.m_bones[i].m_boneWeights[j].second);
    }
  }
}

void Model::Terminate()
{
  for (size_t i = 0; i < m_meshes.size(); ++i)
  {
    m_meshes[i].Terminate();
  }
  m_animScene.Terminate();
}

void Model::SendModelData()
{
  for (auto& elem : m_meshes)
  {
    elem.Init();
  }
}

bool Model::isAnimated() const
{
  return m_isAnimated;
}

unsigned Model::GetNumBones() const
{
  return m_skeleton.m_numBones;
}

std::string Model::GetDirectory() const
{
  return directory;
}

std::string Model::GetName() const
{
  return name;
}

std::string & Model::GetName() 
{
  return name;
}

Vec3 Model::GetScaleDifference() const
{
  return max_bounds - min_bounds;
}

const AnimScene * Model::GetAnimScene() const
{
  return &m_animScene;
}

Material Model::LoadMaterial(const AMaterial & srcMaterial)
{
  Material material;
  material.diffuseMap = material.normalMap = material.specularMap = nullptr;

  if (srcMaterial.diffuseMapCount > 0)
    material.diffuseMap = CreateTextureFromType(srcMaterial, "texture_diffuse");

  if (srcMaterial.specularMapCount > 0)
    material.specularMap = CreateTextureFromType(srcMaterial, "texture_specular");

  if (srcMaterial.normalMapCount > 0)
    material.normalMap = CreateTextureFromType(srcMaterial, "texture_normal");

  return material;
}

Texture * Model::CreateTextureFromType(const AMaterial & material, const std::string & name)
{
  static TextureManager& mgr = TextureManager::GetInstance();

  //gets texture file location
  std::string file;
  if (name == "texture_diffuse")
    file = material.diffusemap_file;
  else if (name == "texture_specular")
    file = material.specularmap_file;
  else if (name == "texture_normal")
    file = material.normalmap_file;

  //get texture absolute file location
  size_t pos = file.find_last_of("\\");
  if (pos != std::string::npos)
    file = file.substr(pos + 1);

  //all model texture has already been loaded in GraphicsSystem Init, so just get the ptr
  Texture* texture = mgr.GetModelTexture(file);
  //if this is the first time this texture is retrieved, set its type.
  if(texture->id != 0 && texture->type == "")
    texture->type = name;
  Assert("Texture is not loaded", mgr.isModelTextureLoaded(file), ErrorLevel::SystemCrash);

  return texture;
}

void Model::PostProcessModel()
{
  //calculate center
  float min_X, min_Y, min_Z, max_X, max_Y, max_Z;
  min_X = min_Y = min_Z = (std::numeric_limits<float>::max)();
  max_X = max_Y = max_Z = (std::numeric_limits<float>::min)();

  for (auto& elem : m_meshes)
  {
    auto& vertices = elem.m_vertices;
    for (size_t i = 0; i < elem.m_vertices.size(); ++i)
    {
    min_X = (std::min)(min_X, vertices[i].position.x);
    max_X = (std::max)(max_X, vertices[i].position.x);

    min_Y = (std::min)(min_Y, vertices[i].position.y);
    max_Y = (std::max)(max_Y, vertices[i].position.y);

    min_Z = (std::min)(min_Z, vertices[i].position.z);
    max_Z = (std::max)(max_Z, vertices[i].position.z);
    }
  }

  // Calculating Center and unit scale
  Vec3 temp(max_X + min_X, max_Y + min_Y, max_Z + min_Z);
  Vec3 center = temp * 0.5f;

  //offset to center
  for (auto& elem : m_meshes)
  {
    auto& vertices = elem.m_vertices;
    for (size_t i = 0; i < elem.m_vertices.size(); ++i)
    {
      vertices[i].position -= center;
    }
  }

  //unit scale everything and default scale again in the editor
  float xDiff = max_X - min_X;
  float yDiff = max_Y - min_Y;
  float zDiff = max_Z - min_Z;
  xDiff = (xDiff == 0) ? 1.0f : xDiff;
  yDiff = (yDiff == 0) ? 1.0f : yDiff;
  zDiff = (zDiff == 0) ? 1.0f : zDiff;

  Mtx3 norm_Scale = Mtx3::scale(1.0f / xDiff, 1.0f / yDiff, 1.0f / zDiff);

  for (auto& elem : m_meshes)
  {
    auto& vertices = elem.m_vertices;
    for (size_t i = 0; i < elem.m_vertices.size(); ++i)
    {
      vertices[i].position = norm_Scale * vertices[i].position;
    }
  }
  
  //assign bounding box for default scale
  min_bounds = Vec3(min_X, min_Y, min_Z);
  max_bounds = Vec3(max_X, max_Y, max_Z);

  //normalized aabb for raycast
  aabb = std::pair<Vec3, Vec3>{ norm_Scale * min_bounds, norm_Scale* max_bounds };
 
  if (m_isAnimated)
  {
    //change bone transforms to account for centralizing and unit scaling of all vertices
    Vec3 norm_trans = norm_Scale * center;
    m_skeleton.m_translateToCenter = Mtx4::translate(-norm_trans);
    Mtx4 normalizedToLocal = Mtx4::translate(center) * Mtx4::scale(xDiff, yDiff, zDiff);          //inverse of centralizing and unitscaling
    m_skeleton.TransformBoneOffsetMatrices(normalizedToLocal);
  }

}

