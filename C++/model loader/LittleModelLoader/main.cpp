#include <sstream>
#include <fstream>
#include "loadmodel.h"
#include "ModelReadWrite.h"

bool MeshEqual(const AMesh& lhs, const AMesh& rhs)
{
  bool a = lhs.m_bones == rhs.m_bones;
  bool b = lhs.m_faces == rhs.m_faces;
  bool c = lhs.m_materialIndex == rhs.m_materialIndex;
  bool d = lhs.m_meshName == rhs.m_meshName;
  bool e = lhs.m_normals == rhs.m_normals;
  bool f = lhs.m_textureCoords == rhs.m_textureCoords;
  bool g = lhs.m_vertices == rhs.m_vertices;
  bool allequal = a && b && c && d && e && f && g;
  assert(allequal);
  return allequal;
}

bool VectorMeshEqual(const std::vector<AMesh>& lhs, const std::vector<AMesh>& rhs)
{
  size_t n = lhs.size();
  assert(n == rhs.size());

  bool result = true;
  for (size_t i = 0; i < n; ++i)
  {
    result &= MeshEqual(lhs[i], rhs[i]);
  }
  assert(result);
  return result;
}


int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "No file given" << std::endl;
    assert(false);
    return -1;
  }
    
  //load model
  std::string modelFile = std::string(argv[1]);
  AnimScene scene = LoadModel(modelFile);

  LittleModelWrite(scene, modelFile);

//#define TEST_READ
#ifdef TEST_READ
  AnimScene newScene = LittleModelRead("test.lmf");

  CreateFileFromScene(newScene, "result");
#endif

  //free model
  FreeScene(scene);
}

