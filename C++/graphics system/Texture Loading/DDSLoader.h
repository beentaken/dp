/******************************************************************************/
/*!
\file		DDSLoader.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef DDS_LOADER_H
#define DDS_LOADER_H
#include "Graphics\ogl.h"

struct D3D_PixelFormat // DDPIXELFORMAT
{
  int dwSize;
  int dwFlags;
  int dwFourCC;
  int dwRGBBitCount;
  int dwRBitMask, dwGBitMask, dwBBitMask;
  int dwRGBAlphaBitMask;
};

struct D3D_Caps2
{
  int dwCaps1;
  int dwCaps2;
  int Reserved[2];
};


struct D3D_SurfaceDesc2
{
  int dwSize;
  int dwFlags;
  int dwHeight;
  int dwWidth;
  int dwPitchOrLinearSize;
  int dwDepth;
  int dwMipMapCount;
  int dwReserved1[11];
  D3D_PixelFormat ddpfPixelFormat;
  D3D_Caps2 ddsCaps;
  int dwReserved2;
};


unsigned char* LoadDDS(const std::string& filepath, int* width, int* height, int* format, int* size);  
GLuint LoadCubeMapDDS(const std::vector<std::string>& faces);
GLuint LoadDDSTexture(const std::string& name);
GLuint LoadDDSTexture2(const std::string& filename);

#endif

