#pragma once
#include "Component.h"

class CStatus : public CComponent
{
public:
	CStatus(CGameObject* obj);
	~CStatus();

public:
	float maxHP;

	float hp;
	
	float speed;
	float damage;

	bool isDie;

public:
	bool CheckDie();
};

