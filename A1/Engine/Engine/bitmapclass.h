#pragma once
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

// INCLUDES //
#include <d3d11.h>
#include <d3dx10math.h>

// MY CLASS INCLUDES //
#include "textureclass.h"

class BitmapClass
{
private:
	struct GuiElementVertexData
	{
		D3DXVECTOR2 position;
		D3DXVECTOR2 size;
		D3DXVECTOR4 color;
		D3DXVECTOR4 source;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_guiVertexBuffer;
	TextureClass* m_Texture;

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

#endif