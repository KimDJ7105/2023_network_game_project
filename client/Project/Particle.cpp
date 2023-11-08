#include "stdafx.h"
#include "Particle.h"
#include "Shader.h"

CParticleSystem::CParticleSystem(int index) : particleIndex(index)
{
	name = "Particle";
	CGameObject* obj = LoadGeometryFromFile("Model/Particle_Cube.bin");
	particleObjects = new CGameObject *[ObjectIndex];
	particleObjects[0] = obj;
	for (int i = 0; i < ObjectIndex; i++) {
		particleObjects[i]->ReleaseUploadBuffers();
		auto p = find(Define::GameObjectList.begin(), Define::GameObjectList.end(), particleObjects[i]);
		if (p != Define::GameObjectList.end()) Define::GameObjectList.erase(p);
	}

	//particles = new CGameObject * [particleIndex];

	//for (int i = 0; i < particleIndex; i++)
	//{
	//	CGameObject* obj = LoadGeometryFromFile("Model/Particle_Cube.bin");
	//	SetChild(obj);
	//	
	//	particles[i] = obj;
	//}

	transforms = new CTransform * [particleIndex];
	for (int i = 0; i < particleIndex; i++) transforms[i] = new CTransform(this);

	angle = XMFLOAT3(0, 0, 0);

	isLoop = false;

	minSpeed = 1;
	maxSpeed = 1;
}

CParticleSystem::~CParticleSystem()
{
}

void CParticleSystem::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (GetActive() == false) return;

	for (int i = 0; i < particleIndex; i++)
	{
		particleObjects[0]->UpdateShaderVariable(pd3dCommandList, &transforms[i]->m_xmf4x4World);

		for (int i = 0; i < ObjectIndex; i++)
		{
			if (particleObjects[i]->m_ppMaterials[0])
			{
				if (particleObjects[i]->m_ppMaterials[0]->m_pShader) particleObjects[i]->m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera);
				particleObjects[i]->m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);
			}

			if (particleObjects[i]->m_pMesh) particleObjects[i]->m_pMesh->Render(pd3dCommandList, 0);
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void CParticleSystem::Update()
{
	CGameObject::Update();

	if (GetActive() == false) return;

	float x = XMVectorGetX(transform->GetRotate());

	//for (int i = 0; i < particleIndex; i++)
	//{
	//	particles[i]->transform->MoveUp(fRandom(minSpeed, maxSpeed));
	//	if (XMVectorGetX(particles[i]->transform->GetRotate()) + x < 180.0f) particles[i]->transform->Rotate(iRandom(1, 2), 0, 0);
	//}

	float deltaTime = Define::GameTimer->GetTimeElapsed();

	for (int i = 0; i < particleIndex; i++)
	{
		transforms[i]->MoveUp(fRandom(minSpeed, maxSpeed) * deltaTime);
		if (XMVectorGetX(transforms[i]->GetRotate()) + x < 180.0f) transforms[i]->Rotate(iRandom(1, 2), 0, 0);
 		transforms[i]->UpdateTransform(&transform->m_xmf4x4World);
	}

	lifeTime.End();
	if (!isLoop && lifeTime.GetTimer().count() > duration)
 		Stop();
}

void CParticleSystem::Play()
{
	//Rotate(angle);
	for (int i = 0; i < particleIndex; i++)
		transforms[i]->SetRotate(0, fRandom(0.0f, 360.0f), 0);

	lifeTime.Start();
	SetActive(true);
}

void CParticleSystem::Stop()
{
	transform->ReSet();
	for (int i = 0; i < particleIndex; i++)
		transforms[i]->ReSet();

	SetActive(false);
}

void CParticleSystem::SetAngle(XMFLOAT3& _Angle)
{
	angle = _Angle;
	//Rotate(angle);
}

void CParticleSystem::SetSpeed(float min, float max)
{
	minSpeed = min;
	maxSpeed = max;
}
