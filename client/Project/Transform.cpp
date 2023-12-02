#include "stdafx.h"
#include "Object.h"
#include "Camera.h"

CTransform::CTransform(CGameObject* obj) : gameObject(obj)
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	scale = XMVectorSet(1, 1, 1, 1.0f);
	rotate = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
}

CTransform::~CTransform()
{
}

void CTransform::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (gameObject->m_pSibling) gameObject->m_pSibling->transform->UpdateTransform(pxmf4x4Parent);
	if (gameObject->m_pChild) gameObject->m_pChild->transform->UpdateTransform(&m_xmf4x4World);
}

void CTransform::ReSet()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	scale = XMVectorSet(1, 1, 1, 1.0f);
	rotate = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
}

void CTransform::MoveForward(float dis)
{
	SetPosition(GetPosition() + GetLook() * dis);
}

void CTransform::MoveUp(float dis)
{
	SetPosition(GetPosition() + GetUp() * dis);
}

void CTransform::MoveRight(float dis)
{
	SetPosition(GetPosition() + GetRight() * dis);
}

void CTransform::Rotate(float x, float y, float z)
{
	XMVECTOR ro = XMVectorSet(x, y, z, 1.0f);
	ro += rotate;

	SetRotate(ro);
}

void CTransform::LookAt(XMVECTOR& targetPosition, XMVECTOR* upVector)
{
	XMVECTOR myPos = GetPosition();
	XMVECTOR targetPos = targetPosition;
	XMVECTOR up = XMVectorZero();
	if (upVector == nullptr) up = GetUp();
	else up = *upVector;

	XMVECTOR lookDirection = XMVectorSubtract(targetPosition, myPos);
	lookDirection = XMVector3Normalize(lookDirection);

	XMVECTOR rightDirection = XMVector3Cross(up, lookDirection);
	rightDirection = XMVector3Normalize(rightDirection);

	up = XMVector3Cross(lookDirection, rightDirection);
	up = XMVector3Normalize(up);

	XMMATRIX viewMatrix;
	viewMatrix.r[0] = rightDirection;
	viewMatrix.r[1] = up;
	viewMatrix.r[2] = lookDirection;
	viewMatrix.r[3] = GetLocalPosition();
	XMFLOAT4X4 lookMatrix;
	XMStoreFloat4x4(&lookMatrix, viewMatrix);
	m_xmf4x4Transform = lookMatrix;

	if (gameObject->root) gameObject->root->transform->UpdateTransform(NULL);
	else UpdateTransform(NULL);
}

XMVECTOR CTransform::GetPosition()
{
	return XMVectorSet(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43, 1.0f);
}

XMVECTOR CTransform::GetRotate()
{
	XMFLOAT3 rotation;

	// 행렬에서 회전 값을 추출
	rotation.x = XMConvertToDegrees(atan2f(m_xmf4x4World._32, m_xmf4x4World._33));
	rotation.y = XMConvertToDegrees(atan2f(-m_xmf4x4World._31, sqrtf(m_xmf4x4World._32 * m_xmf4x4World._32 + m_xmf4x4World._33 * m_xmf4x4World._33)));
	rotation.z = XMConvertToDegrees(atan2f(m_xmf4x4World._21, m_xmf4x4World._11));

	return XMLoadFloat3(&rotation);
}

XMVECTOR CTransform::GetLook()
{
	return XMVector3Normalize(XMVectorSet(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33, 1.0f));
}

XMVECTOR CTransform::GetUp()
{
	return XMVector3Normalize(XMVectorSet(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23, 1.0f));
}

XMVECTOR CTransform::GetRight()
{
	return XMVector3Normalize(XMVectorSet(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13, 1.0f));
}

void CTransform::SetPosition(XMVECTOR& xmf3Position)
{
	position = xmf3Position;
	position.m128_f32[3] = 1;

	m_xmf4x4Transform._41 = XMVectorGetX(position);
	m_xmf4x4Transform._42 = XMVectorGetY(position);
	m_xmf4x4Transform._43 = XMVectorGetZ(position);

	if (gameObject->root) gameObject->root->transform->UpdateTransform(NULL);
	else UpdateTransform(NULL);
}

void CTransform::SetRotate(XMVECTOR& RotateVector)
{
	XMVECTOR ro = RotateVector - rotate;
	rotate += ro;
	rotate.m128_f32[3] = 1;

	float xRadian = XMConvertToRadians(XMVectorGetX(ro));
	float yRadian = XMConvertToRadians(XMVectorGetY(ro));
	float zRadian = XMConvertToRadians(XMVectorGetZ(ro));
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(xRadian, yRadian, zRadian);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	if (gameObject->root) gameObject->root->transform->UpdateTransform(NULL);
	else UpdateTransform(NULL);
}

void CTransform::SetScale(XMVECTOR& Scale)
{
	scale = Scale;
	scale.m128_f32[3] = 1;

	XMMATRIX mtxScale = XMMatrixScaling(XMVectorGetX(scale), XMVectorGetY(scale), XMVectorGetZ(scale));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	if (gameObject->root) gameObject->root->transform->UpdateTransform(NULL);
	else UpdateTransform(NULL);
}