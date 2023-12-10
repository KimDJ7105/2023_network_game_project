#include "stdafx.h"
#include "PlayerController.h"
#include "GameFramework.h"
#include "CTankPlayer.h"

CPlayerController::CPlayerController(CGameObject* obj) : CComponent(obj)
{
	for (int i = 0; i < 5; i++)
		keyState[i] = false;
	for (int i = 0; i < 2; i++)
		mouseState[i] = false;

	prevTime = std::chrono::high_resolution_clock::now();
}

void CPlayerController::Start()
{
	playerID = ((CPlayer*)gameObject)->playerID;
	pivotObject = gameObject->root->GetChilde("Pivot");
	pivotObject->transform->SetRotate(0, 180.0, 0);
	tank = dynamic_cast<CTank*>(gameObject->root->GetChilde("Tank"));

	status = gameObject->GetChilde("Tank")->GetComponent<CStatus>();
}

void CPlayerController::Update()
{
	IsDie();

	//if (Define::Input->GetKeyDown(KeyCode::Space))
	//	tank->FireBullet(nullptr);

	KeyUpdate();
	//MouseRotate();

	auto currentTime = std::chrono::high_resolution_clock::now();

	auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime).count();

	if (keyState[KEY_SPACE] && elapsedTime > 1) {
		tank->FireBullet(nullptr);
		prevTime = currentTime;
	}

	MoveMent();

	Define::Players[playerID]->ClearInputEventList();
}

void CPlayerController::MoveMent()
{
	XMFLOAT3 dir = XMFLOAT3(0, 0, 0);

	if (keyState[KEY_W])
		dir.z += 1;
	if (keyState[KEY_S])
		dir.z -= 1;
	if (keyState[KEY_D])
		dir.x += 1;
	if (keyState[KEY_A])
		dir.x -= 1;
	//if (Define::Input->GetKeyDown(KeyCode::Q))
	//	status->speed += 1;
	//if (Define::Input->GetKeyDown(KeyCode::E))
	//	status->speed -= 1;

	tank->Move(dir, status->speed * Define::GameTimer->GetTimeElapsed());

	XMVECTOR pos = tank->transform->GetLocalPosition() + XMVectorSet(0, 2, 0, 0);
	pivotObject->transform->SetPosition(pos);
}

void CPlayerController::MouseRotate()
{
	if (mouseState[0])
	{
		float x = (float)mouseAxis.x * Define::GameTimer->GetTimeElapsed();
		//float x = (float)mouseAxis.x;
		//printf("y : %f\n", tank->upperBodyFrame->transform->GetPosition().m128_f32[2]);

		pivotObject->transform->Rotate(0.0f, x, 0.0f);
		tank->Rotate(0.0f, x, 0.0f);
	}

	if (mouseState[1])
	{
		float y = (float)mouseAxis.y / 3.0f;
		float roX = XMVectorGetX(pivotObject->transform->GetRotate()) + y;
		if (roX > 30.0f) roX = -30.0f;
		else if (roX < -30.0f) roX -= -30.0f;

		tank->Rotate(roX, 0.0f, 0.0f);
	}

	mouseAxis.x = mouseAxis.y = 0;

	//POINT mouseAxis = Define::Input->GetMouseAxis();
	//mouseAxis.x /= 3.0f;
	//mouseAxis.y /= 3.0f;

	//if (Define::Input->GetMousePress(MouseButton::Right))
	//{
	//	float roX = XMVectorGetX(pivotObject->transform->GetRotate()) + mouseAxis.y;
	//	if (roX > 30.0f) roX =- 30.0f;
	//	else if (roX < -30.0f) roX -= -30.0f;
	//}

	//if (Define::Input->GetMousePress(MouseButton::Left))
	//{
	//	pivotObject->transform->Rotate(0.0f, mouseAxis.x, 0.0f);
	//	tank->Rotate(0.0f, mouseAxis.x, 0.0f);
	//}
}

void CPlayerController::KeyUpdate()
{
	for (const auto input : *Define::Players[playerID]->GetInputEventList())
	{
		switch (input.state)
		{
		case KEY_DOWN:
			keyState[input.event_id] = true;
			break;
		case KEY_UP:
			keyState[input.event_id] = false;
			break;
		case MOUSE_DOWN:
			mouseState[input.event_id - 10] = true;
			mouseAxis = input.mouseAxis;
			MouseRotate();
			break;
		case MOUSE_UP:
			mouseState[input.event_id - 10] = false;
			break;
		default:
			break;
		}
	}
}

void CPlayerController::IsDie()
{
	if(status->CheckDie()) ::PostQuitMessage(0);
}
