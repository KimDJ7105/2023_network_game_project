#include "stdafx.h"
#include "Collider.h"
#include "Object.h"

CCollider::CCollider(CGameObject* obj) : CComponent(obj)	
{
	tag = "None";

	bound = new BoundingOrientedBox();
	if(obj->m_pMesh) bound->Extents = obj->m_pMesh->m_xmf3AABBExtents;

	pivot = XMVectorSet(0, 0, 0, 0);
	Define::ColliderList.emplace_back(this);
}

void CCollider::UpdateBoundingBox()
{
	if (gameObject->GetActive() == false) return;

	if (bound)
	{
		XMStoreFloat3(&bound->Center, gameObject->transform->GetPosition() + pivot);
		XMVECTOR s = (gameObject->GetMesh()) ? XMLoadFloat3(&gameObject->GetMesh()->m_xmf3AABBExtents) : gameObject->transform->GetScale();
		XMStoreFloat3(&bound->Extents, gameObject->transform->GetScale() * s);
		XMStoreFloat4(&bound->Orientation, XMQuaternionNormalize(XMLoadFloat4(&bound->Orientation)));
	}

	if (gameObject->m_pSibling && gameObject->m_pSibling->collider) gameObject->m_pSibling->collider->UpdateBoundingBox();
	if (gameObject->m_pChild && gameObject->m_pChild->collider) gameObject->m_pChild->collider->UpdateBoundingBox();
}