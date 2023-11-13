#pragma once
#include "Scene.h"
#include "ObjectManager.h"

class CTankPlayer;

class CGameScene : public CScene
{
public:
	CGameScene(int index);
	~CGameScene(){}

public:
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void LoadScene();

public:
	CCamera* mainCamera;

private:
	CTankPlayer* localPlayer;
	CTankPlayer* remotePlayer;
};
