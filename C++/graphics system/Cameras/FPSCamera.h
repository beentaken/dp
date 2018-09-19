/******************************************************************************/
/*!
\file		FPSCamera.h
\project	Little Chef Story
\primary author Chan Yu Hong

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include "CameraBase.h"

class FPSCamera : public CameraBase
{
private:
	void Update();
	virtual void UpdateMovement() override;
  virtual void UpdateView() override;
  virtual void UpdateZoom() override;
};

#endif

