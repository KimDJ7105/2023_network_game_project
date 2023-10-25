#pragma once
#include "Component.h"

class CRigidBody : public CComponent
{
public:
	CRigidBody(CGameObject* obj);
	~CRigidBody();

public:
	virtual void Update();
	
public: //getter setter
	void SetGravity(XMVECTOR _Gravity) { gravity = _Gravity; }
	void SetGravity(XMFLOAT3& _Gravity) { gravity = XMLoadFloat3(&_Gravity); }
	void SetGravity(float x, float y, float z) { SetGravity(XMFLOAT3(x, y, z)); }
	XMVECTOR GetGravity() { return gravity; }

private:
	XMVECTOR gravity;
};

