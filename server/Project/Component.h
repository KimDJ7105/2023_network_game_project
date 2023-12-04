#pragma once

class CGameObject;
class CCollider;

class CComponent
{
public:
	CComponent(CGameObject* obj);
	~CComponent() {}

public:
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}

	virtual void OnCollision(CCollider& other) {}

	void SetActive(bool value);
	bool GetActive() { return isActive; }
	void ActiveUpdate(bool value);

	virtual void Enable() {}
	virtual void Disable() {}

public:
	CGameObject* gameObject;

private:
	bool isActive;
};

