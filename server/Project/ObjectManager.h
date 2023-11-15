#pragma once

#include "Object.h"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

public: //getter setter
	deque<CGameObject*>* GetList() { return &_GameObjectList; }
	vector<sc_create_object_packet> GetCreatePack() { return _CreatePack; }
	vector<sc_delete_object_packet> GetDeletePack() { return _DeletePack; }
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

	vector<sc_object_transform_packet> AllTrnasformToPacket();

private:
	deque<CGameObject*> _GameObjectList;
	vector<sc_create_object_packet> _CreatePack;
	vector<sc_delete_object_packet> _DeletePack;
};