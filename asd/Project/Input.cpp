#include "stdafx.h"
#include "Input.h"
#include "GameFramework.h"

CKey::CKey()
{
	key[KeyCode::Q] = 'Q'; key[KeyCode::W] = 'W'; key[KeyCode::E] = 'E'; key[KeyCode::R] = 'R';
	key[KeyCode::A] = 'A'; key[KeyCode::S] = 'S'; key[KeyCode::D] = 'D'; key[KeyCode::F] = 'F';
	key[KeyCode::Space] = VK_SPACE;
}

void CKey::KeyStateUpdate()
{
	for (auto& pair : KeyState)
	{
		if (pair.second == 1 || pair.second == 3) pair.second = 3;
		else pair.second = 0;
	}
}

void CKey::OnProceesingKeyBoardMassage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		KeyState[wParam] = 1;	
		break;
	case WM_KEYUP:
		KeyState[wParam] = 2;
		break;
	}
}

bool CKey::GetKeyDown(KeyCode code)
{
	return (KeyState[key[code]] == 1 ) ? true : false;
}

bool CKey::GetKeyPress(KeyCode code)
{
	return (KeyState[key[code]] == 3 || KeyState[key[code]] == 1) ? true : false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Mouse

CMouse::CMouse()
{
	GetCursorPos(&oldCursorPos);
	MouseState[MouseButton::Left] = 0;
	MouseState[MouseButton::Middle] = 0;
	MouseState[MouseButton::Right] = 0;
}

void CMouse::MouseStateUpdate()
{
	//SetCursorPos(oldCursorPos.x, oldCursorPos.y);
	for (auto& pair : MouseState)
	{
		if (pair.second == 1 || pair.second == 3) pair.second = 3;
		else pair.second = 0;
	}
}

void CMouse::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		MouseState[MouseButton::Left] = 1;
		GetCursorPos(&oldCursorPos);
		break;
	case WM_RBUTTONDOWN:
		MouseState[MouseButton::Right] = 1;
		GetCursorPos(&oldCursorPos);
		break;
	case WM_LBUTTONUP:
		MouseState[MouseButton::Left] = 2;
		break;
	case WM_RBUTTONUP:
		MouseState[MouseButton::Right] = 2;
		break;
	case WM_MOUSEMOVE:
		GetCursorPos(&oldCursorPos);
		break;
	default:
		break;
	}
}

bool CMouse::GetMouseDown(MouseButton button)
{
	return (MouseState[button] == 1) ? true : false;
}

bool CMouse::GetMouseUp(MouseButton button)
{
	return (MouseState[button] == 2) ? true : false;
}

bool CMouse::GetMousePress(MouseButton button)
{
	return (MouseState[button] == 3) ? true : false;
}

POINT CMouse::GetMouseAxis()
{
	POINT Axis{ 0,0 };
	if (GetCapture() == Define::Framework->m_hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&nowCursorPos);
		Axis.x = (float)(nowCursorPos.x - oldCursorPos.x);
		Axis.y = (float)(nowCursorPos.y - oldCursorPos.y);
	}

	return Axis;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 


CInput::CInput()
{
	Define::Input = this;
}