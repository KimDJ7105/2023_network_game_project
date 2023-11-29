#include "stdafx.h"
#include "Define.h"
#include "Object.h"

CSceneManager* Define::SceneManager = nullptr;
CSyncObjectManager* Define::SyncObjectManager = nullptr;

CGameFramework* Define::Framework = nullptr;
CGameTimer* Define::GameTimer = nullptr;
CInput* Define::Input = nullptr;

list<CCollider*> Define::ColliderList;
CCamera* Define::MainCamera = nullptr;
CPlayer** Define::Players = nullptr;

SOCKET Define::sock;
int Define::ClientIndex = -1;

Define::Define()
{
	
}

Define::~Define()
{
}

void Define::RecvInputPack(int id)
{
	EVENT e{ id, -1 };
	recv(sock, (char*)&e, sizeof(EVENT), 0);
}
