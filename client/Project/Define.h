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
	static CGameFramework* Framework;
	static CGameTimer* GameTimer;
	static CInput* Input;

public:
	static list<CCollider*> ColliderList;
	static CCamera* MainCamera;
	static CPlayer* Player;

public: // Server Sock
	static SOCKET sock;
	static int ClientIndex;

	static void RecvInputPack(int id);

public:
	Define() {}
	~Define();
};