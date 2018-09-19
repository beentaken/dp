/******************************************************************************/
/*!
\file		EditorCamera.cpp
\project	Little Chef Story
\primary author Yeo Zhi Xiang

All content \@ 2017-2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "stdafx.h"
#include "EditorCamera.h"
#include "Input\InputManager.h"
#include "Editor\Editor.h"
#include "Physics\CollisionManager.h"
enum class MouseDown
{
	NONE = 0,
	LEFT,
	RIGHT,
	MIDDLE,
};

enum class ArrowDown
{
	NONE = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

static MouseDown mDwn = MouseDown::NONE;
static ArrowDown forwardDwn = ArrowDown::NONE;
static ArrowDown sidesDwn = ArrowDown::NONE;
static bool altHeld;
static float const minZoom = 500.f;
static float const zoomAmount = 100.0f;
static int exponent = 9;
static Vec3 cam_dir, cam_right;
static float dt = 0.0f, multiplier = 0.0f;

static POINT curMousePt, oriMousePt;
static Vec2 oriMousePos, curMousePos;
static Vec3 projWorldUp, projCamDirToXZ;
static Vec3 pivotPos;

void EditorCamera::PlaneMovement()
{
	if (IsCtrlHeld()) return;
	// XZ plane based on cam_dir XZ or world XZ
	{
		switch (forwardDwn)
		{
		case ArrowDown::UP:
			if (mDwn == MouseDown::RIGHT) m_position -= m_zoom * multiplier * dt * cam_dir;
			else m_position -= m_zoom * multiplier * dt * projCamDirToXZ;
			break;

		case ArrowDown::DOWN:
			if (mDwn == MouseDown::RIGHT) m_position += m_zoom * multiplier * dt * cam_dir;
			else m_position += m_zoom * multiplier * dt * projCamDirToXZ;
			break;
		}

		switch (sidesDwn)
		{
		case ArrowDown::LEFT:
			m_position -= m_zoom * multiplier * dt * cam_right;
			break;

		case ArrowDown::RIGHT:
			m_position += m_zoom * multiplier * dt * cam_right;
			break;
		}
	}

	// YZ plane
	{
		if (mDwn == MouseDown::MIDDLE && m_input->IsKeyDown(VK_MBUTTON))
		{
			m_position += (oriMousePos.x - curMousePos.x) * multiplier * cam_right;
			m_position.y += (curMousePos.y - oriMousePos.y) * multiplier;
			SetCursorPos((int)oriMousePos.x, (int)oriMousePos.y);
		}
	}
}

void EditorCamera::Zoom()
{
	if (!Editor::GetInstance().IsHoveringEditor())
	{
		if (m_input->getScrollDeltaY() < 0.0f)
		{
			m_zoom += zoomAmount * multiplier;
			//m_cam_speed += zoomAmount;
			m_position += zoomAmount * multiplier * cam_dir;
		}
		else if (m_input->getScrollDeltaY() > 0.0f)
		{
			m_zoom -= m_zoom - zoomAmount * multiplier <= minZoom ? 0 : zoomAmount;
			//m_cam_speed -= m_cam_speed - zoomAmount > 0 ? zoomAmount : 0.f;
			m_position -= zoomAmount * multiplier * cam_dir;
		}
	}
}

void EditorCamera::RotateCamera()
{
	bool orbit = mDwn == MouseDown::LEFT && m_input->IsKeyDown(VK_LBUTTON) && altHeld;
	bool fps = mDwn == MouseDown::RIGHT && m_input->IsKeyDown(VK_RBUTTON);

	if (!ImGuizmo::IsUsing() && !Editor::GetInstance().IsEditorInUse() && (orbit || fps))
	{
		Vec2 mouseDelta = (curMousePos - oriMousePos) * m_sensitivity;
		m_yaw += mouseDelta.x;
    m_pitch += mouseDelta.y;

		//m_pitch -= mouseDelta.y;                                            //yh changed so water reflection works
		SetCursorPos((int)oriMousePos.x, (int)oriMousePos.y);

		if (orbit)
		{
			float rad_yaw = m_yaw * DEG2RAD;
			float rad_pitch = m_pitch * DEG2RAD;
			Vec3 vec(std::sin(rad_yaw) * std::cos(rad_pitch), std::sin(rad_pitch), -std::cos(rad_yaw) * std::cos(rad_pitch));
			vec.normalize();
			m_position.x = pivotPos.x - m_zoom * vec.x;
			m_position.y = pivotPos.y + m_zoom * vec.y;
			m_position.z = pivotPos.z - m_zoom * vec.z;
		}
	}
}


void EditorCamera::UpdateVars()
{
	GetCursorPos(&curMousePt);
	curMousePos = Vec2((float)curMousePt.x, (float)curMousePt.y);

	cam_dir = -m_forward;
	cam_right = m_right;

	dt = static_cast<float>(FrameRateController::GetInstance().GetDelta(false));
	multiplier = (IsShiftHeld() ? 3.f : 1.0f);

	projWorldUp = m_worldUp.dot(cam_dir) / m_worldUp.dot(m_worldUp) * m_worldUp;
	projCamDirToXZ = cam_dir - projWorldUp;
	projCamDirToXZ.normalize();

	if (!IsAltHeld() && !IsCtrlHeld() && !IsShiftHeld() && !Editor::GetInstance().IsHoveringEditor() && !Editor::GetInstance().IsUsingEditor() && !ImGuizmo::IsOver() && !ImGuizmo::IsUsing() && InputManager::GetInstance().IsKeyPressed(VK_LBUTTON))
	{
		Vec3 curMouseWorld = GraphicsSystem::GetInstance().ConvertScreenToWorld(GraphicsSystem::GetInstance().GetCurrentMousePosition());
		Vec3 dir = curMouseWorld - m_position;

		std::list<MeshRaycastInfo> collided = CollisionManager::GetInstance().EditorMeshRaycast(m_position, dir);

		float minDist = (std::numeric_limits<float>::max());
		EntityHandle hnd;
		if (collided.size())
		{
			for (auto& cinfo : collided)
			{
				if (minDist > cinfo.dist)
				{
					hnd = cinfo.myself;
					minDist = cinfo.dist;
				}
			}
		}

		Editor::GetInstance().SetSelectedEntity(hnd);

		//if (collided.empty())
		//	std::cout << "Raycasted on nothing\n";
		//else
		//{
		//	std::cout << "Raycast List:\n";
		//	for (auto& elem : collided)
		//	{
		//		std::cout << (*elem.myself).info_.entityName_ << std::endl;
		//	}
		//}
		//std::cout << "end raycast\n";
	}
}

void EditorCamera::InputEnter()
{
	// Keys for moving in xz plane
	if (forwardDwn == ArrowDown::NONE)
	{
		if (!Editor::GetInstance().IsEditorInUse())
		{
			if (m_input->IsKeyDown(KEY_W)) forwardDwn = ArrowDown::UP;
			else if (m_input->IsKeyDown(KEY_S)) forwardDwn = ArrowDown::DOWN;

			if (m_input->IsKeyDown(VK_UP)) forwardDwn = ArrowDown::UP;
			else if (m_input->IsKeyDown(VK_DOWN)) forwardDwn = ArrowDown::DOWN;
		}
	}

	// Keys for moving in xz plane
	if (sidesDwn == ArrowDown::NONE)
	{
		if (!Editor::GetInstance().IsEditorInUse())
		{
			if (m_input->IsKeyDown(KEY_A)) sidesDwn = ArrowDown::LEFT;
			else if (m_input->IsKeyDown(KEY_D)) sidesDwn = ArrowDown::RIGHT;

			if (m_input->IsKeyDown(VK_LEFT)) sidesDwn = ArrowDown::LEFT;
			else if (m_input->IsKeyDown(VK_RIGHT)) sidesDwn = ArrowDown::RIGHT;
		}
	}

	if (mDwn == MouseDown::NONE && !Editor::GetInstance().IsEditorInUse() && !ImGuizmo::IsUsing())
	{
		if (m_input->IsKeyPressed(VK_LBUTTON))
		{
			oriMousePos = curMousePos;
			pivotPos = m_position - cam_dir * m_zoom;
			mDwn = MouseDown::LEFT;
		}
		else if (m_input->IsKeyPressed(VK_MBUTTON))
		{
			oriMousePos = curMousePos;
			mDwn = MouseDown::MIDDLE;
		}
		else if (m_input->IsKeyPressed(VK_RBUTTON))
		{
			oriMousePos = curMousePos;
			mDwn = MouseDown::RIGHT;
		}
	}

	altHeld = IsAltHeld();
}

void EditorCamera::InputExit()
{
	if (m_input->IsKeyReleased(KEY_W)
		|| m_input->IsKeyReleased(KEY_S)
		|| m_input->IsKeyReleased(VK_UP)
		|| m_input->IsKeyReleased(VK_DOWN))
		forwardDwn = ArrowDown::NONE;

	if (m_input->IsKeyReleased(KEY_A)
		|| m_input->IsKeyReleased(KEY_D)
		|| m_input->IsKeyReleased(VK_LEFT)
		|| m_input->IsKeyReleased(VK_RIGHT))
		sidesDwn = ArrowDown::NONE;

	if (m_input->IsKeyReleased(VK_LBUTTON)
		|| m_input->IsKeyReleased(VK_RBUTTON)
		|| m_input->IsKeyReleased(VK_MBUTTON))
		mDwn = MouseDown::NONE;
}

void EditorCamera::UpdateMovement()
{
	InputEnter();
	UpdateVars();

	PlaneMovement();
	Zoom();
	RotateCamera();

	InputExit();

#ifdef CAMERA_DEBUG
  std::string x = std::to_string(m_position.x);
  std::string y = std::to_string(m_position.y);
  std::string z = std::to_string(m_position.z);
  std::string yaw = std::to_string(m_yaw);
  std::string pitch = std::to_string(m_pitch);
  std::string roll = std::to_string(m_roll);

  MLogX_("xpos: " + x);
  MLogX_("ypos: " + y);
  MLogX_("zpos: " + z);
  MLogX_("yaw: " + yaw);
  MLogX_("pitch: " + pitch);
  MLogX_("roll: " + roll);
#endif

	UpdateView();
}

void EditorCamera::UpdateView()
{
#if 1
	Quaternion qPitch = Quaternion(m_pitch, Vec3::EX());
	Quaternion qYaw = Quaternion(m_yaw, Vec3::EY());

	Quaternion curr_orient = qPitch * qYaw;
	curr_orient.Normalize();

	Mtx4 rot = curr_orient.ToMatrix();

	//inverse translation
	Vec3 translation(m_position.x, m_position.y, m_position.z);
	Mtx4 transMat = Mtx4::translate(-translation);

	//translate back to origin and rotate
	m_viewMatrix = rot * transMat;

	m_forward = -m_viewMatrix.row(2).ToVec3();
	m_right = m_viewMatrix.row(0).ToVec3();
	m_up = m_viewMatrix.row(1).ToVec3();
	m_forward.normalize();
	m_right.normalize();
	m_up.normalize();
#endif

  //yh changed to make water reflection work
  //CameraBase::UpdateView();
}

void EditorCamera::UpdateZoom()
{
	/*
	float scrolldelta = m_input->getScrollDeltaY();

	if (m_fov >= 1.f && m_fov <= 180.f)
		m_fov -= scrolldelta;

	m_fov = (m_fov < 1.f) ? 1.f : m_fov;
	m_fov = (m_fov > 180.f) ? 180.f : m_fov;
	*/
}
