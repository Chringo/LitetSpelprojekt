//
//
//

#include "ShadowMap.h"

using namespace DirectX;

ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
	if (m_Depth)
		m_Depth->Release();
	if (m_Texture)
		m_Texture->Release();
}

HRESULT ShadowMap::Initialize(ID3D11Device *device, UINT width, UINT height)
{
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	// Set up viewport to match quality.
	m_Viewport.Width = (FLOAT)width;
	m_Viewport.Height = (FLOAT)height;
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Create depth view & texture.
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	if (FAILED(hr = device->CreateTexture2D(&texDesc, nullptr, &depthTex)))
		return hr;

	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;

	// Create depth stencil view.
	if (FAILED(hr = device->CreateDepthStencilView(depthTex, &dsvDesc, &m_Depth)))
		return hr;

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	// Create shader resource view.
	if (FAILED(hr = device->CreateShaderResourceView(depthTex, &srvDesc, &m_Texture)))
		return hr;

	depthTex->Release();

	// Shadow friendly rasterizer state.
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthBias = 10;
	rd.DepthBiasClamp = 1.5f;
	rd.SlopeScaledDepthBias = 3.5f;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.MultisampleEnable = FALSE;
	rd.AntialiasedLineEnable = FALSE;
	if (FAILED(hr = device->CreateRasterizerState(&rd, &m_RasterizerState)))
		return hr;

	return S_OK;
}

void ShadowMap::Apply(ID3D11DeviceContext *deviceContext)
{
	deviceContext->OMSetRenderTargets(0, 0, m_Depth);
	deviceContext->ClearDepthStencilView(m_Depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	deviceContext->RSSetViewports(1, &m_Viewport);
	deviceContext->RSSetState(m_RasterizerState);
}

DirectX::XMMATRIX ShadowMap::CreateViewProjection(DirectX::XMVECTOR focus, DirectX::XMVECTOR lightDirection, float distance)
{
	XMFLOAT3 f;
	XMStoreFloat3(&f, focus);

	XMMATRIX view = XMMatrixLookAtLH(
		focus - (-XMVector3Normalize(lightDirection) * distance),
		focus,
		XMVectorSet(0.f, 1.f, 0.f, 0.f));

	XMMATRIX projection = XMMatrixOrthographicLH(200, 200, 0.1f, 100);	// Tweak values.
	
	return view * projection;
}

ID3D11ShaderResourceView** ShadowMap::GetDepthAsTexture()
{
	return &m_Texture;
}