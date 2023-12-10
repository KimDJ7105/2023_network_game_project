#pragma once
#include "SyncObject.h"

class CSyncObjectManager
{
public:
	CSyncObjectManager();
	~CSyncObjectManager();

public:
	void AddSyncObject(SyncObject* sync);
	bool DeleteSyncObject(SyncObject* sync);

public: // packet
	vector<sc_object_transform_packet> GetAllTransformPack();
	void SetTransformPack(vector<sc_object_transform_packet> packList);
		
	void UpdateAllTransformPack();

private:
	deque<SyncObject*> syncList;
	vector<sc_object_transform_packet> transformPackList;
};

