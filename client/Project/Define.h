#pragma once

class CGameObject;
class CCollider;
class CCamera;
class CPlayer;
class CSceneManager;
class CGameFramework;
class CGameTimer;
class CInput;
class CSyncObjectManager;

class Define {
public: // Manager
	static CSceneManager* SceneManager;
	static CSyncObjectManager* SyncObjectManager;

public: // Framework
	static bool GameRunnig;
	static CGameFramework* Framework;
	static CGameTimer* GameTimer;
	static CInput* Input;

public:
	static list<CCollider*> ColliderList;
	static CCamera* MainCamera;
	static CPlayer** Players;

public: // Server Sock
	static SOCKET sock;
	static int ClientIndex;

public:
	Define();
	~Define();
};