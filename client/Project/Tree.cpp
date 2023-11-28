#include "stdafx.h"
#include "Tree.h"
#include "CTankPlayer.h"

CTree::CTree()
{
	name = "Tree";

	CGameObject* obj = LoadGeometryFromFile("Model/Tree.bin");
	SetChild(obj, true);
	obj->collider = new CCollider(obj);
	obj->m_pMesh->m_xmf3AABBExtents = XMFLOAT3(1, obj->m_pMesh->m_xmf3AABBExtents.y, 1);
	obj->collider->tag = name;
	obj->collider->SetPivot(0,obj->collider->bound->Extents.y,0);

	float dis = fRandom(10.0f, 100.0f);
	float radian = XMConvertToRadians(fRandom(0, 360));
	transform->SetPosition(dis * sin(radian), 0.0f, dis * cos(radian));
	
	isDie = false;
}

void CTree::Collision(CCollider& other)
{
	if (other.tag == "Player" || other.tag == "Enumy")
	{
		if (isDie == true) return;

		transform->LookAt(*other.gameObject->root->FindFrame("Lower_Body")->transform);
		dieTimer.Start();
		isDie = true;
	}
}


void CTree::Update()
{
	if (GetActive() == false)
	{
		respawnTimer.End();
		if (respawnTimer.GetTimer().count())
		{
			float dis = fRandom(10.0f, 100.0f);
			float radian = XMConvertToRadians(fRandom(0, 360));
			transform->SetPosition(dis * sin(radian), 0.0f, dis * cos(radian));
			SetActive(true);
		}
	}

	DieAnimation();
}

void CTree::DieAnimation()
{
	if (isDie == false) return;

	dieTimer.End();
	float x = XMVectorGetX(transform->GetRotate());
	if (dieTimer.GetTimer().count() > 10.0f)
	{
		isDie = false;
		transform->Rotate(-x, 0, 0);
		SetActive(false);
		respawnTimer.Start();
	}

	if (abs(x) < 80.0f)
		transform->Rotate(-0.1f, 0, 0);

}