#include "stdafx.h"
#include "PlayerController.h"
#include "GameFramework.h"
#include "Tank.h"

CPlayerController::CPlayerController(CGameObject* obj) : CComponent(obj)
{
}

void CPlayerController::Start()
{
	pivotObject = gameObject->root->GetChilde("Pivot");
	pivotObject->transform->SetRotate(0, 180.0, 0);
	tank = dynamic_cast<CTank*>(gameObject->root->GetChilde("Tank"));

	status = gameObject->GetChilde("Tank")->GetComponet<CStatus>();
}

void CPlayerController::Update()
{
	IsDie();

	if (Define::Input->GetKeyDown(KeyCode::Space))
		tank->FireBullet(nullptr);

	MouseRotate();
	MoveMent();
}

void CPlayerController::MoveMent()
{
	XMFLOAT3 dir = XMFLOAT3(0, 0, 0);
	if (Define::Input->GetKeyPress(KeyCode::W))
		dir.z += 1;
	if (Define::Input->GetKeyPress(KeyCode::S))
		dir.z -= 1;
	if (Define::Input->GetKeyPress(KeyCode::D))
		dir.x += 1;
	if (Define::Input->GetKeyPress(KeyCode::A))
		dir.x -= 1;

	//if (Define::Input->GetKeyPress(KeyCode::W))
	//	Define::RecvInputPack(KEY_UP);
	//if (Define::Input->GetKeyPress(KeyCode::S))
	//	Define::RecvInputPack(KEY_DOWN);
	//if (Define::Input->GetKeyPress(KeyCode::D))
	//	Define::RecvInputPack(KEY_RIGHT);
	//if (Define::Input->GetKeyPress(KeyCode::A))
	//	Define::RecvInputPack(KEY_LEFT);

	if (Define::Input->GetKeyDown(KeyCode::Q))
		status->speed += 1;
	if (Define::Input->GetKeyDown(KeyCode::E))
		status->speed -= 1;

	tank->Move(dir, status->speed * Define::GameTimer->GetTimeElapsed());

	XMVECTOR pos = tank->transform->GetLocalPosition() + XMVectorSet(0, 2, 0, 0);
	pivotObject->transform->SetPosition(pos);
}

void CPlayerController::MouseRotate()
{
	POINT mouseAxis = Define::Input->GetMouseAxis();
	mouseAxis.x /= 3.0f;
	mouseAxis.y /= 3.0f;

	//if (Define::Input->GetMousePress(MouseButton::Right))
	//{
	//	float roX = XMVectorGetX(pivotObject->transform->GetRotate()) + mouseAxis.y;
	//	if (roX > 30.0f) roX =- 30.0f;
	//	else if (roX < -30.0f) roX -= -30.0f;
	//}
	if (Define::Input->GetMousePress(MouseButton::Left))
	{
		pivotObject->transform->Rotate(0.0f, mouseAxis.x, 0.0f);
		tank->Rotate(0.0f, mouseAxis.x, 0.0f);
	}
}

void CPlayerController::IsDie()
{
	if(status->CheckDie()) ::PostQuitMessage(0);
}
