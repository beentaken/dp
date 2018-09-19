/******************************************************************************/
/*!
\file		TinyBlackBoard.cpp
\project	CS380/CS580 AI Framework
\author		Chi-Hao Kuo
\summary	Local blackboard for tiny.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#include <Stdafx.h>

using namespace BT;

/* public methods */

/*--------------------------------------------------------------------------*
Name:           TinyBlackBoard

Description:    Default constructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
TinyBlackBoard::TinyBlackBoard()
	: m_npc(nullptr)
{
	m_mouseClick = false;
}

/*--------------------------------------------------------------------------*
Name:           OnMessage

Description:    Handle messages.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void TinyBlackBoard::OnMessage(void)
{
	// default behavior is to drop all messages

	if (m_msgqueue.size())
	{
		MSG_Object &msg = m_msgqueue.front();

		switch (msg.GetName())
		{
		case MSG_Reset:
		{
			m_isreset = true;
		}
			break;

		case MSG_MouseClick:
		{
			m_mouseClick = true;
			m_mousePos = msg.GetVector3Data();
		}
			break;

		default:
			break;
		}

		m_msgqueue.pop();
	}
}