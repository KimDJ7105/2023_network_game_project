#include "stdafx.h"
#include "BigMissile.h"
#include "Object.h"
#include "Timer.h"
#include "Scene.h"
#include "Status.h"

CBigMissile::CBigMissile(CGameObject* obj) : CComponent(obj)
{
	gameObject->SetActive(false);
	gameObject->transform->SetPosition(0, 1000, 0);
}

void CBigMissile::Start()
{
	status = new CStatus(gameObject);
	gameObject->AddComponet(status);

	gameObject->transform->Rotate(0, 0, 180);
	XMStoreFloat3(&light->m_xmf3Direction, gameObject->transform->GetUp());

	status->speed = 300.0f;
	status->damage = 100.0f;

	Reset();

	gameObject->SetActive(true);
}

void CBigMissile::Update()
{	
	float deltaTime = Define::GameTimer->GetTimeElapsed();
	gameObject->transform->MoveUp(status->speed * deltaTime);
	gameObject->transform->Rotate(0, 10, 0);

	XMStoreFloat3(&light->m_xmf3Position, gameObject->transform->GetPosition() + gameObject->transform->GetUp() * gameObject->collider->bound->Extents.y );
}

void CBigMissile::Reset()
{
	light->m_bEnable = true;

	float y = 1000.0f;
	POINT pos{ fRandom(1, 100), fRandom(1,100) };

	gameObject->transform->SetPosition(pos.x, y, pos.y);

	XMStoreFloat3(&light->m_xmf3Position, gameObject->transform->GetPosition());

	gameObject->collider->SetActive(true);
}

void CBigMissile::OnCollision(CCollider& other)
{
	if (other.tag == "Terrain")
	{
		gameObject->collider->SetActive(false);
		Reset();
	}
	else if (other.tag == "Player")
	{
		//CStatus* playerStatus = Define::Player->GetChilde("Tank")->GetComponent<CStatus>();
		//playerStatus->hp -= status->damage;

		gameObject->collider->SetActive(false);
		Reset();
	}
}