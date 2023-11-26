#pragma once
#include "Component.h"
class SyncObject : public CComponent
{
public:
	SyncObject(CGameObject* obj);
	~SyncObject();

public:
	int id;
};

