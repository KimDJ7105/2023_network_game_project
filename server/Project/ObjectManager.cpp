#include "stdafx.h"
#include "ObjectManager.h"
#include "SyncObject.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
	for (const auto& obj : _GameObjectList)
		obj->Release();
	_GameObjectList.clear();
}

void ObjectManager::AddCreatePack(CGameObject* obj)
{
	_CreatePack.emplace_back(sc_create_object_packet{ obj->object_Type, obj->transform->m_xmf4x4World });
}

void ObjectManager::AddGameObject(CGameObject* obj)
{
	if (obj == nullptr) return;
	if (obj->m_pParent != nullptr) return;

	_GameObjectList.emplace_back(obj);
	_CreateObjectList.emplace_back(obj);
}

bool ObjectManager::DeleteGameObject(CGameObject* obj)
{
	if (obj == nullptr) return false;

	if (DeleteObjectToList(obj))
	{
		_DeletePack.emplace_back(sc_delete_object_packet{ obj->id });
		obj->Release();
		return true;
	}

	return false;
}

CGameObject* ObjectManager::FindGameObject(string name)
{
	auto p = find_if(_GameObjectList.begin(), _GameObjectList.end(), [name](const CGameObject* obj) {return obj->name._Equal(name); });
	if (p == _GameObjectList.end()) return nullptr;
	else return *p;
}

CGameObject* ObjectManager::FindGameObject(CGameObject* obj)
{
	auto p = find(_GameObjectList.begin(), _GameObjectList.end(), obj);
	if (p != _GameObjectList.end()) return *p;

	return nullptr;
}

bool ObjectManager::DeleteObjectToList(CGameObject* obj)
{
	auto p = find(_GameObjectList.begin(), _GameObjectList.end(), obj);
	if (p != _GameObjectList.end())
	{
		_GameObjectList.erase(p);
		return true;
	}

	return false;
}

void ObjectManager::GameObjectTransformUpdate(int id)
{
	auto obj = FindGameObject(id);
	if (obj != nullptr)
		obj->transform->UpdateTransform(NULL);
}

void ObjectManager::GameObjectStart(int id)
{
	auto obj = FindGameObject(id);
	if (obj != nullptr)
		obj->Start();
}

void ObjectManager::GameObjectUpdate(int id)
{
	auto obj = FindGameObject(id);
	if (obj != nullptr)
		obj->Update();
}

void ObjectManager::GameObjectLateUpdate(int id)
{
	auto obj = FindGameObject(id);
	if (obj != nullptr)
		obj->LateUpdate();
}

void ObjectManager::AllReleaseUploadBuffers()
{
	for (const auto& obj : _GameObjectList)
		obj->ReleaseUploadBuffers();
}

void ObjectManager::AllGameObjectUpdateTransform()
{
	for (const auto& obj : _GameObjectList)
	{
		obj->transform->UpdateTransform(NULL);
	}
}

void ObjectManager::AllGameObjectStart()
{
	for (const auto& obj : _GameObjectList)
		obj->Start();
}

void ObjectManager::AllCreateObjectStart()
{
	for (const auto& obj : _CreateObjectList)
		obj->Start();
	_CreateObjectList.clear();
}

void ObjectManager::AllGameObjectUpdate()
{
	for (const auto& obj : _GameObjectList)
		obj->Update();
}

void ObjectManager::AllGameObjectLateUpdate()
{
	for (const auto& obj : _GameObjectList)
		obj->LateUpdate();
}

void ObjectManager::AllGameObjectRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (const auto& obj : _GameObjectList)
		obj->Render(pd3dCommandList, pCamera);
}
