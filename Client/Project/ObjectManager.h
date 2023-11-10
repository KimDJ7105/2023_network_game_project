#pragma once

#include "Object.h"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	deque<CGameObject*>* GetList() { return &_GameObjectList; }

	CGameObject* GetGameObject(int id) { return _GameObjectList[id]; }
	void AddGameObject(CGameObject* obj);
	bool DeletGameObject(CGameObject* obj);
	CGameObject* FindGameObject(int id);
	CGameObject* FindGameObject(string name);
	CGameObject* FindGameObject(CGameObject* obj);

	bool DeleteObjectToList(CGameObject* obj);
	
public:
	void GameObjectTransformUpdate(int id);

public:
	void AllReleaseUploadBuffers();
	void AllGameObjectUpdateTransform();

	void AllGameObjectStart();
	void AllGameObjectUpdate();
	void AllGameObjectLateUpdate();

	void AllGameObjectRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

private:
	deque<CGameObject*> _GameObjectList;
};