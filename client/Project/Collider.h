#pragma once
#include "Component.h"

class CGameObject;

class CCollider : public CComponent
{
public:
	CCollider(CGameObject* obj);
	~CCollider() {};

	void UpdateBoundingBox();

public: // getter setter
	void SetPivot(XMVECTOR& pos) { pivot = pos; }
	void SetPivot(XMFLOAT3& pos) { SetPivot(XMLoadFloat3(&pos)); }
	void SetPivot(float x, float y, float z) { SetPivot(XMFLOAT3(x,y,z)); }
	XMVECTOR GetPivot() { return pivot; }

public:
	string tag;

	BoundingOrientedBox* bound = nullptr;
	void (*CollisionCall)(CCollider& obj, CCollider& other) = [](CCollider& obj, CCollider& other) {};
	
private:
	XMVECTOR pivot;
};

