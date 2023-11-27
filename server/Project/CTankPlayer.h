#pragma once
#include "Player.h"
#include "Tank.h"

class CTankPlayer : public CPlayer
{
public:
	CTankPlayer(int type);
	virtual ~CTankPlayer();

public:
	CStatus* status = new CStatus(this);
	CTank* tank = NULL;
	CGameObject* pivotObject = nullptr;

private:
	CTimer fireRate;

public:
	virtual void Collision(CCollider& other) override;

	void FireBullet(CGameObject* pLockedObject);
};

