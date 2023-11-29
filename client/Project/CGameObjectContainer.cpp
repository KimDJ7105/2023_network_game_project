#include "stdafx.h"
#include "CGameObjectContainer.h"
#include "CTankPlayer.h"
#include "BulletObject.h"
#include "Tree.h"
#include "CMountineTerrain.h"


CGameObject* CGameObjectContainer::CreateGameObject(int typeID)
{
	switch (typeID)
	{
	case TERRAIN:
	{
		CMountineTerrain* terrain = new CMountineTerrain();
		return terrain;
	}
	case PLAYER:
	{
		int playerID;
		if (CGameObject::FindObject("Player") == nullptr) playerID = 0;
		else playerID = 1;
		CTankPlayer* player = new CTankPlayer();
		player->playerID = playerID;
		Define::Players[playerID] = player;
		return player;
	}
	case BULLET:
		return new CBulletObject;
		//case TREE:
		//	return new CTree;
	default:
		return nullptr;
	}

}
