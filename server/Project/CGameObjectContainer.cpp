#include "stdafx.h"
#include "CGameObjectContainer.h"
#include "Player.h"
#include "BulletObject.h"
#include "Tree.h"
#include "CMountineTerrain.h"


CGameObject* CGameObjectContainer::CreateGameObject(int typeID)
{
	switch (typeID)
	{
	case TERRAIN:
		return new CMountineTerrain;
	case PLAYER:
		return new CPlayer;
	case BULLET:
		return new CBulletObject;
	//case TREE:
	//	return new CTree;
	default:
		return nullptr;
	}

}
