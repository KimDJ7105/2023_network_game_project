#pragma once
#include "Particlelib.h"
#include "Status.h"

class CTank;

class CBulletObject : public CGameObject
{
public:
	CBulletObject();
	virtual ~CBulletObject();

public:

	float						m_fBulletEffectiveRange = 50.0f;
	float						m_fMovingDistance = 0.0f;

	CParticleSystem* collisionParticle;
	CParticleSystem* shotParticle;

	CStatus* status = new CStatus(this);
	CGameObject* missile;

private:
	float						moveDirectionSpeed = 1.0f;

public:
	virtual void Update();

	void Fire();
	void SetMoveDirectionSpeed(float speed) { moveDirectionSpeed = speed; }
	void Reset();
};
