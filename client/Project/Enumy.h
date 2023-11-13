#pragma once
#include "CTankPlayer.h"
#include "Timer.h"

struct IEnumeyState;

class CEnumy : public CGameObject
{
public:
	static float FindTargetRange;

public:
	CEnumy();
	~CEnumy();

public:
	CGameObject* target;
	CTank* tank = NULL;
	CStatus* status = new CStatus(this);

	bool isDie = false;

	float fireRate = 0;
	float reloadRate = 0;
	bool isOnShot = false;
	CTimer reloadTime;
	CTimer fireTime;

private: 
	IEnumeyState* currentState = NULL;
	std::stack<IEnumeyState*> stateStack;

public:
	virtual void Update();

	virtual void Collision(CCollider& other) override;

	void ChangeState(IEnumeyState* pushState);
	void PushState(IEnumeyState* pushState);
	void PopState();

public:
	void Die();
	void FindTarget(CGameObject& _Target);
};


struct IEnumeyState {
	virtual void Enter(CEnumy& _Enumy) = 0;
	virtual void Update(CEnumy& _Enumy) = 0;
	virtual void Exit(CEnumy& _Enumy) = 0;

	virtual void Resume(CEnumy& _Enumy) = 0;
	virtual void Wait(CEnumy& _Enumy) = 0;
};

namespace EnumyState 
{
	class Idle : public IEnumeyState
	{
		CTimer moveTime;
		XMFLOAT3 moveDirection;

		void Enter(CEnumy& _Enumy);
		void Update(CEnumy& _Enumy);
		void Exit(CEnumy& _Enumy);

		void Resume(CEnumy& _Enumy);
		void Wait(CEnumy& _Enumy);

		void SetRandomDir();
	};

	class Attack : public IEnumeyState
	{
		void Enter(CEnumy& _Enumy);
		void Update(CEnumy& _Enumy);
		void Exit(CEnumy& _Enumy);

		void Resume(CEnumy& _Enumy);
		void Wait(CEnumy& _Enumy);
	};

	class Die : public IEnumeyState
	{
		CTimer destoryTime;

		void Enter(CEnumy& _Enumy);
		void Update(CEnumy& _Enumy);
		void Exit(CEnumy& _Enumy);

		void Resume(CEnumy& _Enumy);
		void Wait(CEnumy& _Enumy);
	};
}
