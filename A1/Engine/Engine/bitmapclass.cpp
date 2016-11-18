#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
	m_guiVertexBuffer = 0;
	m_Texture = 0;
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	
	m_previousPosX = -1;
	m_previousPosY = -1;

	
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	
	ReleaseTexture();

	
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	
	RenderBuffers(deviceContext);

	return true;
}

ID3D11ShaderResourceView * BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}
//alteration ala Example buffer creation 
bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vbd;
	HRESULT result;

	vbd.ByteWidth = sizeof(GuiElementVertexData);
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	result = device->CreateBuffer(&vbd, 0, &m_guiVertexBuffer);
	if (result != S_OK)
	{
		return result;
	}

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	
	if (m_guiVertexBuffer)
	{
		m_guiVertexBuffer->Release();
		m_guiVertexBuffer = 0;
	}

	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	GuiElementVertexData* guiVertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	GuiElementVertexData* guiVerticesPtr;
	HRESULT result;

	
	
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	
	guiVertices = new GuiElementVertexData();
	if (!guiVertices)
	{
		return false;
	}

	guiVertices->color = D3DXVECTOR4(0.0f, 0.5f, 1.f, 1.f);
	guiVertices->size = D3DXVECTOR2((float)m_bitmapWidth / m_screenWidth * 2.f, (float)m_bitmapHeight / m_screenHeight * 2.f);
	guiVertices->position = D3DXVECTOR2(2.f * 500.f / m_screenWidth - 1.f, 1.f - 2.f * 300.f / m_screenHeight);
	guiVertices->source = D3DXVECTOR4(0.f, 1.f, 1.f, 0.f);

	
	result = deviceContext->Map(m_guiVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	
	guiVerticesPtr = (GuiElementVertexData*)mappedResource.pData;

	
	memcpy(guiVerticesPtr, (void*)guiVertices, (sizeof(GuiElementVertexData)));

	
	deviceContext->Unmap(m_guiVertexBuffer, 0);

	
	delete guiVertices;
	guiVertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	
	stride = sizeof(GuiElementVertexData);
	offset = 0;

	
	deviceContext->IASetVertexBuffers(0, 1, &m_guiVertexBuffer, &stride, &offset);

	
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
