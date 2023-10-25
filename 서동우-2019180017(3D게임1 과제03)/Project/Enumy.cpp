#include "stdafx.h"
#include "Enumy.h"
#include "RigidBody.h"

float CEnumy::FindTargetRange = 20.0f;
CEnumy::CEnumy()
{
	CGameObject* myObj = (CGameObject*)this;

	name = "Enumy";
	tank = new CTank();
	tank->upperBodyFrame->collider->tag = name;
	tank->lowerBodyFrame->collider->tag = name;
	tank->gunFrame->collider->tag = name;
	tank->BulletInit(1);

	collider = new CCollider(this);
	for (int i = 0; i < tank->bulletIndex; i++)
	{
		//tank->bullets[i]->collider->tag = "Enumy Bullet";
		tank->bullets[i]->missile->collider->tag = "Enumy Bullet";
		tank->bullets[i]->missile->collider->CollisionCall = [](CCollider& obj, CCollider& other) {
			if (other.tag == "Player")
				dynamic_cast<CBulletObject*>(obj.gameObject->root)->Reset();
		};
	}
	myObj = tank;
	//SetChild(tank, true);

	float dis = fRandom(20, 30);
	float radian = XMConvertToRadians(fRandom(0, 360.0f));
	XMFLOAT3 pos = XMFLOAT3(dis * sin(radian), 0, dis * cos(radian));

	tank->transform->SetPosition(pos);

	ChangeState(new EnumyState::Idle);

	fireRate = 2.0f;
	reloadRate = 3.0f;

	tank->AddComponet(status);
	tank->AddComponet(new CRigidBody(tank));
}

CEnumy::~CEnumy()
{
}

void CEnumy::Update()
{
	CGameObject::Update();
	if (GetActive() == false) return;
	currentState->Update(*this);
}

void CEnumy::ChangeState(IEnumeyState* pushState)
{
	while (!stateStack.empty())
	{
		IEnumeyState* state = stateStack.top();
		state->Resume(*this);
		state->Exit(*this);

		stateStack.pop();
	}

	currentState = pushState;
	currentState->Enter(*this);

	stateStack.push(currentState);
}

void CEnumy::PushState(IEnumeyState* pushState)
{
	if(currentState)
		currentState->Wait(*this);
	currentState = pushState;
	currentState->Enter(*this);

	stateStack.push(currentState);
}

void CEnumy::PopState()
{
	stateStack.pop();
	currentState->Exit(*this);
	currentState = stateStack.top();
	currentState->Resume(*this);
}

void CEnumy::FindTarget(CGameObject& _Target)
{
	if (target)
		return;

	float distance = Vector3::Distance(_Target.transform->GetPosition(), tank->transform->GetPosition());
	if ( distance < FindTargetRange)
	{
		target = &_Target;
		PushState(new EnumyState::Attack);
	}
}

void CEnumy::Collision(CCollider& other)
{
	if (isDie) return;

	if (other.tag == "Player Bullet")
	{
		CBulletObject* p = dynamic_cast<CBulletObject*>(other.gameObject->root);
		status->hp -= p->status->damage;

		Die();
	}

	CGameObject::Collision(other);
}

void CEnumy::Die()
{
	if (status->hp > 0) return;
	if (isDie) return;

	isDie = true;
	tank->Die();
	ChangeState(new EnumyState::Die);
}

//---------------------------------------------

void EnumyState::Idle::Enter(CEnumy& _Enumy)
{
	_Enumy.status->speed = 2.5f;
	_Enumy.target = nullptr;
	_Enumy.tank->SetAllColor(0.0f, 0.9f, 0.0f);
	
	SetRandomDir();
	moveTime.Start();
}

void EnumyState::Idle::Update(CEnumy& _Enumy)
{
	// 주변 돌아다니게 하기
	// 패트롤 스폿 지정 해주기
	// 타겟 찾기

	moveTime.End();
	if (moveTime.GetTimer().count() > 3.0f)
	{
		SetRandomDir();
		moveTime.Start();
	}

	_Enumy.tank->Move(moveDirection, _Enumy.status->speed * Define::GameTimer->GetTimeElapsed());
	_Enumy.FindTarget(*((CTankPlayer*)Define::Player)->tank);
}

void EnumyState::Idle::Exit(CEnumy& _Enumy)
{
}

void EnumyState::Idle::Resume(CEnumy& _Enumy)
{
}

void EnumyState::Idle::Wait(CEnumy& _Enumy)
{
}

void EnumyState::Idle::SetRandomDir()
{
	XMFLOAT3 dir = XMFLOAT3(0,0,0);
	if (iRandom(0, 1) == 1) dir.z = iRandom(-1, 1) * 0.001f;
	else dir.x = iRandom(-1, 1) * 0.001f;
	
	moveDirection = dir;
}


//-----------

void EnumyState::Attack::Enter(CEnumy& _Enumy)
{
	_Enumy.tank->SetAllColor(0.8f, 0.0f, 0.0f);
}

void EnumyState::Attack::Update(CEnumy& _Enumy)
{
	float distance;
	XMStoreFloat(&distance ,XMVector3Length(_Enumy.target->transform->GetPosition() - _Enumy.tank->transform->GetPosition()));
	//타겟이 범위에 벗어나면 Idle로 돌아가기
	if (!_Enumy.target->GetActive() || distance > CEnumy::FindTargetRange)
	{
		//_Enumy.PopState();
		_Enumy.ChangeState(new Idle);
		return;
	}

	// 타겟이 있는 방향으로 회전
	// 타겟을 쏠때 회전 멈추기
	// 장전이 끝나면 일정시간이 지난 후 총 발사
	if (_Enumy.isOnShot)
	{
		_Enumy.tank->FireBullet(NULL);
		_Enumy.isOnShot = false;
		_Enumy.reloadTime.Start();
	}
	else
	{
		XMVECTOR lookPos = _Enumy.target->transform->GetPosition();
		lookPos = XMVectorSetY(lookPos, XMVectorGetY(_Enumy.tank->upperBodyFrame->transform->GetPosition()));
		_Enumy.tank->upperBodyFrame->transform->LookAt(lookPos);

		_Enumy.reloadTime.End();
		if (_Enumy.reloadTime.GetTimer().count() > _Enumy.fireRate)
		{
			_Enumy.isOnShot = true;
			_Enumy.fireTime.Start();
		}
	}

}

void EnumyState::Attack::Exit(CEnumy& _Enumy)
{
	_Enumy.target = nullptr;
}

void EnumyState::Attack::Resume(CEnumy& _Enumy)
{
}
void EnumyState::Attack::Wait(CEnumy& _Enumy)
{
}

//-------
void EnumyState::Die::Enter(CEnumy& _Enumy)
{
  	destoryTime.Start();
	_Enumy.tank->SetAllColor(0.1f, 0.1f, 0.1f);
}

void EnumyState::Die::Update(CEnumy& _Enumy)
{
	destoryTime.End();
	double nowTime = destoryTime.GetTimer().count();
	if (nowTime > 2.0f)
	{
		_Enumy.SetActive(false);
	}
	if (nowTime > 5.0f)
	{
		_Enumy.ChangeState(new EnumyState::Idle);
		_Enumy.SetActive(true);
	}
}

void EnumyState::Die::Exit(CEnumy& _Enumy)
{
	float dis = fRandom(20, 30);
	float radian = XMConvertToRadians(fRandom(0, 360.0f));
	XMVECTOR pos = Define::Player->transform->GetPosition() + XMVectorSet(dis * sin(radian), 0, dis * cos(radian), 1.0f);
	
    _Enumy.tank->transform->SetPosition(pos);
	_Enumy.isDie = false;
	_Enumy.status->hp = fRandom(10,50);

	_Enumy.tank->Reset();
}

void EnumyState::Die::Resume(CEnumy& _Enumy)
{
}

void EnumyState::Die::Wait(CEnumy& _Enumy)
{
}
