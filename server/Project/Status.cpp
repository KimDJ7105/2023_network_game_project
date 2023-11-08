#include "stdafx.h"
#include "Status.h"

CStatus::CStatus(CGameObject* obj) : CComponent(obj)
{
	hp = maxHP = 1;
	speed = 1;
	damage = 10;
	isDie = false;
}

CStatus::~CStatus()
{
}

bool CStatus::CheckDie()
{
	if (hp > 0) return isDie = false;
	else return isDie = true;
}
