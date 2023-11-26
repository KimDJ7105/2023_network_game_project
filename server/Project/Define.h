#pragma once

class CGameObject;
class CCollider;
class CCamera;
class CPlayer;
class CSceneManager;
class CGameFramework;
class CGameTimer;
class CInput;

class Define {
public: // Manager
	static CSceneManager* SceneManager;

public: // Framework
	static CGameFramework* Framework;
	static CGameTimer* GameTimer;
	static CInput* Input;

public:
	//static list<CGameObject*> GameObjectList;
	static deque<CGameObject*> GameObjectList;
	static int AddGameObject(CGameObject* addObj);
	static list<CCollider*> ColliderList;
	static CCamera* MainCamera;
	static CPlayer* Player;

public: // Server Sock
	static SOCKET sock[2];

public:
	Define() {}
	~Define();
};