/******************************************************************************/
/*!
\file		EditorCamera.h
\project	Little Chef Story
\primary author Yeo Zhi Xiang

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once
#include "FPSCamera.h"
class EditorCamera :
	public FPSCamera
{
	void PlaneMovement();
	void Zoom();
	void RotateCamera();
	void UpdateVars();
	void InputEnter();
	void InputExit();
	virtual void UpdateMovement() override;
	virtual void  UpdateZoom() override;
	virtual void UpdateView() override;
};

