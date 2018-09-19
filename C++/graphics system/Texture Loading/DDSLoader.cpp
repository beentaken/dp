/******************************************************************************/
/*!
\file		DDSLoader.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "DDSLoader.h"

//DDS defines
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

unsigned char* LoadDDS(const std::string& filepath, int* width, int* height, int* format, int* size)
{
  unsigned char header[124];
  unsigned char * buffer;

  unsigned int bufsize, linearSize, mipMapCount, fourCC, components;

  FILE *fp;

  //open the file
  fopen_s(&fp, filepath.data(), "rb");
  if (fp == NULL)
    return 0;

  //check if it is a dds file
  char filecode[4];
  fread(filecode, 1, 4, fp);
#ifdef _PREFAST_
  filecode[3] = '\0';
#endif
  if (strncmp(filecode, "DDS ", 4) != 0) {
    fclose(fp);
    return 0;
  }

  //get the header info
  fread(&header, 124, 1, fp);

  *height = *(unsigned int*)&(header[8]);
  *width = *(unsigned int*)&(header[12]);
  linearSize = *(unsigned int*)&(header[16]);
  mipMapCount = *(unsigned int*)&(header[24]);
  fourCC = *(unsigned int*)&(header[80]);

  //size including mimaps
  bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
  buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
#ifdef _PREFAST_
  Assert("Malloc Failed", buffer);
  SecureZeroMemory(buffer, bufsize * sizeof(unsigned char));
#endif
  fread(buffer, 1, bufsize, fp);
  //close the file
  fclose(fp);

  components = (fourCC == FOURCC_DXT1) ? 3 : 4;
  switch (fourCC)
  {
  case FOURCC_DXT1:
    *format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    break;
  case FOURCC_DXT3:
    *format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    break;
  case FOURCC_DXT5:
    *format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    break;
  default:
    free(buffer);
	return nullptr;
  }

  unsigned int blockSize = (*format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
  *size = ((*width + 3) / 4)*((*height + 3) / 4)*blockSize;

  return buffer;
}

GLuint LoadCubeMapDDS(const std::vector<std::string>& faces)
{
  Assert("Skybox::LoadCubeMap error: Cubemap texture list must have 6 faces", faces.size() == 6, ErrorLevel::SystemCrash);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  int width, height, format, size;
  for (unsigned int i = 0; i < faces.size(); i++)
  {

#ifndef TEST
    unsigned char* data = LoadDDS(faces[i].c_str(), &width, &height, &format, &size);
    if (data)
    {

      glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, size, data);
      free(data);
    }
    else
    {
      std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
      free(data);

    }
#else
    //new dds loading here
#endif
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

GLuint LoadDDSTexture(const std::string& filepath)
{
  unsigned char header[124];
  unsigned char * buffer;

  unsigned int bufsize, height, width, linearSize, mipMapCount, fourCC, components, format;

  FILE *fp;

  //open the file
  fopen_s(&fp, filepath.data(), "rb");
  if (fp == NULL)
    return 0;

  //check if it is a dds file
  char filecode[4];
  fread(filecode, 1, 4, fp);
#ifdef _PREFAST_
  filecode[3] = '\0';
#endif
  if (strncmp(filecode, "DDS ", 4) != 0) {
    fclose(fp);
    return 0;
  }

  //get the header info
  fread(&header, 124, 1, fp);

  height = *(unsigned int*)&(header[8]);
  width = *(unsigned int*)&(header[12]);
  linearSize = *(unsigned int*)&(header[16]);
  mipMapCount = *(unsigned int*)&(header[24]);
  fourCC = *(unsigned int*)&(header[80]);

  //size including mimaps
  bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
  buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
  Assert("Malloc Failed", buffer);
  fread(buffer, 1, bufsize, fp);
  
  //close the file
  fclose(fp);

  //rgba color space, gamma corrected in shader
  components = (fourCC == FOURCC_DXT1) ? 3 : 4;
  switch (fourCC)
  {
  case FOURCC_DXT1:
    format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    break;
  case FOURCC_DXT3:
    format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    break;
  case FOURCC_DXT5:
    format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    break;
  default:
    free(buffer);
    return false;
  }

  GLuint texture;
  glGenTextures(1, &texture);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, texture);

  unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
  unsigned int offset = 0;

  //if mipmaps exist
  if (mipMapCount > 0)
  {
    //load the mipmaps
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
      unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
      glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

      offset += size;
      width = std::max(width / 2, (unsigned int)1);
      height = std::max(height / 2, (unsigned int)1);
    }
    //std::cout << "has mipmaps" << std::endl;
  }
  else  //lots of items with no mipmaps
  {
    //if no mipmaps
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, bufsize, buffer + offset);
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  // Generate mipmaps for the texture.
  glGenerateMipmap(GL_TEXTURE_2D);

  //free memory
  free(buffer);

  return texture;
}
