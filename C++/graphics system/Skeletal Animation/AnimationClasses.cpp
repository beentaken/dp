/******************************************************************************/
/*!
\file		AnimationClasses.cpp
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "AnimationClasses.h"

void FreeNode(ANode * node)
{
  if (!node) return;

  for (auto& elem : node->m_children)
  {
    FreeNode(elem);
  }

  delete node;
}
