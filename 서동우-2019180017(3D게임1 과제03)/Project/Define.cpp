#include "stdafx.h"
#include "Define.h"

CSceneManager* Define::SceneManager = nullptr;

CGameFramework* Define::Framework = nullptr;
CGameTimer* Define::GameTimer = nullptr;
CInput* Define::Input = nullptr;

list<CGameObject*> Define::GameObjectList;
list<CCollider*> Define::ColliderList;
CCamera* Define::MainCamera = nullptr;
CPlayer* Define::Player = nullptr;

Define::~Define()
{
}
