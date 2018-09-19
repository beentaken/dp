/******************************************************************************/
/*!
\file		AnimationClasses.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef ANIMATION_CLASSES_H
#define ANIMATION_CLASSES_H

#include <vector>
#include "Math\OMath.h"

//store and convert assimp data structures

//#define NO_ASSIMP

struct ANode
{
  ANode() = default;
  inline ~ANode()
  {
    m_children.clear();
    m_meshIndices.clear();
  }
  std::vector<ANode*> m_children;
  std::vector<unsigned> m_meshIndices;         //each is a index into mesh array in scene
  std::string m_name;
  Mtx4 m_transformation;
};

void FreeNode(ANode* node);


struct VectorKey
{
  VectorKey() = default;
  double time;
  Vec3 value;
};

struct QuatKey
{
  QuatKey() = default;
  double time;
  Quaternion value;
};

struct NodeAnimation
{ 
  NodeAnimation() = default;
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
  std::vector<NodeAnimation> m_nodeAnimations;
  double m_duration;                          //duration in frames
  double m_TicksPerSecond;                    //frames per second
  std::string m_animationName;                //name of the animation
};


struct ABone
{
  ABone() = default;
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

  inline void Terminate()
  {
    //safely delete tree
    FreeNode(m_rootNode);
  }
  
  inline const std::vector<AnimationData>& GetAnimationData() const { return m_animations; }
  inline const std::vector<AMesh>& GetMeshes() const { return m_meshes; }
  inline const std::vector<AMaterial>& GetMaterials() const { return m_materials; }
  inline const ANode* GetRootNode() const { return m_rootNode; }

  inline void SetAnimationData(std::vector<AnimationData> && data) { m_animations = data; }
  inline void SetMeshes(std::vector<AMesh>&& data) { m_meshes = data; }
  inline void SetMaterials(std::vector<AMaterial>&& data) { m_materials = data; }
  inline void SetRootNode(ANode * ptr) { m_rootNode = ptr; }

private:
  //variables
  std::vector<AMesh> m_meshes;
  std::vector<AMaterial> m_materials;
  std::vector<AnimationData> m_animations;
  ANode* m_rootNode;
};

#endif