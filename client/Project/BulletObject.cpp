#include "stdafx.h"
#include "BulletObject.h"
#include "SyncObject.h"

CBulletObject::CBulletObject()
{
	name = "Bullet";

	missile = CGameObject::LoadGeometryFromFile("Model/Missile.bin");
	//missile = new CGameObject();
	missile->AddComponet(new SyncObject(missile));
	missile->name = "Missile";
	//missile->collider = new CCollider(missile);
	//missile->collider->tag = "Bullet";
	SetChild(missile);

	//collisionParticle = new CParticleSystem(10);
	//collisionParticle->duration = 5.0f;
	//collisionParticle->SetSpeed(1.5f, 4.0f);
	//
	//shotParticle = new CParticleSystem(4);
	//shotParticle->duration = 0.3f;
	//shotParticle->SetSpeed(1.0f, 3.0f);

	//collisionParticle->SetActive(false);
	//shotParticle->SetActive(false);
	SetActive(false);

	status->damage = 10;

	missile->AddComponet(status);
	AddComponet(new SyncObject(this));
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::Update()
{
	CGameObject::Update();

	if (GetActive() == true)
	{
		float fDistance = moveDirectionSpeed * Define::GameTimer->GetTimeElapsed();

		transform->MoveForward(fDistance);
		m_fMovingDistance += fDistance;

		if (XMVectorGetY(transform->GetPosition()) < 0) Reset();
		if (m_fMovingDistance > m_fBulletEffectiveRange) Reset();
	}
}

void CBulletObject::Fire()
{
	SetActive(true);
	collisionParticle->Stop();

	shotParticle->transform->SetPosition(transform->GetPosition() + transform->GetLook());
	shotParticle->Play();
}

void CBulletObject::Reset()
{
	m_fMovingDistance = 0;
	moveDirectionSpeed = 0;

	//missile->UpdateTransform();
	collisionParticle->transform->SetPosition(transform->GetPosition() + transform->GetLook() * 1.5f);
	collisionParticle->Play();
	shotParticle->Stop();

	SetActive(false);
}