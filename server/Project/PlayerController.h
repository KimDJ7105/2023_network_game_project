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
	void KeyUpdate();

	void IsDie();

private:
	int playerID;
	CGameObject* pivotObject;
	CTank* tank;
	CStatus* status;

	bool keyState[5];
};