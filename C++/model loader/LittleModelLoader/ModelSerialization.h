#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "Math\OMath.h"
#include "AnimationClasses.h"
#include <vector>
#include <sstream>
#include <limits>

#pragma region print specializations
template<typename T>
void PrintStartTag(std::ostream& out);

template <>
void PrintStartTag<Vec2>(std::ostream& out) { out << "Vector2: " << std::endl;}
template <>
void PrintStartTag<Vec3>(std::ostream& out) { out << "Vector3: " << std::endl;}
template <>
void PrintStartTag<AFace>(std::ostream& out) { out << "Faces: " << std::endl;}
template <>
void PrintStartTag<ABone>(std::ostream& out) { out << "Bones: " << std::endl; }
template <>
void PrintStartTag<AMesh>(std::ostream& out) { out << "Meshes: " << std::endl; }
template <>
void PrintStartTag<VectorKey>(std::ostream& out) { out << "VectorKeys: " << std::endl; }
template <>
void PrintStartTag<QuatKey>(std::ostream& out) { out << "QuatKeys: " << std::endl; }
template <>
void PrintStartTag<AMaterial>(std::ostream& out) { out << "Materials: " << std::endl; }
template <>
void PrintStartTag<NodeAnimation>(std::ostream& out) { out << "NodeAnimations: " << std::endl; }
template <>
void PrintStartTag<AnimationData>(std::ostream& out) { out << "Animations: " << std::endl; }

#pragma endregion

#pragma region mesh_sub_structures

inline void Serialize(const Vec2& v, std::ostream& out)
{
  out << "{" << v.x << " " << v.y << "}" << std::endl;
}

inline void Serialize(const Vec3& v, std::ostream& out)
{
  out << "{" << v.x << " " << v.y << " " << v.z << "}" << std::endl;
}

inline void Serialize(const AFace& face, std::ostream& out)
{
  out << "{" << face.index1 << " " << face.index2 << " " << face.index3 << "}" << std::endl;
}

inline void Serialize(const Mtx4& mtx, std::ostream& out)
{
  out << "{";
  for (int i = 0; i < 15; ++i)
  {
    out << mtx.data[i] << " ";
  }
  out << mtx.data[15] << "}" << std::endl;
}

inline void Serialize(const ABone& bone, std::ostream& out)
{
  out << "Bone: " << bone.bone_name << " ";
  size_t n = bone.m_boneWeights.size();
  out << "<" << n << ">" << " ";
  for (size_t i = 0; i < n; ++i)
  {
    const ABone::VertexWeight& w = bone.m_boneWeights[i];
    out << "[" << w.first << " " << w.second << "]";
    if (i != n - 1)
      out << ",";
    else
      out << " ";
  }
  Serialize(bone.boneOffsetMtx, out);
}

#pragma endregion


#pragma region animation_sub_structures

inline void Serialize(const VectorKey& v, std::ostream& out)
{
  out << "[" << v.time << ", ";
  out << "{" << v.value.x << " " << v.value.y << " " << v.value.z << "}";
  out << "]" << std::endl;
}

inline void Serialize(const Quaternion& q, std::ostream& out)
{
  out << "{" << q.w << " " << q.x << " " << q.y << " " << q.z << "}";
}

inline void Serialize(const QuatKey& v, std::ostream& out)
{
  out << "[" << v.time << ", ";
  Serialize(v.value, out);
  out << "]" << std::endl;
}

std::string TrimFilepath(std::string const & path)
{
  auto pos = path.find_last_of("\\");
  return path.substr(pos, std::string::npos);
}

inline void Serialize(const AMaterial& material, std::ostream& out)
{
  //if count = 0, print "none"

  out << "{" << material.diffuseMapCount << " " << material.specularMapCount
    << " " << material.normalMapCount << " ";

  std::string none = "none";
  if (material.diffuseMapCount == 0)
    out << none;
  else
    out << TrimFilepath(material.diffusemap_file);

  out << " ";

  if (material.specularMapCount == 0)
    out << none;
  else
    out << TrimFilepath(material.specularmap_file);

  out << " ";

  if (material.normalMapCount == 0)
    out << none;
  else
    out << TrimFilepath(material.normalmap_file);

  out << " }" << std::endl;
}

inline void Serialize(const ANode& anode, std::ostream& out)
{
  out << std::setprecision(15);
  out << "{" << std::endl;
  
  // serialize name
  out << anode.m_name << std::endl;
  
  // serialize matrix
  Serialize(anode.m_transformation, out);

  //serialize mesh indices
  size_t n = anode.m_meshIndices.size();
  out << "<" << n << ">" << std::endl;
  out << "[";
  for (size_t i = 0; i < n; ++i)
  {
    out << anode.m_meshIndices[i] << " ";
  }
  out << "]" << std::endl;

  //serialize children
  out << "Children: " << "<" << anode.m_children.size() << ">" << std::endl;
  for (auto & elem : anode.m_children)
  {
    if(elem)
      Serialize(*elem, out);
  }
  out << "}" << std::endl;
}
#pragma endregion


template <typename T>
inline void SerializeVector(const std::vector<T>& cont, std::ostream& out)
{
  PrintStartTag<T>(out);
  size_t size = cont.size();
  out << "<" << size << ">" << std::endl;
  for (unsigned i = 0; i < size; ++i)
  {
    Serialize(cont[i], out);
  }
}

inline void Serialize(const AMesh& mesh, std::ostream& out)
{
  out << std::setprecision(15);
  out << "Mesh name: " << mesh.m_meshName << std::endl;
  out << "Material index: " << mesh.m_materialIndex << std::endl;
  out << "Vertices" << std::endl;
  SerializeVector(mesh.m_vertices, out);
  out << "Normals" << std::endl;
  SerializeVector(mesh.m_normals, out);
  out << "TexCoords" << std::endl;
  SerializeVector(mesh.m_textureCoords, out);
  SerializeVector(mesh.m_faces, out);
  SerializeVector(mesh.m_bones, out);
}

inline void Serialize(const NodeAnimation& anim, std::ostream& out)
{
  out << "NodeAnim name: " << anim.m_nodeName << " ";
  out << anim.m_PreState << " " << anim.m_PostState << std::endl;
  SerializeVector(anim.m_PositionKeys, out);
  SerializeVector(anim.m_ScalingKeys, out);
  SerializeVector(anim.m_RotationKeys, out);
}

inline void Serialize(const AnimationData& anim, std::ostream& out)
{
  out << std::setprecision(15);
  out << "Animation name: " << anim.m_animationName << " ";
  out << anim.m_duration << " " << anim.m_TicksPerSecond << std::endl;
  SerializeVector(anim.m_nodeAnimations, out);
}



#endif
