#include "stdafx.h"
#include "Object.h"
#include "CSyncObjectManager.h"

CSyncObjectManager::CSyncObjectManager()
{
	Define::SyncObjectManager = this;
}

CSyncObjectManager::~CSyncObjectManager()
{
}

void CSyncObjectManager::AddSyncObject(SyncObject* sync)
{
	syncList.emplace_back(sync);
	sync->id = syncList.size() - 1;
}

bool CSyncObjectManager::DeleteSyncObject(SyncObject* sync)
{
	auto p = find(syncList.begin(), syncList.end(), sync);
	if (p != syncList.end())
	{
		syncList.erase(p);
		return true;
	}
	return false;
}

vector<sc_object_transform_packet> CSyncObjectManager::GetAllTransformPack()
{
	const int packSize = syncList.size();
	vector<sc_object_transform_packet> packlist(packSize);

	for (int i = 0; i < packSize; i++)
	{
		packlist[i].object_id = syncList[i]->id;
		XMStoreFloat3(&packlist[i].position, syncList[i]->gameObject->transform->GetLocalPosition());
		XMStoreFloat3(&packlist[i].rotate, syncList[i]->gameObject->transform->GetRotate());
		XMStoreFloat3(&packlist[i].scale, syncList[i]->gameObject->transform->GetScale());
		//packlist[i].position = syncList[i]->gameObject->transform->GetLocalPosition();
		//packlist[i].rotate = syncList[i]->gameObject->transform->GetRotate();
		//packlist[i].scale = syncList[i]->gameObject->transform->GetScale();
		packlist[i].matrix = syncList[i]->gameObject->transform->m_xmf4x4Transform;
		packlist[i].isActive = syncList[i]->gameObject->GetActive();
	}

	return packlist;
}

void CSyncObjectManager::SetTransformPack(vector<sc_object_transform_packet> packList)
{
	transformPackList = packList;
}

void CSyncObjectManager::UpdateAllTransformPack()
{
	for (auto pack : transformPackList)
	{
		if (syncList.size() <= pack.object_id) continue;
		syncList[pack.object_id]->gameObject->transform->m_xmf4x4World = pack.matrix;
	}

	transformPackList.clear();
}
