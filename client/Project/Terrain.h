#pragma once
#include "Object.h"
#include "Shader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHeightMapImage
{
private:
	//높이 맵 이미지 픽셀(8-비트)들의 이차원 배열이다. 각 픽셀은 0~255의 값을 갖는다.
	BYTE* m_pHeightMapPixels;
	//높이 맵 이미지의 가로와 세로 크기이다. 
	int m_nWidth;
	int m_nLength;
	//높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다. 
	XMFLOAT3 m_xmf3Scale;
public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);
	//높이 맵 이미지에서 (x, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다. 
	float GetHeight(float x, float z);
	//높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다.
	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTerrainShader : public CIlluminatedShader
{
public:
	CTerrainShader(){}
	~CTerrainShader(){}

public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHeightMapGridMesh : public CMesh
{
public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), 
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);
	virtual ~CHeightMapGridMesh();
	
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet = 0);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }
	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다. 
	virtual float OnGetHeight(int x, int z, void* pContext);
	//격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수이다. 
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);

public:
	/*인덱스 버퍼(인덱스의 배열)와 인덱스 버퍼를 위한 업로드 버퍼에 대한 인터페이스 포인터이다. 인덱스 버퍼는 정점
버퍼(배열)에 대한 인덱스를 가진다.*/
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_VERTEX_BUFFER_VIEW m_d3dNormalBufferView;
	D3D12_VERTEX_BUFFER_VIEW m_d3dColorBufferView;

protected:
	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;

	ID3D12Resource* m_pd3dColorBuffer = NULL;
	ID3D12Resource* m_pd3dColorUploadBuffer = NULL;

	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	//격자의 크기(가로: x-방향, 세로: z-방향)이다. 
	int m_nWidth;
	int m_nLength;
	/*격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터이다. 실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 갖는다. 즉, 실제 격자의 x-축 방향의 간격은 1이 아니
	라 스케일 벡터의 x-좌표가 된다. 이렇게 하면 작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할
	수 있다.*/
	XMFLOAT3 m_xmf3Scale;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CTerrain : public CGameObject
{
public:
	CTerrain(string filePath);
	virtual ~CTerrain();

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void Update();

	//virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	void TerrainCollision(CGameObject* obj);

public:
	void SetMesh(int nIndex, CMesh* pMesh);
	void SetShader(CShader* pShader);
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다. 
	float GetHeight(float x, float z);
//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다. 
	XMFLOAT3 GetNormal(float x, float z);
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { XMFLOAT3 scale; XMStoreFloat3(&scale, transform->GetScale()); return scale; }
	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다. 
	float GetWidth() { return(m_nWidth * XMVectorGetX(transform->GetScale())); }
	float GetLength() { return(m_nLength * XMVectorGetZ(transform->GetScale())); }

public:
	CShader* m_pShader = nullptr;

	int m_nMeshes = 0;
	CMesh** m_ppMeshes;

private:
	//지형의 높이 맵으로 사용할 이미지이다. 
	CHeightMapImage* m_pHeightMapImage;
	//높이 맵의 가로와 세로 크기이다. 
	int m_nWidth;
	int m_nLength;
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다. 
	//XMFLOAT3 m_xmf3Scale;
};

