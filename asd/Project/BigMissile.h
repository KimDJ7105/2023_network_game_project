#pragma once
#include "Component.h"

struct LIGHT;
class CStatus;

class CBigMissile : public CComponent
{
public:
	CBigMissile(CGameObject* obj);
	~CBigMissile(){}

public:
	virtual void Start();
	virtual void Update();

	virtual void OnCollision(CCollider& other);

private:
	virtual void Reset();

public:
	LIGHT* light;

private:
	CStatus* status;
};

