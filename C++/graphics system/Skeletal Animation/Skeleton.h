/******************************************************************************/
/*!
\file		Skeleton.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef SKELETON_H
#define SKELETON_H

#include "Math\OMath.h"
#include "Graphics\Shader.h"
#include "AnimationClasses.h"

#define MAX_BONES_PER_VERTEX 4


struct BoneInfo
{
  Mtx4 BoneOffsetMtx;
  Mtx4 FinalTransform;
};

struct VertexBoneData   //size = 16/64
{
  VertexBoneData();
  unsigned boneIDs[MAX_BONES_PER_VERTEX];
  float weights[MAX_BONES_PER_VERTEX];
  
  //assign id and weight of bone affecting vertex
  void add(unsigned boneID, float weight);
};

using BoneMapping = std::unordered_map<std::string, unsigned>;
class Skeleton
{
public:
  friend class Model;
  Skeleton();
  std::vector<Mtx4> UpdateBoneTransforms(float TimeInSecs, const AnimScene* pAnimScene, unsigned animIndex) const;
  std::vector<Mtx4> UpdateBoneTransformsForBlending(float blendFactor, const AnimScene* pAnimScene, unsigned startAnimIndex, unsigned endAnimIndex) const;

private:
  //use for transforming bone offset due to normalized scale and center
  void TransformBoneOffsetMatrices(const Mtx4& transform);
  //getting the final transform each frame
  void ComputeFinalBoneTransform(float animTime, const AnimationData& aData, const ANode* pNode, const Mtx4& parentTransform,
                                 std::vector<Mtx4>& finalTransforms) const;

  //for interpolation
  Quaternion ComputeInterpolatedRotation(float animTime, const NodeAnimation& nodeAnim) const;
  Vec3 ComputeInterpolatedPosition(float animTime, const NodeAnimation& nodeAnim) const;
  Vec3 ComputeInterpolatedScale(float animTime, const NodeAnimation& nodeAnim) const;

  //for blending
  Mtx4 BlendTransforms(const NodeAnimation& currAnim, const NodeAnimation& nextAnim, float blendFactor) const;
  void ComputeBlendingBoneTransforms(float blendFactor, const AnimationData& current, const AnimationData& next,
                                     const ANode* pNode, const Mtx4& parentTransform, std::vector<Mtx4>& finalTransforms) const;

  //utility functions 
  const NodeAnimation* FindNodeAnim(const AnimationData& animData, const std::string& NodeName) const;
  unsigned FindRotationKey(float animTime, const NodeAnimation& nodeAnim) const;
  unsigned FindScalingKey(float animTime, const NodeAnimation& nodeAnim) const;
  unsigned FindPositionKey(float animTime, const NodeAnimation& nodeAnim) const;
  
  //variables
  BoneMapping m_boneMapping;
  Mtx4 m_globalInverseTransform;
  Mtx4 m_translateToCenter;
  std::vector<Mtx4> m_boneOffsetMatrices;
  unsigned m_numBones;
};

#endif
