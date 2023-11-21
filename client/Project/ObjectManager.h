#pragma once

#include "Object.h"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

public: // List function
	deque<CGameObject*>* GetList() { return &_GameObjectList; }

	void AddCreatePack(sc_create_object_packet pack) { _CretaePack.emplace_back(pack); }
	void AddDeletePack(sc_delete_object_packet pack) { _DeletePack.emplace_back(pack); }
	void AddTransformPack(sc_object_transform_packet pack) { _TransformPack.emplace_back(pack); }

	void AllCreatePackUpdate();
	void AllTransformPackUpdate();
public:
	void AddGameObject(CGameObject* obj);
	bool DeleteGameObject(CGameObject* obj);
	bool DeleteGameObject(int id) { return	DeleteGameObject(FindGameObject(id)); }
	CGameObject* FindGameObject(int id) { return _GameObjectList[id]; }
	CGameObject* FindGameObject(string name);
	CGameObject* FindGameObject(CGameObject* obj);

	bool DeleteObjectToList(CGameObject* obj);
	
public:
	void GameObjectTransformUpdate(int id);
	void GameObjectStart(int id);
	void GameObjectUpdate(int id);
	void GameObjectLateUpdate(int id);

public:
	void AllReleaseUploadBuffers();
	void AllGameObjectUpdateTransform();

	void AllGameObjectStart();
	void AllGameObjectUpdate();
	void AllGameObjectLateUpdate();

	void AllGameObjectRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

private:
	deque<CGameObject*> _GameObjectList;
	vector<sc_create_object_packet> _CretaePack;
	vector<sc_delete_object_packet> _DeletePack;
	vector<sc_object_transform_packet> _TransformPack;
};