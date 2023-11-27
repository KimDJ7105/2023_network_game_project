//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#include "CSyncObjectManager.h"

CScene::CScene(int index)
{
	Define::SceneManager->AddScene(this, index);

	objectManager = new ObjectManager();
}

CScene::~CScene()
{
	delete objectManager;
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 6;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	for (int i = 0; i < 2; i++)
	{
		m_pLights[i].m_bEnable = true;
		m_pLights[i].m_nType = SPOT_LIGHT;
		m_pLights[i].m_fRange = 20.0f;
		m_pLights[i].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		m_pLights[i].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		m_pLights[i].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
		m_pLights[i].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_pLights[i].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
		m_pLights[i].m_fFalloff = 4.0f;
		m_pLights[i].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
		m_pLights[i].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	}

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);

	for (int i = 3; i < 6; i++)
	{
		m_pLights[i].m_bEnable = false;
		m_pLights[i].m_nType = SPOT_LIGHT;
		m_pLights[i].m_fRange = 1000.0f;
		m_pLights[i].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		m_pLights[i].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		m_pLights[i].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
		m_pLights[i].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_pLights[i].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
		m_pLights[i].m_fFalloff = 20.0f;
		m_pLights[i].m_fPhi = (float)cos(XMConvertToRadians(20.0f));
		m_pLights[i].m_fTheta = (float)cos(XMConvertToRadians(10.0f));
	}
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	//for (const auto& obj : Define::GameObjectList)
	//	obj->Release();
	//Define::GameObjectList.clear();

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	//for (const auto& obj : Define::GameObjectList)
	//	obj->ReleaseUploadBuffers();

	objectManager->AllReleaseUploadBuffers();
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	//{
	//	int createPackSize = 0;
	//	recv(Define::sock, (char*)createPackSize, sizeof(int), 0);
	//	for (int i = 0; i < createPackSize; i++)
	//	{
	//		sc_create_object_packet pack;
	//		recv(Define::sock, (char*)&pack, sizeof(sc_create_object_packet), 0);
	//		objectManager->AddCreatePack(pack);
	//	}
	//}

	//{
	//	int deletePackSize = 0;
	//	recv(Define::sock, (char*)deletePackSize, sizeof(int), 0);
	//	for (int i = 0; i < deletePackSize; i++)
	//	{
	//		sc_delete_object_packet pack;
	//		recv(Define::sock, (char*)&pack, sizeof(sc_create_object_packet), 0);
	//		objectManager->AddDeletePack(pack);
	//	}
	//}

	//{
	//	int transformPackSize = 0;
	//	recv(Define::sock, (char*)transformPackSize, sizeof(int), 0);
	//	for (int i = 0; i < transformPackSize; i++)
	//	{
	//		sc_object_transform_packet pack;
	//		recv(Define::sock, (char*)&pack, sizeof(sc_object_transform_packet), 0);
	//		objectManager->AddTransformPack(pack);
	//	}
	//}

	objectManager->AllCreatePackUpdate();
	objectManager->AllCreateObjectStart();

	objectManager->AllDeletePackUpdate();

	Define::SyncObjectManager->UpdateAllTransformPack();


	//for (const auto& collider : Define::ColliderList)
	//	collider->UpdateBoundingBox();

	//for (const auto& obj : Define::GameObjectList)
	//	obj->transform->UpdateTransform(NULL);
	//objectManager->AllGameObjectUpdateTransform();

	//objectManager->AllGameObjectUpdate();
	//objectManager->AllGameObjectLateUpdate();

	//for (const auto& obj : Define::GameObjectList)
	//	obj->Update();
	//for (const auto& obj : Define::GameObjectList)
	//	obj->LateUpdate();

	//for (auto colliderA : Define::ColliderList)
	//{
	//	if (colliderA->gameObject->GetActive() == false) continue;
	//	for (auto colliderB : Define::ColliderList)
	//	{
	//		if (colliderB->gameObject->GetActive() == false) continue;
	//		if (colliderA->tag == colliderB->tag) continue;
	//		if (colliderA == colliderB) continue;
	//		if (colliderA->gameObject->CheckCollision(*colliderB) == false) continue;

 //			if(colliderA->gameObject->root) colliderA->gameObject->root->Collision(*colliderB);
	//		else colliderA->gameObject->Collision(*colliderB);
	//	}
	//}
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	objectManager->AllGameObjectRender(pd3dCommandList, pCamera);
	//for (const auto& obj : Define::GameObjectList)
 //		obj->Render(pd3dCommandList, pCamera);
}