#ifndef TEST_READ_WRITE_H
#define TEST_READ_WRITE_H

#include <sstream>
#include <fstream>
#include <string>
#include "ModelSerialization.h"
#include "ModelDeserialization.h"
#include "loadmodel.h"


void CreateFileFromScene(const AnimScene& scene, const std::string& outputFile)
{
  std::ofstream outFile(outputFile, std::ios_base::out);

  SerializeVector(scene.GetMeshes(), outFile);
  SerializeVector(scene.GetMaterials(), outFile);

  if (!scene.GetAnimationData().empty())
  {
    SerializeVector(scene.GetAnimationData(), outFile);

    //recusrively serialize animation hiearchy
    const ANode* root = scene.GetRootNode();
    outFile << "Animation_Hierarchy: " << std::endl;
    if (root)
    {
      Serialize(*root, outFile);
    }
  }

  outFile.close();
}

void LittleModelWrite(const AnimScene& scene, const std::string& modelFile)
{
  //create fstream and serialize
  std::string outFileName = modelFile.substr(0, modelFile.find_last_of('.')) + ".lmf";

  CreateFileFromScene(scene, outFileName);
}

AnimScene LittleModelRead(const std::string& lcmFile)
{
  std::ifstream inFile(lcmFile, std::ios_base::in);
  if (!inFile)
    std::cout << "Opening file for input failed" << std::endl;

  std::vector<AMesh> meshes = DeserializeMeshes(inFile);
  std::vector<AMaterial> materials = DeserializeVector<AMaterial>(inFile);

  std::vector<AnimationData> anims;
  ANode* node = nullptr;

  //check if animations exist
  char test = inFile.peek();
  if (test == 'A')
  {
    anims = DeserializeAnimations(inFile);

    std::string check;
    inFile >> check;
    if (check != "Animation_Hierarchy:")
      assert(false);

    node = DeserializeTree(inFile);
  }

  inFile.close();

  return AnimScene(meshes, materials, anims, node);
}




#endif

