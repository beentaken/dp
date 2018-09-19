#ifndef ANIMATION_CLASSES_H
#define ANIMATION_CLASSES_H

#include <vector>
#include "Math\OMath.h"
#include "assimp\scene.h"

//store and convert assimp data structures

struct ANode
{
  ANode() = default;
  inline ~ANode()
  {
    m_children.clear();
    m_meshIndices.clear();
    m_parent = nullptr;
  }
  std::vector<ANode*> m_children;
  std::vector<unsigned> m_meshIndices;         //each is a index into mesh array in scene
  std::string m_name;
  ANode* m_parent;                             //dont think need parent ptr
  Mtx4 m_transformation;
};

//declarations
std::vector<unsigned> CopyMeshIndices(aiNode* node);
std::vector<ANode*> CopyChildren(aiNode* root, ANode* parent);
ANode* CopyTree(aiNode* root, ANode* parent);
void FreeNode(ANode* node);
std::string GetTextureName(aiMaterial* pMaterial, aiTextureType type, const std::string & name);


struct VectorKey
{
  VectorKey() = default;
  inline explicit VectorKey(const aiVectorKey& key)
  {
    time = key.mTime;
    value = key.mValue;
  }
  double time;
  Vec3 value;
};

struct QuatKey
{
  QuatKey() = default;
  inline explicit QuatKey(const aiQuatKey& key)
  {
    time = key.mTime;
    value = Quaternion(key.mValue);
  }
  double time;
  Quaternion value;
};

struct NodeAnimation
{ 
  NodeAnimation() = default;
  inline explicit NodeAnimation(aiNodeAnim* anim)
  {
    m_nodeName = std::string(anim->mNodeName.data);
    m_PostState = anim->mPostState;
    m_PreState = anim->mPreState;
    m_PositionKeys.reserve(anim->mNumPositionKeys);

    for (size_t i = 0; i < anim->mNumPositionKeys; ++i)
    {
      m_PositionKeys.push_back(VectorKey(anim->mPositionKeys[i]));
    }
    m_RotationKeys.reserve(anim->mNumRotationKeys);

    for (size_t i = 0; i < anim->mNumRotationKeys; ++i)
    {
      m_RotationKeys.push_back(QuatKey(anim->mRotationKeys[i]));
    }
    m_ScalingKeys.reserve(anim->mNumScalingKeys);

    for (size_t i = 0; i < anim->mNumScalingKeys; ++i)
    {
      m_ScalingKeys.push_back(VectorKey(anim->mScalingKeys[i]));
    }
  }
  std::string m_nodeName;                     //name of the node of this animation
  std::vector<VectorKey> m_PositionKeys;
  std::vector<VectorKey> m_ScalingKeys;
  std::vector<QuatKey> m_RotationKeys;
  unsigned m_PreState;
  unsigned m_PostState;
};

struct AnimationData
{
  AnimationData() = default;
  inline explicit AnimationData(aiAnimation* anim)
  {
    m_duration = anim->mDuration;
    m_TicksPerSecond = anim->mTicksPerSecond;
    m_animationName = std::string(anim->mName.data);
    m_nodeAnimations.reserve(anim->mNumChannels);
    for (size_t i = 0; i < anim->mNumChannels; ++i)
    {
      m_nodeAnimations.push_back(NodeAnimation(anim->mChannels[i]));
    }
  }

  std::vector<NodeAnimation> m_nodeAnimations;
  double m_duration;                          //duration in frames
  double m_TicksPerSecond;                    //frames per second
  std::string m_animationName;                //name of the animation
};


struct ABone
{
  ABone() = default;
  inline explicit ABone(aiBone* bone)
  {
    m_boneWeights.reserve(bone->mNumWeights);
    bone_name = std::string(bone->mName.C_Str());
    boneOffsetMtx = Mtx4(bone->mOffsetMatrix);
    for (unsigned i = 0; i < bone->mNumWeights; ++i)
    {
      m_boneWeights.push_back(std::make_pair(
          bone->mWeights[i].mVertexId, bone->mWeights[i].mWeight
          ));
    }
  }
  inline bool operator==(const ABone& rhs) const
  {
    bool a = bone_name == rhs.bone_name;

    bool b = true;
    for (int i = 0; i < 16; ++i)
    {
      b = b & (boneOffsetMtx.data[i] == rhs.boneOffsetMtx.data[i]);
    }
    bool c = m_boneWeights == rhs.m_boneWeights;

    return a && b && c;
  }
  using VertexWeight = std::pair<unsigned, float>;
  std::string bone_name;
  Mtx4 boneOffsetMtx;
  std::vector<VertexWeight> m_boneWeights;
};

struct AFace
{
  AFace() = default;
  inline explicit AFace(const aiFace& face)
  {
    assert(face.mNumIndices == 3);
    index1 = face.mIndices[0];
    index2 = face.mIndices[1];
    index3 = face.mIndices[2];
  }
  inline bool operator==(const AFace& rhs) const
  {
    return index1 == rhs.index1 && index2 == rhs.index2 && index3 == rhs.index3;
  }
  unsigned index1;
  unsigned index2;
  unsigned index3;
};

struct AMaterial
{
  AMaterial() = default;
  inline explicit AMaterial(aiMaterial* pMaterial)
  {
    diffuseMapCount = pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
    specularMapCount = pMaterial->GetTextureCount(aiTextureType_SPECULAR);
    normalMapCount = pMaterial->GetTextureCount(aiTextureType_NORMALS);          

    if (diffuseMapCount > 0)
      diffusemap_file = GetTextureName(pMaterial, aiTextureType_DIFFUSE, "texture_diffuse");

    if (specularMapCount > 0)
      specularmap_file = GetTextureName(pMaterial, aiTextureType_SPECULAR, "texture_specular");

    if (normalMapCount > 0)
      normalmap_file = GetTextureName(pMaterial, aiTextureType_NORMALS, "texture_normal");
  }

  unsigned diffuseMapCount;
  unsigned specularMapCount;
  unsigned normalMapCount;
  std::string diffusemap_file;
  std::string specularmap_file;
  std::string normalmap_file;
};

struct AMesh
{
  AMesh() = default;
  inline explicit AMesh(aiMesh* pMesh)
  {
    m_bones.reserve(pMesh->mNumBones);
    for (size_t i = 0; i < pMesh->mNumBones; ++i)
    {
      m_bones.push_back(ABone(pMesh->mBones[i]));
    }

    m_faces.reserve(pMesh->mNumFaces);
    for (size_t i = 0; i < pMesh->mNumFaces; ++i)
    {
      m_faces.push_back(AFace(pMesh->mFaces[i]));
    }

    m_vertices.reserve(pMesh->mNumVertices);
    for (size_t i = 0; i < pMesh->mNumVertices; ++i)
    {
      const aiVector3D& pVec = pMesh->mVertices[i];
      m_vertices.push_back(Vec3(pVec.x, pVec.y, pVec.z));
    }

    if (pMesh->mTextureCoords[0])
    {
      m_textureCoords.reserve(pMesh->mNumVertices);
      for (size_t i = 0; i < pMesh->mNumVertices; ++i)
      {
        const aiVector3D& pVec = pMesh->mTextureCoords[0][i];
        m_textureCoords.push_back(Vec2(pVec.x, pVec.y));
      }
    }

    m_normals.reserve(pMesh->mNumVertices);
    for (size_t i = 0; i < pMesh->mNumVertices; ++i)
    {
      const aiVector3D& pVec = pMesh->mNormals[i];
      m_normals.push_back(Vec3(pVec.x, pVec.y, pVec.z));
    }

    m_materialIndex = pMesh->mMaterialIndex;
    m_meshName = std::string(pMesh->mName.C_Str());

  }
  std::string m_meshName;
  unsigned m_materialIndex;
  std::vector<Vec3> m_vertices;
  std::vector<Vec2> m_textureCoords;
  std::vector<Vec3> m_normals;
  std::vector<AFace> m_faces;
  std::vector<ABone> m_bones;
};

class AnimScene
{
public:
  AnimScene() : m_rootNode(nullptr) {}
  ~AnimScene() = default;
  AnimScene(const std::vector<AMesh>& meshes, const std::vector<AMaterial>& mats, 
    const std::vector<AnimationData>& anims, ANode* rootNode) :
    m_meshes(meshes),
    m_materials(mats),
    m_animations(anims),
    m_rootNode(rootNode)
  {

  }

  inline void Init(const aiScene* scene)
  {
    //recrusively copy the scene
    m_rootNode = CopyTree(scene->mRootNode, nullptr);

    //copy meshes
    CopyMeshes(scene);

    //copy materials
    CopyMaterials(scene);

    //copy animations
    CopyAnimationData(scene);
  }

  inline void Terminate()
  {
    //safely delete tree
    FreeNode(m_rootNode);
  }
  
  inline const std::vector<AnimationData>& GetAnimationData() const { return m_animations; }
  inline const std::vector<AMesh>& GetMeshes() const { return m_meshes; }
  inline const std::vector<AMaterial>& GetMaterials() const { return m_materials; }
  inline const ANode* GetRootNode() const { return m_rootNode; }

private:
  inline void CopyMeshes(const aiScene* pScene)
  {
    m_meshes.reserve(pScene->mNumMeshes);
    for (size_t i = 0; i < pScene->mNumMeshes; ++i)
    {
      m_meshes.push_back(AMesh(pScene->mMeshes[i]));
    }
  }

  inline void CopyMaterials(const aiScene* pScene)
  {
    m_materials.reserve(pScene->mNumMaterials);
    for (size_t i = 0; i < pScene->mNumMaterials; ++i)
    {
      m_materials.push_back(AMaterial(pScene->mMaterials[i]));
    }
  }

  inline void CopyAnimationData(const aiScene* pScene)
  {
    m_animations.reserve(pScene->mNumAnimations);
    for (size_t i = 0; i < pScene->mNumAnimations; ++i)
    {
      m_animations.push_back(AnimationData(pScene->mAnimations[i]));
    }
  }

  //variables
  std::vector<AMesh> m_meshes;
  std::vector<AMaterial> m_materials;
  std::vector<AnimationData> m_animations;
  ANode* m_rootNode;
};

//utility functions
inline std::vector<unsigned> CopyMeshIndices(aiNode* node)
{
  std::vector<unsigned> result;
  result.reserve(node->mNumMeshes);
  for (unsigned i = 0; i < node->mNumMeshes; ++i)
  {
    result.emplace_back(node->mMeshes[i]);
  }
  return result;
}

inline std::vector<ANode*> CopyChildren(aiNode* root, ANode* parent)
{
  std::vector<ANode*> result;
  result.reserve(root->mNumChildren);

  for (unsigned i = 0; i < root->mNumChildren; ++i)
  {
    ANode* newNode = CopyTree(root->mChildren[i], parent);
    result.emplace_back(newNode);
  }
  return result;
}

inline ANode* CopyTree(aiNode* root, ANode* parent)
{
  if (!root) return nullptr;

  ANode* nNode = new ANode();
  nNode->m_children = CopyChildren(root, nNode);
  nNode->m_meshIndices = CopyMeshIndices(root);
  nNode->m_name = std::string(root->mName.data);
  nNode->m_parent = parent;
  nNode->m_transformation = Mtx4(root->mTransformation);

  return nNode;
}

inline void FreeNode(ANode* node)
{
  if (!node) return;

  for (auto& elem : node->m_children)
  {
    FreeNode(elem);
  }

  delete node;
}


inline std::string GetTextureName(aiMaterial* pMaterial, aiTextureType type, const std::string & name)
{
  aiString str;
  std::string file;
  if (pMaterial->GetTexture(type, 0, &str) == AI_SUCCESS)
    file = std::string(str.C_Str());
  else
    assert(false);

  return file;
}

#endif

