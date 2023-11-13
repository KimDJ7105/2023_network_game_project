#pragma once
#include "Object.h"
#include "BulletObject.h"

class CTank : public CGameObject
{
public:
	CTank();
	virtual ~CTank() {};

public:
	CGameObject* gunFrame = NULL;
	CGameObject* upperBodyFrame = NULL;
	float y_upper = 0;
	CGameObject* lowerBodyFrame = NULL;
	float y_Lower = 0;
	CGameObject* wheelParent;
	CGameObject* wheelsFrame[4];

	CBulletObject** bullets = NULL;
	int bulletIndex = 0;
	int spawnBulletIndex = 0;

public:
	void Move(const XMFLOAT3& xmf3Shift, const float& speed);
	void Rotate(float x, float y, float z);

public:
	void BulletInit(int index);
	void FireBullet(CGameObject* pLockedObject);

	void LowerBodyRotate(const XMFLOAT3& xmf3Shift);

	void Reset();
	void Die();

private:
	bool IsUpperLeftLower(float a, float b);
};

