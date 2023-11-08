#include "stdafx.h"
#include "GameScene.h"
#include "Terrain.h"
#include "CameraController.h"
#include "Tree.h"
#include "CTankPlayer.h"
#include "Enumy.h"
#include "BigMissile.h"

CGameScene::CGameScene(int index) : CScene(index)
{
}

void CGameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	mainCamera = new CCamera();
	mainCamera->AddComponet(new CCameraController(mainCamera));

	CTerrain* terrain = new CTerrain("Image/terrain.raw");
	terrain->name = "Terrain";

	player = new CTankPlayer();

	for (int i = 0; i < 6; i++)
		new CEnumy();

	for (int i = 0; i < 10; i++)
		CTree* tree = new CTree();

	for (int i = 0; i < 3; ++i)
	{
		CGameObject* obj = CGameObject::LoadGeometryFromFile("Model/BigMissile.bin");
		obj->collider = new CCollider(obj);
		obj->collider->tag = "Big Missile";
		CBigMissile* bigMissile = new CBigMissile(obj);
		bigMissile->light = &m_pLights[i + 2];
		obj->AddComponet(bigMissile);
	}

	for (const auto& obj : Define::GameObjectList)
		obj->Start();
}

void CGameScene::AnimateObjects(float fTimeElapsed)
{
	CScene::AnimateObjects(fTimeElapsed);

	if (m_pLights)
	{
		XMStoreFloat3(&m_pLights[0].m_xmf3Position, player->tank->gunFrame->transform->GetPosition());
		XMStoreFloat3(&m_pLights[0].m_xmf3Position, XMLoadFloat3(&m_pLights[0].m_xmf3Position) + player->tank->gunFrame->transform->GetLook());
		XMStoreFloat3(&m_pLights[0].m_xmf3Direction, player->tank->gunFrame->transform->GetLook());
	}
}

void CGameScene::LoadScene()
{
	Define::MainCamera = mainCamera;
}