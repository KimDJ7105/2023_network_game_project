#include "stdafx.h"
#include "Terrain.h"
#include "GameFramework.h"
#include "Player.h"
#include "Shader.h"

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	//파일을 열고 읽는다. 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다. 
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x + (y * m_nWidth)];
		}
	}
	if (pHeightMapPixels) delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터이다. 
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));
	/*높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한 픽셀 값을 사용하여 법선 벡터를
	계산한다.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	//(x, z), (x+1, z), (z, z+1)의 픽셀에서 지형의 높이를 구한다. 
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	//xmf3Edge1은 (0, y3, m_xmf3Scale.z) - (0, y1, 0) 벡터이다. 
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	//xmf3Edge2는 (m_xmf3Scale.x, y2, 0) - (0, y1, 0) 벡터이다.
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	//법선 벡터는 xmf3Edge1과 xmf3Edge2의 외적을 정규화하면 된다.
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);
	return(xmf3Normal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMapImage::GetHeight(float fx, float fz)
{
	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다. 높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 높이는
	0이다.*/
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다. 
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-좌표가 1, 3, 5, ...인 경우 인덱스가 오른쪽에서 왼쪽으로 나열된다. 
	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE* dsb = nullptr;
	if (dsb == nullptr)
	{
		dsb = &CShader::ReadCompiledShaderFromFile(L"VSShaders.cso", ppd3dShaderBlob);
	}
	return *dsb;
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::ReadCompiledShaderFromFile(L"PSShaders.cso", ppd3dShaderBlob));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	int xStart, int zStart, int nWidth, int nLength, 
	XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext) : CMesh()
{
	//격자의 교점(정점)의 개수는 (nWidth * nLength)이다.
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(XMFLOAT3);
	//격자는 삼각형 스트립으로 구성한다. 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	CVertex* pVertices = new CVertex[m_nVertices];
	CVertex* pNormals = new CVertex[m_nVertices];
	CDiffusedVertex* xmf4Colors = new CDiffusedVertex[m_nVertices];
	/*xStart와 zStart는 격자의 시작 위치(x-좌표와 z-좌표)를 나타낸다. 커다란 지형은 격자들의 이차원 배열로 만들 필
	요가 있기 때문에 전체 지형에서 각 격자의 시작 위치를 나타내는 정보가 필요하다.*/
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;

	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//정점의 높이와 색상을 높이 맵으로부터 구한다. 
			XMFLOAT3 xmf3Position = XMFLOAT3((x * m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z * m_xmf3Scale.z));
			XMFLOAT4 xmf3Color = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			xmf3Color.w = 1;
			pVertices[i] = CVertex(xmf3Position);
			pNormals[i] = CVertex(XMFLOAT3(0, 0, 0));
			xmf4Colors[i] = CDiffusedVertex(xmf4Color);
			//xmf4Colors[i] = CDiffusedVertex(XMFLOAT4(0, 0, 0, 1));
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}
	
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	delete[] pVertices;

	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pNormals,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, 
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

	m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	delete[] pNormals;

	m_pd3dColorBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, xmf4Colors,
		sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dColorUploadBuffer);

	m_d3dColorBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_d3dColorBufferView.StrideInBytes = sizeof(XMFLOAT4);
	m_d3dColorBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
	delete[] xmf4Colors;

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	UINT* pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다.
			for (int x = 0; x < nWidth; x++)
			{
				//첫 번째 줄을 제외하고 줄이 바뀔 때마다(x == 0) 첫 번째 인덱스를 추가한다. 
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다. 
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다. 
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (nWidth-1)) 첫 번째 인덱스를 추가한다. 
				if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래(x, z), 위(x, z+1)의 순서로 인덱스를 추가한다. 
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	delete[] pnIndices;
}
CHeightMapGridMesh::~CHeightMapGridMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();

	if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
	if (m_pd3dColorUploadBuffer) m_pd3dColorUploadBuffer->Release();

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CHeightMapGridMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;
	if (m_pd3dColorUploadBuffer) m_pd3dColorUploadBuffer->Release();
	m_pd3dColorUploadBuffer = NULL;
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}

void CHeightMapGridMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[3] = { m_d3dVertexBufferView, m_d3dNormalBufferView, m_d3dColorBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 3, pVertexBufferViews);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext) 
{
	//조명의 방향 벡터(정점에서 조명까지의 벡터)이다.
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	//조명의 색상(세기, 밝기)이다. 
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);
	float fScale = Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x + 1, z + 1), xmf3LightDirection);
	fScale += Vector3::DotProduct(pHeightMapImage->GetHeightMapNormal(x, z + 1), xmf3LightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;
	//fScale은 조명 색상(밝기)이 반사되는 비율이다. 
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}

CTerrain::CTerrain(string filePath)
{
	m_nWidth = m_nLength = 257;
	int nBlockWidth = 17; int nBlockLength = 17;
	m_nMeshes = 1;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	//지형에 사용할 높이 맵을 생성한다.
	XMFLOAT3 scale;
	XMStoreFloat3(&scale, transform->GetScale());

	wstring wfilePath{ filePath.begin(), filePath.end() };
	m_pHeightMapImage = new CHeightMapImage((LPCTSTR)wfilePath.c_str(), m_nWidth, m_nLength, scale);
	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	//지형 전체를 표현하기 위한 격자 메쉬의 개수이다.
	m_nMeshes = cxBlocks * czBlocks;
	//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다. 
	m_ppMeshes = new CMesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;
	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다. 
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다. 
			pHeightMapGridMesh = new CHeightMapGridMesh(Define::Framework->m_pd3dDevice, Define::Framework->m_pd3dCommandList, 
				xStart, zStart, nBlockWidth, nBlockLength, scale, XMFLOAT4(0, 1.0f, 1.0f, 1.0f), m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	transform->SetPosition(XMFLOAT3(GetWidth() * -0.5f, 0, GetLength() * -0.5f));
	CTerrainShader* pShader = new CTerrainShader();
	pShader->CreateShader(Define::Framework->m_pd3dDevice, Define::Framework->m_pd3dCommandList, Define::SceneManager->GetCurrentScene()->m_pd3dGraphicsRootSignature);
	SetShader(pShader);

	collider = new CCollider(this);
	collider->tag = "Terrain";
}

CTerrain::~CTerrain(void)
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}

	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

void CTerrain::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	UpdateShaderVariable(pd3dCommandList, &transform->m_xmf4x4World);
	if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	//게임 객체가 포함하는 모든 메쉬를 렌더링한다. 
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[0])
			{
				m_ppMaterials[0]->UpdateShaderVariable(pd3dCommandList);
			}
		}

		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, i);
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void CTerrain::Update()
{
	for (const auto& obj : *Define::SceneManager->GetCurrentScene()->objectManager->GetList())
	{
		if (typeid(*obj) == typeid(CTerrain)) continue;
		if (typeid(*obj) == typeid(CCamera)) continue;

		TerrainCollision(obj);
	}
}

void CTerrain::TerrainCollision(CGameObject* obj)
{
	if (obj->GetActive() == false) return;

	if (obj->collider && obj->collider->GetActive())
	{
		if (obj->m_pMesh)
		{
			XMFLOAT3 ObjectPos;
			float fHeight;

			XMStoreFloat3(&ObjectPos, obj->transform->GetPosition());
			fHeight = GetHeight(ObjectPos.x, ObjectPos.z);
			fHeight -= XMVectorGetY(transform->GetPosition());

			if (ObjectPos.y - obj->collider->bound->Extents.y < fHeight)
			{
				XMVECTOR pos = XMVectorSet(0, fHeight + obj->collider->bound->Extents.y - (XMVectorGetY(obj->transform->GetPosition() + obj->collider->GetPivot())), 0, 0);
				if (obj->root) obj->root->transform->SetPosition(pos + obj->root->transform->GetPosition());
				else obj->transform->SetPosition(pos + obj->transform->GetPosition());

				obj->Collision(*collider);
			}
		}
	}

	if (obj->m_pSibling) TerrainCollision(obj->m_pSibling);
	if (obj->m_pChild) TerrainCollision(obj->m_pChild);
}

void CTerrain::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CTerrain::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	CGameObject::SetShader(pShader);
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

float CTerrain::GetHeight(float x, float z)
{
	XMFLOAT3 scale;
	XMStoreFloat3(&scale, transform->GetScale());

	x -= XMVectorGetX(transform->GetPosition());
	z -= XMVectorGetZ(transform->GetPosition());

	return(m_pHeightMapImage->GetHeight(x / scale.x, z / scale.z) * scale.y);
}

XMFLOAT3 CTerrain::GetNormal(float x, float z)
{
	XMFLOAT3 scale;
	XMStoreFloat3(&scale, transform->GetScale());

	return(m_pHeightMapImage->GetHeightMapNormal(int(x / scale.x), int(z / scale.z)));
}

