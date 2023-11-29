#pragma once

enum KeyCode {
	Q, W, E, R,
	A, S, D, F,
	Space,
};

class CKey {
public:
	CKey();
	~CKey() {}

public:
	void KeyStateUpdate();
	void OnProceesingKeyBoardMassage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool GetKeyAny();
	bool GetKeyDown(KeyCode code); // 최초의 1회 눌렀을 시만 true
	bool GetKeyPress(KeyCode code); // 눌려져 있는 키일 경우 true

protected:
	map<KeyCode, WPARAM> key;
	map<WPARAM, unsigned __int8> KeyState; // 0 : 아무것도 안함, 1 : Key Down , 2 : Key Up, 3 : Key Press
};

enum MouseButton {
	Left,
	Middle,
	Right,
};

class CMouse {
public:
	CMouse();
	~CMouse() {}

public:
	void MouseStateUpdate();
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool GetMouseDown(MouseButton button);
	bool GetMouseUp(MouseButton button);
	bool GetMousePress(MouseButton button);
	
	POINT GetMouseAxis();
	bool IsMoveAxis();

protected:
	map<MouseButton, unsigned __int8> MouseState; // 0 : 아무것도 안함, 1 : Mouse Down , 2 : Mouse Up, 3 : Mouse Press

	POINT oldCursorPos;
	POINT nowCursorPos;
};

class CInput : public CKey, public CMouse
{
public:
	CInput();
	~CInput() {}
};