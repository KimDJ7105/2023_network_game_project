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
		return new CMountineTerrain;
	case PLAYER:
		CTankPlayer* player = new CTankPlayer();
		if (CGameObject::FindObject("Player") == nullptr) player->playerID = 0;
		else player->playerID = 1;
		if (player->playerID == Define::ClientIndex) Define::Player = player;
		return player;
	case BULLET:
		return new CBulletObject;
		//case TREE:
		//	return new CTree;
	default:
		return nullptr;
	}

}
