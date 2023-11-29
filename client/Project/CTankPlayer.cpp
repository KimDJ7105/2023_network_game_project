#include "stdafx.h"
#include "CTankPlayer.h"
#include "Shader.h"
#include "PlayerController.h"
#include "RigidBody.h"
#include "SyncObject.h"

CTankPlayer::CTankPlayer()
{
	//Define::Player = this;
	name = "Player";

	pivotObject = new CGameObject();
	pivotObject->name = "Pivot";
	SetChild(pivotObject, true);

	tank = new CTank();
	tank->BulletInit(0);
	tank->AddComponet(status);
	tank->AddComponet(new SyncObject(this));
	SetChild(tank, true);

	//string collidertag = "Player";
	//collider = new CCollider(this);
	//tank->upperBodyFrame->collider->tag = collidertag;
	tank->upperBodyFrame->AddComponet(new SyncObject(this));
	//tank->lowerBodyFrame->collider->tag = collidertag;
	tank->lowerBodyFrame->AddComponet(new SyncObject(this));
	//tank->gunFrame->collider->tag = collidertag;
	tank->gunFrame->AddComponet(new SyncObject(this));
	//m_pCamera = ChangeCamera(/*SPACESHIP_CAMERA*/THIRD_PERSON_CAMERA, 0.0f);

	tank->SetAllColor(0.0f, 0.0f, 0.8f);

	// init collider
	//collider->CollisionCall = [](CCollider& obj, CCollider& other) {
	//	if (other.tag == "Enumy Bullet")
	//	{
	//		auto status = obj.gameObject->GetComponet<CStatus>();
	//		status->hp -= dynamic_cast<CBulletObject*>(other.gameObject->m_pParent)->status->damage;
	//	}
	//};

	//for (int i = 0; i < tank->bulletIndex; i++)
	//{
	//	//tank->bullets[i]->collider->tag = "Player Bullet";
	//	tank->bullets[i]->missile->collider->tag = "Player Bullet";
	//	tank->bullets[i]->missile->collider->CollisionCall = [](CCollider& obj, CCollider& other) {
	//		if (other.tag != "Player")
	//			dynamic_cast<CBulletObject*>(obj.gameObject->root)->Reset();
	//	};
	//}

	status->maxHP = status->hp = 1000;
	status->speed = 3.0f;
	//AddComponet(new CRigidBody(this));
	//AddComponet(new CPlayerController(this));
	AddComponet(new SyncObject(this));
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