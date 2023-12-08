#include "stdafx.h"
#include "Input.h"
#include "GameFramework.h"

CKey::CKey()
{
	isKeyDown = isKeyUp = false;

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
	isKeyDown = isKeyUp = false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		KeyState[wParam] = 1;
		isKeyDown = true;
		break;
	case WM_KEYUP:
		KeyState[wParam] = 2;
		isKeyUp = true;
		break;
	}
}

bool CKey::GetKeyAny()
{
	for (auto key : KeyState)
	{
		if (key.first == 1 || key.first == 3) return true;
	}
	return false;
}

bool CKey::GetKeyDown(KeyCode code)
{
	return (KeyState[key[code]] == 1 ) ? true : false;
}

bool CKey::GetKeyUp(KeyCode code)
{
	return (KeyState[key[code]] == 2) ? true : false;
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
		//::SetCapture(hWnd);
		MouseState[MouseButton::Left] = 1;
		GetCursorPos(&oldCursorPos);
		break;
	case WM_RBUTTONDOWN:
		//::SetCapture(hWnd);
		MouseState[MouseButton::Right] = 1;
		GetCursorPos(&oldCursorPos);
		break;
	case WM_LBUTTONUP:
		MouseState[MouseButton::Left] = 2;
		SetCursorPos(oldCursorPos.x, oldCursorPos.y);
		//::ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		MouseState[MouseButton::Right] = 2;
		SetCursorPos(oldCursorPos.x, oldCursorPos.y);
		//::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
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
	if (::GetCapture() == m_hWnd)
	{
		::SetCursor(NULL);
		GetCursorPos(&nowCursorPos);
		Axis.x = (float)(nowCursorPos.x - oldCursorPos.x);
		Axis.y = (float)(nowCursorPos.y - oldCursorPos.y);
		SetCursorPos(oldCursorPos.x, oldCursorPos.y);
	}
	return Axis;
}

bool CMouse::IsMoveAxis()
{
	if (nowCursorPos.x == oldCursorPos.x && nowCursorPos.y == oldCursorPos.y)
		return false;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 


CInput::CInput()
{
	Define::Input = this;
}