//
//
//

#ifndef SHADOW_MAP__H
#define SHADOW_MAP__H

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

// Determines texture size used by depth buffer in multiples of 1024.
#define SHADOW_QUALITY		4

class ShadowMap
{
	public:
		ShadowMap();
		~ShadowMap();

		HRESULT Initialize(ID3D11Device *device, UINT width, UINT height);
		void Apply(ID3D11DeviceContext *deviceContext);

		DirectX::XMMATRIX CreateViewProjection(DirectX::XMVECTOR focus, DirectX::XMVECTOR lightDirection, float distance);

		ID3D11ShaderResourceView** GetDepthAsTexture();

private:

	ID3D11DepthStencilView *m_Depth = nullptr;
	ID3D11ShaderResourceView *m_Texture = nullptr;

	D3D11_VIEWPORT m_Viewport;
	ID3D11RasterizerState *m_RasterizerState = nullptr;
};


#endif /// SHADOW_MAP__H