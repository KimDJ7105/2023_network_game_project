#include "stdafx.h"
#include "Define.h"
#include "Object.h"

CSceneManager* Define::SceneManager = nullptr;

CGameFramework* Define::Framework = nullptr;
CGameTimer* Define::GameTimer = nullptr;
CInput* Define::Input = nullptr;

//list<CGameObject*> Define::GameObjectList;
deque<CGameObject*> Define::GameObjectList;
list<CCollider*> Define::ColliderList;
CCamera* Define::MainCamera = nullptr;
CPlayer* Define::Player = nullptr;

int Define::AddGameObject(CGameObject* addObj)
{
	for (int i = 0; i < GameObjectList.size(); i++)
	{
		if (GameObjectList[i]->isEmpty) continue;

		GameObjectList[i] = addObj;
		addObj->id = i;
		return i;
	}

	addObj->id = GameObjectList.size();
	GameObjectList.emplace_back(addObj);
	return addObj->id;
}

Define::~Define()
{
}
