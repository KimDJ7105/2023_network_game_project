#pragma once
#include "Component.h"

class CStatus;
class CTank;

class CPlayerController : public CComponent
{
public:
	CPlayerController(CGameObject* obj);
	~CPlayerController() {}

public:
	virtual void Start();
	virtual void Update();

private:
	void MoveMent();
	void MouseRotate();

	void IsDie();

private:
	int playerID;
	CGameObject* pivotObject;
	CTank* tank;
	CStatus* status;
};