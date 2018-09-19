/******************************************************************************/
/*!
\file		Skeleton.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "Skeleton.h"

#define DEFAULT_FRAMES_PER_SEC 24.0

VertexBoneData::VertexBoneData()
{
  //zero memory
  memset(&boneIDs, 0, sizeof(unsigned) * MAX_BONES_PER_VERTEX);
  memset(&weights, 0, sizeof(float) * MAX_BONES_PER_VERTEX);
}

void VertexBoneData::add(unsigned boneID, float weight)
{
  for (unsigned i = 0; i < MAX_BONES_PER_VERTEX; i++)
  {
    if (weights[i] == 0.0f)
    {
      boneIDs[i] = boneID;
      weights[i] = weight;
      //ensure we dont write on any other slot in this vertex bonedata
      return;
    }
  }

  Assert("Number of bones affecting this vertex exceeds the maximum", false, ErrorLevel::SystemCrash);
}

Skeleton::Skeleton() : m_numBones(0){}

std::vector<Mtx4> Skeleton::UpdateBoneTransforms(float elapsed, const AnimScene * pAnimScene, unsigned animIndex) const
{
  std::vector<Mtx4> finalMatrices(m_boneOffsetMatrices.size(), Mtx4());
  assert(pAnimScene != nullptr);
  const std::vector<AnimationData>& allAnims = pAnimScene->GetAnimationData();
  assert(allAnims.size() != 0);

  const AnimationData& currAnimData = allAnims[animIndex];
  Mtx4 identity;
  //use number of ticks to calculate current animation time
  float TicksPerSecond = static_cast<float>(currAnimData.m_TicksPerSecond != 0 ? currAnimData.m_TicksPerSecond : DEFAULT_FRAMES_PER_SEC);

  //how many frames has elapsed since game started?
  float TimeInTicks = elapsed * TicksPerSecond;

  //mod the animation duration to get the animation time to interpolate
  float animationTime = std::fmod(TimeInTicks, static_cast<float>(currAnimData.m_duration));

  //recursively compute the final bone transform
  ComputeFinalBoneTransform(animationTime, currAnimData, pAnimScene->GetRootNode(), identity, finalMatrices);
  return finalMatrices;
}

std::vector<Mtx4> Skeleton::UpdateBoneTransformsForBlending(float blendFactor, const AnimScene * pAnimScene, unsigned startAnimIndex, unsigned endAnimIndex) const
{
  std::vector<Mtx4> finalMatrices(m_boneOffsetMatrices.size(), Mtx4());
  assert(pAnimScene != nullptr);
  const std::vector<AnimationData>& allAnims = pAnimScene->GetAnimationData();
  assert(allAnims.size() != 0);

  const AnimationData& startAnimData = allAnims[startAnimIndex];
  const AnimationData& endAnimData = allAnims[endAnimIndex];
  Mtx4 identity;

  //recursively compute the final blended bone transform
  ComputeBlendingBoneTransforms(blendFactor, startAnimData, endAnimData, pAnimScene->GetRootNode(), identity, finalMatrices);
  return finalMatrices;
}

void Skeleton::TransformBoneOffsetMatrices(const Mtx4 & transform)
{
  for (auto& elem : m_boneOffsetMatrices)
  {
    elem = elem * transform;
  }
}

void Skeleton::ComputeFinalBoneTransform(float animTime, const AnimationData & aData, const ANode * pNode, const Mtx4 & parentTransform,
    std::vector<Mtx4>& finalTransforms) const
{
  //check if animations exist
  std::string nodeName(pNode->m_name);
  Mtx4 nodeTransform = pNode->m_transformation;
  const NodeAnimation* pNodeAnim = FindNodeAnim(aData, nodeName);
  //if node has animation
  if (pNodeAnim)
  {
    //interpolate scale and generate current scale matrix
    Vec3 scale = ComputeInterpolatedScale(animTime, *pNodeAnim);
    Mtx4 scaleMtx = Mtx4::scale(scale);

    //interpolate rotation and generate current rotation matrix
    Quaternion rotQ = ComputeInterpolatedRotation(animTime, *pNodeAnim);
    Mtx4 rotMtx = rotQ.ToMatrix();

    //interpolate translation and generate current translation matrix
    Vec3 position = ComputeInterpolatedPosition(animTime, *pNodeAnim);
    Mtx4 transMtx = Mtx4::translate(position);

    //combine to form TRS matrix
    nodeTransform = transMtx * rotMtx * scaleMtx;
  }

  //concatenate with parent transformation
  Mtx4 toParent = parentTransform * nodeTransform;

  //if bone exists in skeleton, set the final transformation:
  //final transformation brings the vertex from local space to bone space (BoneOffsetMtx)
  //and up the hiearchy towards the root (mesh space) and finally to world space (m_globalInverseTransform)
  BoneMapping::const_iterator it = m_boneMapping.find(nodeName);
  if (it != m_boneMapping.end())
  {
    unsigned index = it->second;
    Mtx4 toRoot = m_globalInverseTransform * toParent * m_boneOffsetMatrices[index];

    //solves the mesh becoming too big when animating
    Vec3 lengths = toRoot.getLengthOfCoordinateVectorsInMatrix();
    Mtx4 normalizedScale = Mtx4::scale(1.f / lengths.x, 1.f / lengths.y, 1.f / lengths.z);
    Mtx4 tempMtx = m_translateToCenter * normalizedScale * toRoot;
    finalTransforms[index] = tempMtx;
  }

  //recursively process all children
  size_t numChilds = pNode->m_children.size();
  for (size_t i = 0; i < numChilds; ++i)
  {
    ComputeFinalBoneTransform(animTime, aData, pNode->m_children[i], toParent, finalTransforms);
  }
}

Quaternion Skeleton::ComputeInterpolatedRotation(float animTime, const NodeAnimation & nodeAnim) const
{
  //if only one value, return it
  size_t count = nodeAnim.m_RotationKeys.size();
  if (count == 1)
    return nodeAnim.m_RotationKeys[0].value;

  //use two values to interpolate
  unsigned curr = FindRotationKey(animTime, nodeAnim);
  unsigned next = curr + 1;
  assert(next < count);
  double start = nodeAnim.m_RotationKeys[curr].time;
  double end = nodeAnim.m_RotationKeys[next].time;
  float delta = static_cast<float>(end - start);

  //how far from the starting keyframe are we?
  float s = (animTime - static_cast<float>(start)) / delta;
  Assert("ComputeInterpolatedRotation: Interpolant s is not between 0 and 1: " + std::to_string(s), s >= 0.f && s <= 1.f, ErrorLevel::Warning);

  Quaternion startQ = nodeAnim.m_RotationKeys[curr].value;
  Quaternion endQ = nodeAnim.m_RotationKeys[next].value;

  //use spherical interpolation for quaternions
  return Quaternion::Slerp(startQ, endQ, s);
}

Vec3 Skeleton::ComputeInterpolatedPosition(float animTime, const NodeAnimation & nodeAnim) const
{
  //if only one value, return it
  size_t count = nodeAnim.m_PositionKeys.size();
  if (count == 1)
  {
    return nodeAnim.m_PositionKeys[0].value;
  }

  //use two values to interpolate
  unsigned curr = FindPositionKey(animTime, nodeAnim);
  unsigned next = curr + 1;
  assert(next < count);
  double start = nodeAnim.m_PositionKeys[curr].time;
  double end = nodeAnim.m_PositionKeys[next].time;
  float delta = static_cast<float>(end - start);

  //how far from the starting keyframe are we?
  float t = (animTime - static_cast<float>(start)) / delta;
  Assert("ComputeInterpolatedPosition: Interpolant t is not between 0 and 1: " + std::to_string(t), t >= 0.f && t <= 1.f, ErrorLevel::Warning);
  Vec3 startPos = nodeAnim.m_PositionKeys[curr].value;
  Vec3 endPos = nodeAnim.m_PositionKeys[next].value;

  return Lerp(startPos, endPos, t);
}

Vec3 Skeleton::ComputeInterpolatedScale(float animTime, const NodeAnimation & nodeAnim) const
{
  //if only one value, return it
  size_t count = nodeAnim.m_ScalingKeys.size();
  if (count == 1)
  {
    return nodeAnim.m_ScalingKeys[0].value;
  }

  //use two values to interpolate
  unsigned curr = FindScalingKey(animTime, nodeAnim);
  unsigned next = curr + 1;
  assert(next < count);
  double start = nodeAnim.m_ScalingKeys[curr].time;
  double end = nodeAnim.m_ScalingKeys[next].time;
  float delta = static_cast<float>(end - start);

  //how far from the starting keyframe are we?
  float t = (animTime - static_cast<float>(start)) / delta;
  Assert("ComputeInterpolatedScale: Interpolant t is not between 0 and 1: " + std::to_string(t), t >= 0.f && t <= 1.f, ErrorLevel::Warning);

  Vec3 startScale = nodeAnim.m_ScalingKeys[curr].value;
  Vec3 endScale = nodeAnim.m_ScalingKeys[next].value;
  return Lerp(startScale, endScale, t);
}

Mtx4 Skeleton::BlendTransforms(const NodeAnimation & currAnim, const NodeAnimation & nextAnim, float blendFactor) const
{
  Mtx4 result;

  //there should be at least one value
  size_t last_key = currAnim.m_ScalingKeys.size() - 1;
  size_t first_key = 0;

  //blending factor: how far from the last keyframe of the current animation are we?

  //blend scale
  Vec3 startScale = currAnim.m_ScalingKeys[last_key].value;
  Vec3 endScale = nextAnim.m_ScalingKeys[first_key].value;
  Mtx4 scaleMtx = Mtx4::scale(Lerp(startScale, endScale, blendFactor) );

  //blend rotation
  last_key = currAnim.m_RotationKeys.size() - 1;
  Quaternion startQ = currAnim.m_RotationKeys[last_key].value;
  Quaternion endQ = nextAnim.m_RotationKeys[first_key].value;
  Quaternion blendQ = Quaternion::Slerp(startQ, endQ, blendFactor);
  Mtx4 rotMtx = blendQ.ToMatrix();

  //blend position
  last_key = currAnim.m_PositionKeys.size() - 1;
  Vec3 startPos = currAnim.m_PositionKeys[last_key].value;
  Vec3 endPos = nextAnim.m_PositionKeys[first_key].value;
  Mtx4 transMtx = Mtx4::translate(Lerp(startPos, endPos, blendFactor));

  return transMtx * rotMtx * scaleMtx;
}


void Skeleton::ComputeBlendingBoneTransforms(float blendFactor, const AnimationData & current, const AnimationData & next, 
                                             const ANode * pNode, const Mtx4 & parentTransform, std::vector<Mtx4>& finalTransforms) const
{
  //check if animations exist
  std::string nodeName(pNode->m_name);
  Mtx4 nodeTransform = pNode->m_transformation;
  const NodeAnimation* currAnim = FindNodeAnim(current, nodeName);
  const NodeAnimation* nextAnim = FindNodeAnim(next, nodeName);
  
  //if this bone has animation in current and next animation, blend
  if (currAnim && nextAnim)
  {
    nodeTransform = BlendTransforms(*currAnim, *nextAnim, blendFactor);
  }

  //concatenate with parent transformation
  Mtx4 toParent = parentTransform * nodeTransform;

  //if bone exists in skeleton, set the final transformation:
  //final transformation brings the vertex from local space to bone space (BoneOffsetMtx)
  //and up the hiearchy towards the root (mesh space) and finally to world space (m_globalInverseTransform)
  BoneMapping::const_iterator it = m_boneMapping.find(nodeName);
  if (it != m_boneMapping.end())
  {
    unsigned index = it->second;
    Mtx4 toRoot = m_globalInverseTransform * toParent * m_boneOffsetMatrices[index];

    //solves the mesh becoming too big when animating
    Vec3 lengths = toRoot.getLengthOfCoordinateVectorsInMatrix();
    Mtx4 normalizedScale = Mtx4::scale(1.f / lengths.x, 1.f / lengths.y, 1.f / lengths.z);
    Mtx4 tempMtx = m_translateToCenter * normalizedScale * toRoot;
    finalTransforms[index] = tempMtx;
  }

  //recursively process all children
  size_t numChilds = pNode->m_children.size();
  for (size_t i = 0; i < numChilds; ++i)
  {
    ComputeBlendingBoneTransforms(blendFactor, current, next, pNode->m_children[i], toParent, finalTransforms);
  }
}

const NodeAnimation* Skeleton::FindNodeAnim(const AnimationData& animData, const std::string & NodeName) const
{
  const NodeAnimation* ret = nullptr;

  //find animation with the name given
  size_t size = animData.m_nodeAnimations.size();
  for (size_t i = 0; i < size; ++i)
  {
    if (animData.m_nodeAnimations[i].m_nodeName == NodeName)
    {
      ret = &animData.m_nodeAnimations[i];
    }
  }
  return ret;
}

unsigned Skeleton::FindRotationKey(float animTime, const NodeAnimation & nodeAnim) const
{
  size_t n = nodeAnim.m_RotationKeys.size();
  assert(n > 0);
  //find the keyframe with the quaternion that is just before the animation time
  for (unsigned i = 0; i < n-1; ++i)
  {
    if (animTime < static_cast<float>(nodeAnim.m_RotationKeys[i + 1].time))
      return i;
  }

  //if reaches here, means animTime is wrong
  Assert("Did not find a rotation key for this animation time: " + std::to_string(animTime), false, ErrorLevel::Warning);
  return 0;
}

unsigned Skeleton::FindScalingKey(float animTime, const NodeAnimation & nodeAnim) const
{
  size_t n = nodeAnim.m_ScalingKeys.size();
  assert(n > 0);
  for (unsigned i = 0; i < n-1; ++i)
  {
    if (animTime < static_cast<float>(nodeAnim.m_ScalingKeys[i + 1].time))
      return i;
  }

  //if reaches here, means animTime is wrong
  Assert("Did not find a scaling key for this animation time: " + std::to_string(animTime), false, ErrorLevel::Warning);
  return 0;
}

unsigned Skeleton::FindPositionKey(float animTime, const NodeAnimation & nodeAnim) const
{
  size_t n = nodeAnim.m_PositionKeys.size();
  assert(n > 0);
  for (unsigned i = 0; i < n-1; ++i)
  {
    if (animTime < static_cast<float>(nodeAnim.m_PositionKeys[i + 1].time))
      return i;
  }

  //if reaches here, means animTime is wrong
  Assert("Did not find a position key for this animation time: " + std::to_string(animTime), false, ErrorLevel::Warning);
  return 0;
}
