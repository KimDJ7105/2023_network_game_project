#include "stdafx.h"
#include "Object.h"
#include "Timer.h"
#include "RigidBody.h"

CRigidBody::CRigidBody(CGameObject* obj) : CComponent(obj)
{
	gravity = XMVectorSet(0, -1, 0, 0);
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::Update()
{
	XMVECTOR pos = gameObject->transform->GetPosition() + gravity * Define::GameTimer->GetTimeElapsed() * 10.0f;
	gameObject->transform->SetPosition(pos);
}