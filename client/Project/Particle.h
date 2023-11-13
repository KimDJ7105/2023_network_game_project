#pragma once
#include "Object.h"
#include "Timer.h"

class CParticleSystem : public CGameObject
{
public:
	CParticleSystem(int index);
	~CParticleSystem();

public:
	int particleIndex = 0;
	double duration = 1.0f;

private:
	int ObjectIndex = 1;
	CGameObject** particleObjects;
	CTransform** transforms;
	XMFLOAT3 angle;

	CTimer lifeTime;
	bool isLoop;
	
	bool isSameSpeed = true;
	float minSpeed;
	float maxSpeed;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void Update();

	void Play();
	void Stop();

public:
	void SetAngle(XMFLOAT3& _Angle);
	void SetSpeed(float min, float max);
	void SetSpeed(float speed) { SetSpeed(speed, speed); }
};