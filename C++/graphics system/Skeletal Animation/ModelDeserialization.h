/******************************************************************************/
/*!
\file		ModelDeserialization.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/

#ifndef DESERIALIZATION_H
#define DESERIALIZATION_H

#include "Math\OMath.h"
#include "AnimationClasses.h"
#include <sstream>
#include <limits>
#include <cassert>

#define MAX_STR_SIZE 1000000   //1 million

using streamsize = std::numeric_limits<std::streamsize>;

#pragma region type_check specializations
template<typename T>
bool CheckStartTag(const std::string& type);

template<>
bool CheckStartTag<Vec2>(const std::string& type) {return type == "Vector2:";}
template<>
bool CheckStartTag<Vec3>(const std::string& type){ return type == "Vector3:"; }
template<>
bool CheckStartTag<AFace>(const std::string& type){ return type == "Faces:"; }
template<>
bool CheckStartTag<ABone>(const std::string& type) { return type == "Bones:"; }
template<>
bool CheckStartTag<AMesh>(const std::string& type) { return type == "Meshes:"; }
template<>
bool CheckStartTag<VectorKey>(const std::string& type) { return type == "VectorKeys:"; }
template<>
bool CheckStartTag<QuatKey>(const std::string& type) { return type == "QuatKeys:"; }
template<>
bool CheckStartTag<AMaterial>(const std::string& type) { return type == "Materials:"; }
template<>
bool CheckStartTag<NodeAnimation>(const std::string& type) { return type == "NodeAnimations:"; }
template<>
bool CheckStartTag<AnimationData>(const std::string& type) { return type == "Animations:"; }

#pragma endregion

#pragma region mesh_sub_structures

template <typename T>
T Deserialize(std::istream& in);

template<>
Vec2 Deserialize<Vec2>(std::istream& in)
{
  float x, y;
  char temp1, temp2;

  in >> temp1 >> x >> y >> temp2;

  return Vec2(x, y);
}

template<>
Vec3 Deserialize(std::istream& in)
{
  float x, y, z;
  char temp1, temp2;

  in >> temp1 >> x >> y >> z >> temp2;

  return Vec3(x, y, z);
}

template<>
inline AFace Deserialize(std::istream& in)
{
  AFace face;
  char temp1, temp2;
  in >> temp1 >> face.index1 >> face.index2 >> face.index3 >> temp2;
  return face;
}

template<>
inline Mtx4 Deserialize(std::istream& in)
{
  char temp1, temp2;
  in >> temp1;
  Mtx4 result = Mtx4();
  for (int i = 0; i < 15; ++i)
  {
    in >> result.data[i];
  }
  in >> result.data[15] >> temp2;
  return result;
}

template<>
inline ABone Deserialize(std::istream& in)
{
  ABone bone;
  std::string temp;
  in >> temp; 
  if (temp != "Bone:") 
    assert(false);
  in >> bone.bone_name;

  //continue putting in bone weights here
  size_t size = 0;
  //ignore "<" , ">" characters and get size
  in.ignore(streamsize::max(), '<');
  in >> size;
  bone.m_boneWeights.reserve(size);
  in.ignore(streamsize::max(), ' ');

  //read bone weights
  for (size_t i = 0; i < size; ++i)
  {
    ABone::VertexWeight vWeight;
    char temp; // to discard[ ],
    in >> temp >> vWeight.first >> vWeight.second >> temp;
    if (i != size - 1)
      in >> temp;
    bone.m_boneWeights.push_back(vWeight);
  }

  //read bone offset matrix
  char temp1, temp2;
  in >> temp1;
  for (int i = 0; i < 15; ++i)
  {
    in >> bone.boneOffsetMtx.data[i];
  }
  in >> bone.boneOffsetMtx.data[15] >> temp2;

  return bone;
}
#pragma endregion

#pragma region animation_sub_structures

template<>
VectorKey Deserialize<VectorKey>(std::istream& in)
{
  VectorKey vk;
  char discard;
  in >> discard >> vk.time >> discard >> discard;
  in >> vk.value.x >> vk.value.y >> vk.value.z >> discard >> discard;
  return vk;
}

template<>
QuatKey Deserialize<QuatKey>(std::istream& in)
{
  QuatKey qk;
  char discard;
  in >> discard >> qk.time >> discard >> discard;
  in >> qk.value.w >> qk.value.x >> qk.value.y >> qk.value.z >> discard >> discard;
  return qk;
}

//problem here
template<>
AMaterial Deserialize<AMaterial>(std::istream& in)
{
  char discard;
  AMaterial material;
  in >> discard >> material.diffuseMapCount >> material.specularMapCount
    >> material.normalMapCount;

  if (material.diffuseMapCount != 0)
    in >> material.diffusemap_file;

  if (material.specularMapCount != 0)
    in >> material.specularmap_file;

  if (material.normalMapCount != 0)
    in >> material.normalmap_file;

  in >> discard;
  return material;
}

#pragma endregion

template<typename T>
inline std::vector<T> DeserializeVector(std::istream& in)
{
  std::vector<T> result;
  std::string type;
  in >> type;
  if (!CheckStartTag<T>(type))
  {
    std::cout << "Wrong deserialize type!" << std::endl;
    assert(false);
  }

  unsigned size = 0;
  //ignore "<" , ">" characters and get size
  in.ignore(streamsize::max(), '<');
  in >> size;
  result.reserve(size);
  in.ignore(streamsize::max(), '\n');

  char* buffer = new char[MAX_STR_SIZE];
  assert(buffer != nullptr);
  //extract each value and add to vector
  for (unsigned i = 0; i < size; ++i)
  {
    in.getline(buffer, MAX_STR_SIZE);
    std::istringstream input(buffer);
    result.push_back(Deserialize<T>(input));
    memset(buffer, 0, MAX_STR_SIZE);
  }
  delete[] buffer;

  return result;
}

AMesh MeshDeserialize(std::istream& in)
{
  AMesh mesh;
  char tempBuf[1000];
  std::string discard1, discard2;

  //get mesh name
  in.getline(tempBuf, 1000);
  std::stringstream nameinfo(tempBuf);
  nameinfo >> discard1 >> discard2;
  std::string identifier = discard1 + " " + discard2;
  assert(identifier == "Mesh name:");
  nameinfo >> mesh.m_meshName;
  memset(tempBuf, 0, 1000);

  //get material index
  in.getline(tempBuf, 1000);
  std::stringstream indexinfo(tempBuf);
  indexinfo >> discard1 >> discard2;
  identifier = discard1 + " " + discard2;
  assert(identifier == "Material index:");
  indexinfo >> mesh.m_materialIndex;
  memset(tempBuf, 0, 1000);

  //get vertices
  in.getline(tempBuf, 1000);
  std::stringstream verts(tempBuf);
  verts >> identifier;
  assert(identifier == "Vertices");
  mesh.m_vertices = DeserializeVector<Vec3>(in);
  memset(tempBuf, 0, 1000);

  //get normals
  in.getline(tempBuf, 1000);
  std::stringstream norms(tempBuf);
  norms >> identifier;
  assert(identifier == "Normals");
  mesh.m_normals = DeserializeVector<Vec3>(in);
  memset(tempBuf, 0, 1000);

  //get texcoords
  in.getline(tempBuf, 1000);
  std::stringstream uvs(tempBuf);
  uvs >> identifier;
  assert(identifier == "TexCoords");
  mesh.m_textureCoords = DeserializeVector<Vec2>(in);
  memset(tempBuf, 0, 1000);

  //faces and bones
  mesh.m_faces = DeserializeVector<AFace>(in);
  mesh.m_bones = DeserializeVector<ABone>(in);

  return mesh;
}

NodeAnimation NodeAnimDeserialize(std::istream& in)
{
  NodeAnimation anim;
  char tempBuf[1000];
  std::string discard1, discard2;

  //get nodeanim name
  in.getline(tempBuf, 1000);
  std::stringstream header(tempBuf);
  header >> discard1 >> discard2;
  std::string identifier = discard1 + " " + discard2;
  assert(identifier == "NodeAnim name:");
  header >> anim.m_nodeName;
  header >> anim.m_PreState >> anim.m_PostState;

  //get keyframes
  anim.m_PositionKeys = DeserializeVector<VectorKey>(in);
  anim.m_ScalingKeys = DeserializeVector<VectorKey>(in);
  anim.m_RotationKeys = DeserializeVector<QuatKey>(in);

  return anim;
}

std::vector<AMesh> DeserializeMeshes(std::istream& in)
{
  std::vector<AMesh> result;
  std::string type;
  in >> type;
  if (!CheckStartTag<AMesh>(type))
  {
    std::cout << "Wrong deserialize type!" << std::endl;
    assert(false);
  }
  
  size_t numMeshes = 0;
  //ignore "<" , ">" characters and get size
  in.ignore(streamsize::max(), '<');
  in >> numMeshes;
  result.reserve(numMeshes);
  in.ignore(streamsize::max(), '\n');

  for (size_t i = 0; i < numMeshes; ++i)
  {
    result.push_back(MeshDeserialize(in));
  }

  return result;
}

std::vector<NodeAnimation> DeserializeNodeAnims(std::istream& in)
{
  std::vector<NodeAnimation> result;
  std::string type;
  in >> type;
  if (!CheckStartTag<NodeAnimation>(type))
  {
    std::cout << "Wrong deserialize type!" << std::endl;
    assert(false);
  }

  size_t numAnims = 0;
  //ignore "<" , ">" characters and get size
  in.ignore(streamsize::max(), '<');
  in >> numAnims;
  result.reserve(numAnims);
  in.ignore(streamsize::max(), '\n');

  for (size_t i = 0; i < numAnims; ++i)
  {
    result.push_back(NodeAnimDeserialize(in));
  }

  return result;
}

AnimationData AnimationDeserialize(std::istream& in)
{
  AnimationData anim;
  char tempBuf[1000];
  std::string discard1, discard2;

  //get nodeanim name
  in.getline(tempBuf, 1000);
  std::stringstream header(tempBuf);
  header >> discard1 >> discard2;
  std::string identifier = discard1 + " " + discard2;
  assert(identifier == "Animation name:");
  header >> anim.m_animationName;
  header >> anim.m_duration >> anim.m_TicksPerSecond;

  //get animations
  anim.m_nodeAnimations = DeserializeNodeAnims(in);

  return anim;
}

std::vector<AnimationData> DeserializeAnimations(std::istream& in)
{
  std::vector<AnimationData> result;
  std::string type;
  in >> type;
  if (!CheckStartTag<AnimationData>(type))
  {
    std::cout << "Wrong deserialize type!" << std::endl;
    assert(false);
  }

  size_t numAnims = 0;
  //ignore "<" , ">" characters and get size
  in.ignore(streamsize::max(), '<');
  in >> numAnims;
  result.reserve(numAnims);
  in.ignore(streamsize::max(), '\n');

  for (size_t i = 0; i < numAnims; ++i)
  {
    result.push_back(AnimationDeserialize(in));
  }

  return result;
}

ANode* DeserializeTree(std::istream& in)
{
  std::string temp;
  char discard;
  in >> discard;
  ANode* result = new ANode();

  //get name
  in >> result->m_name;

  //get transformation matrix
  result->m_transformation = Deserialize<Mtx4>(in);

  //get mesh indices
  unsigned numindices;
  in.ignore(streamsize::max(), '<');
  in >> numindices;
  result->m_meshIndices.reserve(numindices);
  in.ignore(streamsize::max(), '\n');
  in >> discard;
  for (unsigned i = 0; i < numindices; ++i)
  {
    unsigned value;
    in >> value;
    result->m_meshIndices.push_back(value);
  }
  in >> discard;

  // Read data
  unsigned childsize;
  in.ignore(streamsize::max(), '<');
  in >> childsize;
  result->m_children.reserve(childsize);
  in.ignore(streamsize::max(), '\n');
  // Read children data
  for (unsigned ind = 0; ind < childsize; ++ind)
  {
    ANode* node = DeserializeTree(in);
    result->m_children.push_back(node);
  }
  in >> discard;
  return result;
}

#endif
