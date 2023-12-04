#include "stdafx.h"
#include "Component.h"
#include "Object.h"
#include "SceneManager.h"
#include "Scene.h"

CComponent::CComponent(CGameObject* obj) : gameObject(obj)
{
	isActive = true;
}

void CComponent::SetActive(bool value)
{
	Define::SceneManager->GetCurrentScene()->objectManager->AddActiveObject(gameObject, value);
}

void CComponent::ActiveUpdate(bool value)
{
	if (isActive == true && value == false)
		Disable();
	else if (isActive == false && value == true)
		Enable();
	isActive = value;

	if (gameObject->m_pSibling) gameObject->m_pSibling->SetActive(value);
	if (gameObject->m_pChild) gameObject->m_pChild->SetActive(value);
}
