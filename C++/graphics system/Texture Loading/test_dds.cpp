/******************************************************************************/
/*!
\file		test_dds.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "test_dds.h"
#include "Graphics\ogl.h"

// Loading images
//#define LTRACE_LOAD

// Magic
#define MAGIC_DDS 0x20534444      // DDS<space>

// DXT<n> id's
#define ID_DXT1   0x31545844
#define ID_DXT3   0x33545844
#define ID_DXT5   0x35545844

//
// C/dtor
//
DDDS::DDDS()
{
  int i;
  for (i = 0; i<MAX_MIPMAP_LEVEL; i++)
  {
    pixels[i] = 0;
    bytes[i] = 0;
  }

  mipmapLevels = 0;
  wid = hgt = 0;
  format = 0;
  compressionType = 0;
  internalFormat = 0;
  blockSize = 0;
}
DDDS::~DDDS()
{
  int i;

  for (i = 0; i<MAX_MIPMAP_LEVEL; i++)
    QFREE(pixels[i]);
}

static void ConvertARGB2RGBA(unsigned char *a, int n)
//static void ConvertBGRA2RGBA(char *a,int n)
{
  // In hex dump: BGRA -> RGBA
  int i;
  unsigned char t;
  n /= 4;
  for (i = 0; i<n; i++)
  {
    t = a[2];
    a[2] = a[0];
    a[0] = t;
    a += 4;
  }
}

static void FlipDXT1BlockFull(unsigned char *data)
// A DXT1 block layout is:
// [0-1] color0.
// [2-3] color1.
// [4-7] color bitmap, 2 bits per pixel.
// So each of the 4-7 bytes represents one line, flipping a block is just
// flipping those bytes.
// Note that http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
// contains an error in the last line: data[6]=data[5] is a bug!
{
  //return;
  unsigned char tmp;

  tmp = data[4];
  data[4] = data[7];
  data[7] = tmp;

  tmp = data[5];
  data[5] = data[6];
  data[6] = tmp;

  //data[4]=data[5]=data[6]=data[7]=0;
}

static void FlipDXT3BlockFull(unsigned char *block)
// Flips a full DXT3 block in the y direction.
{
  // A DXT3 block layout is:
  // [0-7]  alpha bitmap, 4 bits per pixel.
  // [8-15] a DXT1 block.

  // We can flip the alpha bits at the byte level (2 bytes per line).
  unsigned char tmp = block[0];
  block[0] = block[6];
  block[6] = tmp;
  tmp = block[1];
  block[1] = block[7];
  block[7] = tmp;
  tmp = block[2];
  block[2] = block[4];
  block[4] = tmp;
  tmp = block[3];
  block[3] = block[5];
  block[5] = tmp;

  // And flip the DXT1 block using the above function.
  FlipDXT1BlockFull(block + 8);
}

static void FlipDXT5BlockFull(unsigned char *block)
// From http://src.chromium.org/viewvc/chrome/trunk/src/o3d/core/cross/bitmap_dds.cc?view=markup&pathrev=21227
// Original source contained bugs; fixed here.
{
  // A DXT5 block layout is:
  // [0]    alpha0.
  // [1]    alpha1.
  // [2-7]  alpha bitmap, 3 bits per pixel.
  // [8-15] a DXT1 block.

  // The alpha bitmap doesn't easily map lines to bytes, so we have to
  // interpret it correctly.  Extracted from
  // http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt :
  //
  //   The 6 "bits" bytes of the block are decoded into one 48-bit integer:
  //
  //     bits = bits_0 + 256 * (bits_1 + 256 * (bits_2 + 256 * (bits_3 +
  //                   256 * (bits_4 + 256 * bits_5))))
  //
  //   bits is a 48-bit unsigned integer, from which a three-bit control code
  //   is extracted for a texel at location (x,y) in the block using:
  //
  //       code(x,y) = bits[3*(4*y+x)+1..3*(4*y+x)+0]
  //
  //   where bit 47 is the most significant and bit 0 is the least
  //   significant bit.
  //QByteDump(block+2,6);

  // From Chromium (source was buggy)
  unsigned int line_0_1 = block[2] + 256 * (block[3] + 256 * block[4]);
  unsigned int line_2_3 = block[5] + 256 * (block[6] + 256 * block[7]);
  // swap lines 0 and 1 in line_0_1.
  unsigned int line_1_0 = ((line_0_1 & 0x000fff) << 12) |
    ((line_0_1 & 0xfff000) >> 12);
  // swap lines 2 and 3 in line_2_3.
  unsigned int line_3_2 = ((line_2_3 & 0x000fff) << 12) |
    ((line_2_3 & 0xfff000) >> 12);
  block[2] = line_3_2 & 0xff;
  block[3] = (line_3_2 & 0xff00) >> 8;
  block[4] = (line_3_2 & 0xff0000) >> 16;
  block[5] = line_1_0 & 0xff;
  block[6] = (line_1_0 & 0xff00) >> 8;
  block[7] = (line_1_0 & 0xff0000) >> 16;

  //QByteDump(block+2,6);

  // And flip the DXT1 block using the above function.
  FlipDXT1BlockFull(block + 8);
}

#ifdef OBS
static void FlipDXT35BlockOLD(char *data)
// Flip one 4x4 block for DXT3/5
{
  char tmp;

  tmp = data[4];
  data[4] = data[7];
  data[7] = tmp;

  tmp = data[5];
  data[5] = data[6];
  data[6] = tmp;

  // 2nd row
  tmp = data[12];
  data[12] = data[15];
  data[15] = tmp;

  tmp = data[13];
  data[13] = data[14];
  data[14] = tmp;

#ifdef OBS
  // Original 1-9-09
  char tmp;

  tmp = data[4];
  data[4] = data[7];
  data[7] = tmp;

  tmp = data[5];
  data[5] = data[6];
  data[6] = tmp;

  // 2nd row
  tmp = data[12];
  data[12] = data[15];
  data[15] = tmp;

  tmp = data[13];
  data[13] = data[14];
  data[14] = tmp;
#endif
}
#endif

bool DDDS::Load(cstring fname)
{
#ifdef LTRACE_LOAD
  qdbg("DDDS:Load(%s)\n", fname);
#endif

  int magic;
  int i;
  unsigned char *temp = 0;

  QFile *f = new QFile(fname);

  f->Read(&magic, sizeof(magic));
  if (magic != MAGIC_DDS) goto fail;
  else
  {
    // Direct3D 9 format
    D3D_SurfaceDesc2 header;
    f->Read(&header, sizeof(header));
    //qdbg("pixelfmt: rgb bit count %d\n",header.ddpfPixelFormat.dwRGBBitCount);

    // Remember info for users of this object
    wid = header.dwWidth;
    hgt = header.dwHeight;
    mipmapLevels = header.dwMipMapCount;

    int nBytes;

    // Number of pixels
    nBytes = header.dwHeight*header.dwWidth;
    // Block size default
    blockSize = 16;

    // DXT5?
    if (header.ddpfPixelFormat.dwFlags&DDPF_FOURCC)
    {
      // Compressed
      format = DDS_FORMAT_YCOCG;      // ???????? Not right
      unsigned int fourCC;
      fourCC = header.ddpfPixelFormat.dwFourCC;
      if (fourCC == ID_DXT1)
      {
        blockSize = 8;
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        //internalFormat=GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
      }
      else if (fourCC == ID_DXT3)
      {
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        //internalFormat=GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
      }
      else if (fourCC == ID_DXT5)
      {
        // DXT5
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        //internalFormat=GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
      }
      else
      {
        char buf[5];
        buf[0] = fourCC & 255;
        buf[1] = (fourCC >> 8) & 255;
        buf[2] = (fourCC >> 16) & 255;
        buf[3] = (fourCC >> 24) & 255;
        buf[4] = 0;
        //qwarn("DDDS:Load(%s); unknown compressed format (%s)", fname, buf);
        goto fail;
      }
      // DXT5 compression
      //bytes/=4;
    }
    else if (header.ddpfPixelFormat.dwRGBBitCount == 32)
    {
      format = DDS_FORMAT_RGBA8;
      // Calculate bytes for highest mipmap level
      //bytes=header.dwHeight*header.dwWidth*header.ddpfPixelFormat.dwRGBBitCount/8;
      nBytes = nBytes*header.ddpfPixelFormat.dwRGBBitCount / 8;
    }
    else //if(header.ddpfPixelFormat.dwRGBBitCount!=32)
    {
      //qwarn("DDDS:Load(%s); unknown DDS format (rgb bitcount not 32, not DXT5)", fname);
      goto fail;
    }

    // Read all mipmap levels
    int w, h;
    w = wid;
    h = hgt;
    for (i = 0; i<header.dwMipMapCount&&i<MAX_MIPMAP_LEVEL; i++)
    {
      // Deduce # of bytes
      // Close to the higher mipmap levels, wid or hgt may become 0; keep things at 1
      if (w == 0)w = 1;
      if (h == 0)h = 1;

      if (format == DDS_FORMAT_RGBA8)
      {
        // RGBA8
        nBytes = w*h * 4;
      }
      else
      {
        // DXTx
        nBytes = ((w + 3) / 4)*((h + 3) / 4)*blockSize;
        if (i == 0)
        {
          // Create temp buffer to flip DDS
          temp = (unsigned char*)qalloc(nBytes);
        }
      }

      bytes[i] = nBytes;
      pixels[i] = (unsigned char*)qalloc(nBytes);
      if (!pixels[i])
      {
        qerr("DDDS:Load(%s); out of memory for mipmap level %d", fname, i);
        goto fail;
      }

      if (format != DDS_FORMAT_RGBA8)
      {
        // First read in temp buffer
        f->Read(temp, nBytes);
        // Flip & copy to actual pixel buffer
        int j, widBytes, k;
        unsigned char *s, *d;
        widBytes = ((w + 3) / 4)*blockSize;
        s = temp;
        d = pixels[i] + ((h + 3) / 4 - 1)*widBytes;
        //int count=0;
        for (j = 0; j<(h + 3) / 4; j++)
        {
          memcpy(d, s, widBytes);
          if (blockSize == 8)
          {
            for (k = 0; k<widBytes / blockSize; k++)
              FlipDXT1BlockFull(d + k*blockSize);
          }
          else
          {
            // DXT3, DXT5
            if (compressionType == DDS_COMPRESS_BC2)
            {
              // DXT3
              for (k = 0; k<widBytes / blockSize; k++)
                FlipDXT3BlockFull((unsigned char*)d + k*blockSize);
            }
            else
            {
              // DXT5
              for (k = 0; k<widBytes / blockSize; k++)
                FlipDXT5BlockFull((unsigned char*)d + k*blockSize);
            }
          }
          s += widBytes;
          d -= widBytes;
          //count+=widBytes;
          //qdbg("j=%d/%d - count=%d (total %d)\n",j,h,count,nBytes); QWait(1);
        }
        //      count=123;
        //f->Read(pixels[i],nBytes);
      }
      else
      {
        // RGBA8
        if (format == DDS_FORMAT_RGBA8)
        {
          ConvertARGB2RGBA(pixels[i], nBytes);
        }
      }
      // Next level is smaller
      w /= 2;
      h /= 2;
    }
    // Release temp buffer
    QFREE(temp);
  }

  QDELETE(f);
  return true;
fail:
  QDELETE(f);
  return false;
}

DTexture *DDDS::CreateTexture()
// Create a texture from this dds
{
  DTexture *t;
  int i, w, h;

  t = new DTexture();
  w = wid;
  h = hgt;
  t->SetSize(w, h);
  t->EnableMipmap(true);
  t->DisableCompression();
  t->SetInternalFormat(DTexture::IF_RGBA);
  t->CreateTexture();
  // Fill mipmaps
  t->Select();
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  for (i = 0; i<mipmapLevels; i++)
  {
    // Keep size at a minimum
    if (w == 0)w = 1;
    if (h == 0)h = 1;

    if (pixels[i])
    {
#ifdef ND
      // FAKE RED
      static unsigned char *myPixels;
      if (!myPixels)
      {
        int j;

        myPixels = (unsigned char*)malloc(1024 * 1024);
      }
      unsigned char *d;
      d = myPixels;
      int l = (i % 4);
      for (int j = 0; j<1024 * 1024 / 4; j++)
      {
        if (l == 0) { *d++ = 255; *d++ = 0; *d++ = 0; *d++ = 255; }
        else if (l == 1) { *d++ = 0; *d++ = 255; *d++ = 0; *d++ = 255; }
        else if (l == 2) { *d++ = 0; *d++ = 0; *d++ = 255; *d++ = 255; }
        else if (l == 3) { *d++ = 255; *d++ = 255; *d++ = 0; *d++ = 255; }
      }
      glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, myPixels);
      goto skip_it;
#endif

      if (format == DDS_FORMAT_RGBA8)
      {
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels[i]);
      }
      else
      {
        // Compressed formats
        if (format == DDS_FORMAT_YCOCG)
        {
          // DXT1/3/5
          glCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, w, h, 0, bytes[i], pixels[i]);
        }
        else
        {
          qwarn("DDS:CreateTexture; not DDS_FORMAT_YCOCG for mipmap level %d", i);
        }
        // Verify that compression took place
        QShowGLErrors("DDDS:CreateTexture()");
        GLint param = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_ARB, &param);
        if (param == 0)
        {
          qwarn("DDDS:CreateTexture(); mipmap level %d indicated compression failed", i);
        }
      }
    skip_it:
      GLint param = 0;
      glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_INTERNAL_FORMAT, &param);
      //qdbg("Mipmap %d: internal format 0x%x\n",i,param);

      //gluBuild2DMipmaps(target,internalFormat,wid,hgt,GL_RGBA,GL_UNSIGNED_BYTE,
      //bm->GetBuffer());
    }
    else
    {
      qwarn("DDDS:CreateTexture(); missing pixels for mipmap level %d", i);
    }

    // Next level uses less levels
    w /= 2;
    h /= 2;
  }
  return t;
}