#pragma once
#include "Object.h"
#include "Timer.h"

class CTree : public CGameObject
{
public:
	CTree();
	virtual ~CTree(){}

public:
	virtual void Update();
	virtual void Collision(CCollider& other);

	void DieAnimation();

private:
	bool isDie;
	CTimer dieTimer;
	CTimer respawnTimer;
};