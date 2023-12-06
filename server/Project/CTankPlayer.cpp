#include "stdafx.h"
#include "CTankPlayer.h"
#include "Shader.h"
#include "PlayerController.h"
#include "RigidBody.h"
#include "SyncObject.h"	

CTankPlayer::CTankPlayer(int type) : CPlayer(type)
{
	//Define::Player = this;
	name = "Player";

	pivotObject = new CGameObject();
	pivotObject->name = "Pivot";
	pivotObject->AddComponet(new SyncObject(pivotObject));
	SetChild(pivotObject, true);

	tank = new CTank();
	tank->BulletInit(1);
	tank->AddComponet(status);
	tank->AddComponet(new SyncObject(tank));
	SetChild(tank, true);

	string collidertag = "Player";
	collider = new CCollider(this);
	tank->upperBodyFrame->collider->tag = collidertag;
	tank->upperBodyFrame->AddComponet(new SyncObject(tank->upperBodyFrame));
	tank->lowerBodyFrame->collider->tag = collidertag;
	tank->lowerBodyFrame->AddComponet(new SyncObject(tank->lowerBodyFrame));
	tank->gunFrame->collider->tag = collidertag;
	tank->gunFrame->AddComponet(new SyncObject(tank->gunFrame));
	//m_pCamera = ChangeCamera(/*SPACESHIP_CAMERA*/THIRD_PERSON_CAMERA, 0.0f);
	tank->wheelParent->AddComponet(new SyncObject(tank->wheelParent));
	for (int i = 0; i < 4; i++)
		tank->wheelsFrame[i]->AddComponet(new SyncObject(tank->wheelsFrame[i]));


	tank->SetAllColor(0.0f, 0.0f, 0.8f);

	// init collider
	collider->CollisionCall = [](CCollider& obj, CCollider& other) {
		if (other.tag == "Bullet")
		{
			auto status = obj.gameObject->GetComponent<CStatus>();
			status->hp -= dynamic_cast<CBulletObject*>(other.gameObject->m_pParent)->status->damage;
			printf("%s hp : %f\n", obj.gameObject->name.data(), status->hp);
			if (status->CheckDie())
				obj.gameObject->GetRoot()->SetActive(false);
		}
	};

	for (int i = 0; i < tank->bulletIndex; i++)
	{
		//tank->bullets[i]->collider->tag = "Bullet";
		tank->bullets[i]->missile->collider->tag = "Bullet";
		tank->bullets[i]->missile->collider->CollisionCall = [](CCollider& obj, CCollider& other) {
			dynamic_cast<CBulletObject*>(obj.gameObject->root)->Reset();
		};
	}

	status->maxHP = status->hp = 100;
	status->speed = 3.0f;
	AddComponet(new CRigidBody(this));
	AddComponet(new CPlayerController(this));
	AddComponet(new SyncObject(this));
	AddComponet(status);
}

CTankPlayer::~CTankPlayer()
{
}

void CTankPlayer::Collision(CCollider& other)
{
	if (other.tag == "Enumy Bullet")
	{
		status->hp -= dynamic_cast<CBulletObject*>(other.gameObject->m_pParent)->status->damage;
	}

	CGameObject::Collision(other);
}

void CTankPlayer::FireBullet(CGameObject* pLockedObject)
{
	fireRate.End();
	if (fireRate.GetTimer().count() > 1.0f) {
		tank->FireBullet(pLockedObject);
		fireRate.Start();
	}
}