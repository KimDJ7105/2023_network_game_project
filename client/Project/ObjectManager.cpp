#include "stdafx.h"
#include "ObjectManager.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
	for (const auto& obj : _GameObjectList)
		obj->Release();
	_GameObjectList.clear();
}

void ObjectManager::AddGameObject(CGameObject* obj)
{
	_GameObjectList.emplace_back(obj);
	obj->Start();
}

bool ObjectManager::DeleteGameObject(CGameObject* obj)
{
	if (DeleteObjectToList(obj))
	{
		obj->Release();
		delete obj;
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
		obj->transform->UpdateTransform(NULL);
}

void ObjectManager::AllGameObjectStart()
{
	for (const auto& obj : _GameObjectList)
		obj->Start();
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