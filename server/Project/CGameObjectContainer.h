#pragma once
#include "stdafx.h"

class CGameObject;

class CGameObjectContainer
{
public:
	CGameObjectContainer() {}
	~CGameObjectContainer(){}
	static CGameObject* CreateGameObject(int typeID);
};