#pragma once
#include "Component.h"

class CTerrain;

class CCameraController : public CComponent 
{
public:
	CCameraController(CGameObject* obj);
	~CCameraController() {}

public:
	virtual void Start();
	virtual void LateUpdate();

private:
	float offsetDistance;
	CGameObject* target;

	CTerrain* terrain;
};

