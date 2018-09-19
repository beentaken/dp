#ifndef LOAD_MODEL_H
#define LOAD_MODEL_H

#include "AnimationClasses.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

static const std::string DIRECTORY = "Models/";

AnimScene LoadModel(const std::string& file)
{
#ifdef TESTING
  std::string path = DIRECTORY + file;
#else
  std::string path = file;
#endif
  AnimScene scene;
  Assimp::Importer importer;
  //importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 8);
  importer.ReadFile(path,
    //post processing options

    /*
    aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
    aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_FixInfacingNormals |
    aiProcess_SplitLargeMeshes | aiProcess_ValidateDataStructure |
    aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
    aiProcess_CalcTangentSpace | aiProcess_GenNormals |
    aiProcess_SortByPType | aiProcess_LimitBoneWeights
    */

    //aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights 
    aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs
  );

  //required for each mesh to have its own scene
  const aiScene* pScene = importer.GetScene();

  //check if scene was loaded
  if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
  {
    std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
    assert(false);
  }

  scene.Init(pScene);

  return scene;
}


void FreeScene(AnimScene scene)
{
  scene.Terminate();
}

#endif
