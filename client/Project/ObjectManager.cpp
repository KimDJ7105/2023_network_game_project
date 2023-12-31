#include "stdafx.h"
#include "ObjectManager.h"
#include "CGameObjectContainer.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
	for (const auto& obj : _GameObjectList)
		obj->Release();
	_GameObjectList.clear();
	_CreateObjectList.clear();

	_DeletePack.clear();
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
		obj->Release();
		return true;
	}

	return false;
}

CGameObject* ObjectManager::FindGameObject(int id)
{
	if(_GameObjectList.size() > id) 
		return _GameObjectList[id]; 
	return nullptr;
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

bool ObjectManager::DeleteObjectToStarList(CGameObject* obj)
{
	auto p = find(_CreateObjectList.begin(), _CreateObjectList.end(), obj);
	if (p != _CreateObjectList.end())
	{
		_CreateObjectList.erase(p);
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
		obj->transform->UpdateTransform(NULL);
}

void ObjectManager::AllDeletePackUpdate()
{
	for (const auto pack : _DeletePack)
	{
		DeleteGameObject(pack.object_id);
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
}

void ObjectManager::AllCreateObjectReleaseUploadBuffers()
{
	for (const auto obj : _CreateObjectList)
		obj->ReleaseUploadBuffers();
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
