#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Text = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_ModelList = 0;
	m_Frustum = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}


GraphicsClass::~GraphicsClass()
{

}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	// Create Direct3D object
	m_D3D = new D3DClass;
	if (!m_D3D)
		return false;

	// Init Direct3D object
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create camera object
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// Initialize a base view matrix with the camera for 2D user interface rendering
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create text object
	m_Text = new TextClass;
	if (!m_Text)
		return false;

	// Initialize text object
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// Initialize the model object
	result = m_Model->Initialize(m_D3D->GetDevice(), L"../Engine/data/seafloor.dds", "../Engine/data/cube.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create light shader object
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
		return false;

	// Initialize light shader object
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create light object
	m_Light = new LightClass;
	if (!m_Light)
		return false;

	// Init light object
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// Create model list object
	m_ModelList = new ModelListClass;
	if (!m_ModelList)
		return false;

	// Init model list object
	result = m_ModelList->Initialize(25);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// Create frustum object
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
		return false;

	return true;
}


void GraphicsClass::Shutdown()
{
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame(float rotationY)
{
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	return true;
}


bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color;
	bool renderModel, result;

	m_D3D->BeginScene(0.0f, 0.5f, 0.5f, 1.0f);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	modelCount = m_ModelList->GetModelCount();

	renderCount = 0;

	for (index = 0; index<modelCount; index++)
	{
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		radius = 1.0f;

		renderModel = m_Frustum->CheckCube(positionX, positionY, positionZ, radius);

		if (renderModel)
		{
			D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ);

			m_Model->Render(m_D3D->GetDeviceContext());

			m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(), m_Light->GetDirection(), color);

			m_D3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}


	result = m_Text->SetRenderCount(renderCount, m_D3D->GetDeviceContext());
	if (!result)
		return false;


	m_D3D->TurnZBufferOff();


	m_D3D->TurnOnAlphaBlending();

	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
		return false;

	m_D3D->TurnOffAlphaBlending();

	m_D3D->TurnZBufferOn();

	m_D3D->EndScene();

	return true;
}