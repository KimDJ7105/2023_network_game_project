#pragma once
#include "SyncObject.h"

class CSyncObjectManager
{
public:
	CSyncObjectManager();
	~CSyncObjectManager();

public:
	deque<SyncObject*>* GetSyncList() { return &syncList; }
	void AddSyncObject(SyncObject* sync);
	bool DeleteSyncObject(SyncObject* sync);

public: // transform packet
	vector<sc_object_transform_packet> GetAllTransformPack();
	void SetTransformPack(vector<sc_object_transform_packet> packList);

	void UpdateAllTransformPack();

public: // create Pack
	void AddCreatePack(sc_create_object_packet pack) { _CreatePack.emplace_back(pack); }
	void AddRangeCreatePack(vector<sc_create_object_packet>* list);
	void AllCreatePackUpdate();

private:
	deque<SyncObject*> syncList;
	deque<sc_create_object_packet> _CreatePack;
	vector<sc_object_transform_packet> transformPackList;
	CRITICAL_SECTION cs;
};

