#include "stdafx.h"
#include "Tank.h"

CTank::CTank()
{
	name = "Tank";

	CGameObject* obj = LoadGeometryFromFile("Model/Tank.bin");

	SetChild(obj, true);

	upperBodyFrame = FindFrame("TankFree_Tower");
	lowerBodyFrame = FindFrame("Lower_Body");
	gunFrame = FindFrame("TankFree_Canon");
	upperBodyFrame->name = "Uppder Body";
	lowerBodyFrame->name = "Lower Body";
	gunFrame->name = "Gun";
	upperBodyFrame->collider = new CCollider(upperBodyFrame);
	lowerBodyFrame->collider = new CCollider(lowerBodyFrame);
	gunFrame->collider = new CCollider(gunFrame);

	wheelParent = FindFrame("Wheels");
	wheelParent->name = "Wheels";
	wheelsFrame[0] = FindFrame("TankFree_Wheel_f_left");
	wheelsFrame[1] = FindFrame("TankFree_Wheel_f_right");
	wheelsFrame[2] = FindFrame("TankFree_Wheel_b_left");
	wheelsFrame[3] = FindFrame("TankFree_Wheel_b_right");
	for (int i = 0; i < 4; i++)
		wheelsFrame[i]->collider = new CCollider(wheelsFrame[i]);

	transform->SetPosition(0, 2, 0);
}

void CTank::Move(const XMFLOAT3& xmf3Shift, const float& speed)
{
	if (Vector3::Distance(XMFLOAT3(xmf3Shift)) == 0.0f) return;

	float s = speed;
	if(xmf3Shift.z < 0) s *= -1;

	transform->SetPosition(transform->GetLocalPosition() + lowerBodyFrame->transform->GetLook() * s);
	LowerBodyRotate(xmf3Shift);

	float wheelRotate;
	if (xmf3Shift.z < 0) wheelRotate = -1;
	else wheelRotate = 1;
	for (int i = 0; i < 4; ++i)
		wheelsFrame[i]->transform->Rotate(wheelRotate, 0, 0);
}

void CTank::Rotate(float x, float y, float z)
{
	if (x != 0.0f)
	{
		gunFrame->transform->Rotate(x, 0, 0);
	}
	if (y != 0.0f)
	{
		y_upper += y;
		if (y_upper > 360.0f) y_upper -= 360.0f;
		if (y_upper < 0.0f)  y_upper += 360.0f;
		upperBodyFrame->transform->Rotate(0, y, 0);
	}
}

void CTank::BulletInit(int index)
{
	bulletIndex = index;
	bullets = new CBulletObject * [bulletIndex];
	for (int i = 0; i < bulletIndex; i++)
		bullets[i] = new CBulletObject();
}

void CTank::FireBullet(CGameObject* pLockedObject)
{
	CBulletObject* pBulletObject = NULL;
	for (int i = spawnBulletIndex; i < bulletIndex; i++)
	{
		if (bullets[i]->GetActive() == false)
		{
			pBulletObject = bullets[i];
			spawnBulletIndex = i + 1;
			break;
		}
	}
	if (spawnBulletIndex == bulletIndex) spawnBulletIndex = 0;

	if (pBulletObject)
	{
		CGameObject* gun = gunFrame;

		pBulletObject->transform->m_xmf4x4Transform = gun->transform->m_xmf4x4World;
		pBulletObject->transform->SetPosition(gun->transform->GetPosition() + gun->transform->GetLook());

		pBulletObject->SetMoveDirectionSpeed(20.0f);
		pBulletObject->Fire();
	}
}

void CTank::LowerBodyRotate(const XMFLOAT3& xmf3Shift)
{
	float y = 0;

	if (xmf3Shift.z > 0)
	{
		if (IsUpperLeftLower(y_Lower, y_upper)) // 哭率
		{
			y -= 0.5f;
			y_Lower += y;
			if (y_Lower < 0) y_Lower += 360.0f;
		}
		else// 坷弗率
		{
			y += 0.5f;
			y_Lower += y;
			if (y_Lower > 360.0f) y_Lower -= 360.0f;
		}
	}
	else if(xmf3Shift.z < 0)
	{
		float fmodY_Lower = y_Lower + 180.0f;
		if (fmodY_Lower > 360.0f) fmodY_Lower -= 360.0f;
		float fmodY_Upper = y_upper + 180.0f;
		if (fmodY_Upper > 360.0f) fmodY_Upper -= 360.0f;
		if (IsUpperLeftLower(fmodY_Lower, fmodY_Upper)) // 哭率
		{
			y -= 0.5f;
			y_Lower += y;
			if (y_Lower < 0) y_Lower += 360.0f;
		}
		else// 坷弗率
		{
			y += 0.5f;
			y_Lower += y;
			if (y_Lower > 360.0f) y_Lower -= 360.0f;
		}
	}
	else if (xmf3Shift.x > 0)
	{
		float fmodY_Upper = y_upper + 90.0f;
		if (fmodY_Upper > 360.0f) fmodY_Upper -= 360.0f;
		if (IsUpperLeftLower(y_Lower, fmodY_Upper)) // 哭率
		{
			y -= 0.5f;
			y_Lower += y;
			if (y_Lower < 0) y_Lower += 360.0f;
		}
		else// 坷弗率
		{
			y += 0.5f;
			y_Lower += y;
			if (y_Lower > 360.0f) y_Lower -= 360.0f;
		}
	}
	else if (xmf3Shift.x < 0)
	{
		float fmodY_Upper = y_upper - 90.0f;
		if (fmodY_Upper < 0.0f) fmodY_Upper += 360.0f;
		if (IsUpperLeftLower(y_Lower, fmodY_Upper)) // 哭率
		{
			y -= 0.5f;
			y_Lower += y;
			if (y_Lower < 0) y_Lower += 360.0f;
		}
		else// 坷弗率
		{
			y += 0.5f;
			y_Lower += y;
			if (y_Lower > 360.0f) y_Lower -= 360.0f;
		}
	}

	lowerBodyFrame->transform->Rotate(0, y, 0);
	wheelParent->transform->Rotate(0, y, 0);
}

void CTank::Reset()
{
	wheelsFrame[0]->transform->Rotate(0, 0, +60);
	wheelsFrame[1]->transform->Rotate(0, 0, -60);
	wheelsFrame[2]->transform->Rotate(0, 0, +60);
	wheelsFrame[3]->transform->Rotate(0, 0, -60);

	lowerBodyFrame->transform->SetPosition(lowerBodyFrame->transform->GetLocalPosition() + XMVectorSet(0, +0.15f, 0, 1.0f));
	lowerBodyFrame->transform->Rotate(+5.0f, 0.0f, 0.0f);

	upperBodyFrame->transform->SetPosition(upperBodyFrame->transform->GetLocalPosition() + XMVectorSet(0, +0.13f, 0,1.0f));
	upperBodyFrame->transform->Rotate(+5.0f, 0.0f, 0.0f);

	gunFrame->transform->Rotate(-30, 0, 0);
}

void CTank::Die()
{
	wheelsFrame[0]->transform->Rotate(0, 0, -60);
	wheelsFrame[1]->transform->Rotate(0, 0, +60);
	wheelsFrame[2]->transform->Rotate(0, 0, -60);
	wheelsFrame[3]->transform->Rotate(0, 0, +60);

	lowerBodyFrame->transform->SetPosition(lowerBodyFrame->transform->GetLocalPosition() + XMVectorSet(0, -0.15f, 0,1.0f));
	lowerBodyFrame->transform->Rotate(-5.0f,0.0f,0.0f);

	upperBodyFrame->transform->SetPosition(upperBodyFrame->transform->GetLocalPosition() + XMVectorSet(0, -0.13f, 0,1.0f));
	upperBodyFrame->transform->Rotate(-5.0f, 0.0f, 0.0f);

	gunFrame->transform->Rotate(30, 0, 0);
}

bool CTank::IsUpperLeftLower(float a, float b)
{
	// true 哭率 flase 坷弗率
	if (a == 0.0f || a == 360.0f)
	{
		if (b > 0.0f && b < 180.0f)
			return false;
		else
			return true;
	}
	else if (a == 180.0f)
	{
		if (b < 180.0f)
			return true;
		else
			return false;
	}
	else if (a > 0.0f && a < 180.0f)
	{
		if (b < 180.0f + a && b > a) return false;
		else return true;
	}
	else
	{
		if (a - 180.0f < b && b < a) return true;
		else return false;
	}
}
