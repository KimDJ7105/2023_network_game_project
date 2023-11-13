#include "stdafx.h"
#include "CameraController.h"
#include "Object.h"
#include "Player.h"
#include "Terrain.h"

CCameraController::CCameraController(CGameObject* obj) : CComponent(obj)
{
	offsetDistance = 10.0f;
}

void CCameraController::Start()
{
	target = Define::Player->GetChilde("Pivot");
	terrain = (CTerrain*)CGameObject::FindObject("Terrain");
}

void CCameraController::LateUpdate()
{
	XMVECTOR rotate = target->transform->GetRotate();
	float yRadian = XMConvertToRadians(XMVectorGetY(rotate));

	XMVECTOR vPos = XMVectorSet(offsetDistance * sin(yRadian), 3.0f, offsetDistance * cos(yRadian), 0.0f) + target->transform->GetPosition();

	gameObject->transform->SetPosition(vPos);
	gameObject->transform->LookAt(*target->transform, &XMVectorSet(0, 1, 0, 0));
}