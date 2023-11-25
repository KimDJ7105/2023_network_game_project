#pragma once

class CGameObject;
class CCollider;
class CCamera;
class CPlayer;
class CSceneManager;
class CGameFramework;
class CGameTimer;
class CInput;
struct Eveet;

class Define {
public: // Manager
	static CSceneManager* SceneManager;

public: // Framework
	static CGameFramework* Framework;
	static CGameTimer* GameTimer;
	static CInput* Input;

public:
	//static list<CGameObject*> GameObjectList;
	static list<CCollider*> ColliderList;
	static CCamera* MainCamera;
	static CPlayer* Player;

public: // Server Sock
	static SOCKET sock;

	static void RecvInputPack(int id);

public:
	Define() {}
	~Define();
};