#pragma once

class CGameObject;

class CTransform
{
public:
	CTransform(CGameObject* obj);
	~CTransform();

	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);

public:
	void ReSet();

	void MoveForward(float dis);
	void MoveUp(float dis);
	void MoveRight(float dis);

	void Rotate(float x, float y, float z);

	void LookAt(XMVECTOR& targetPosition, XMVECTOR* upVector = nullptr);
	void LookAt(CTransform& targetTransform, XMVECTOR* upVector = nullptr) { LookAt(targetTransform.GetPosition(), upVector); }

public: // getter setter
	XMVECTOR GetPosition();
	XMVECTOR GetLocalPosition() { return position; }

	XMVECTOR GetScale() { return scale; }

	XMVECTOR GetRotate() { return rotate; }
	//XMVECTOR GetRotate();

	XMVECTOR GetLook();
	XMVECTOR GetUp();
	XMVECTOR GetRight();

	void SetPosition(XMVECTOR& xmf3Position);
	void SetPosition(XMFLOAT3& xmf3Position) { SetPosition(XMLoadFloat3(&xmf3Position)); }
	void SetPosition(float x, float y, float z) { SetPosition(XMVectorSet(x, y, z, 1.0f)); }

	void SetRotate(XMVECTOR& RotateVector);
	void SetRotate(XMFLOAT3& RotateVector) { SetRotate(XMLoadFloat3(&RotateVector)); }
	void SetRotate(float x, float y, float z) {SetRotate(XMVectorSet(x, y, z, 1.0f));}

	void SetScale(XMVECTOR& Scale);
	void SetScale(XMFLOAT3& Scale) { SetScale(XMLoadFloat3(&Scale)); }
	void SetScale(float x, float y, float z) { SetScale(XMVectorSet(x, y, z, 1.0f)); }

public:
	CGameObject* gameObject = NULL;

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Transform;

private:
	XMVECTOR position;
	XMVECTOR rotate;
	XMVECTOR scale;
};