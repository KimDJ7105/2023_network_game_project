#pragma once

#include "Object.h"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

public: //getter setter
	deque<CGameObject*>* GetList() { return &_GameObjectList; }
	deque<CGameObject*>* GetCreateObjectList() { return &_CreateObjectList; }
	void ClearCreateObjectList() { _CreateObjectList.clear(); }
	void ClearActiveObjectList() { _ActiveObjectList.clear(); }

	vector<sc_create_object_packet>* GetCreatePack() { return &_CreatePack; }
	vector<sc_delete_object_packet>* GetDeletePack() { return &_DeletePack; }

public: //pack
	void AddCreatePack(CGameObject* obj);

public: // List
	void AddGameObject(CGameObject* obj);
	void AddActiveObject(CGameObject* obj, bool value) { _ActiveObjectList.emplace(obj, value); }
	bool DeleteGameObject(CGameObject* obj);
	bool DeleteGameObject(int id) { return	DeleteGameObject(FindGameObject(id)); }
	CGameObject* FindGameObject(int id) { return _GameObjectList[id]; }
	CGameObject* FindGameObject(string name);
	CGameObject* FindGameObject(CGameObject* obj);

	bool DeleteObjectToList(CGameObject* obj);
	bool DeleteObjectToStartList(CGameObject* obj);
	
public:
	void GameObjectTransformUpdate(int id);
	void GameObjectStart(int id);
	void GameObjectUpdate(int id);
	void GameObjectLateUpdate(int id);

public: // All Fucntion
	void AllReleaseUploadBuffers();
	void AllGameObjectUpdateTransform();

	void AllGameObjectStart();
	void AllCreateObjectStart();

	void AllGameObjectUpdate();
	void AllGameObjectLateUpdate();

	void AllActiveObjectUpdate();

	void AllGameObjectRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
private:
	deque<CGameObject*> _GameObjectList;
	deque<CGameObject*> _CreateObjectList;
	map<CGameObject*, bool> _ActiveObjectList;

private:
	vector<sc_create_object_packet> _CreatePack;
	vector<sc_delete_object_packet> _DeletePack;
};