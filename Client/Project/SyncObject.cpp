#include "stdafx.h"
#include "CSyncObjectManager.h"

SyncObject::SyncObject(CGameObject* obj) : CComponent(obj)
{
	id = -1;
	Define::SyncObjectManager->AddSyncObject(this);
}

SyncObject::~SyncObject()
{
}
