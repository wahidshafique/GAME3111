#include "inputclass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other)
{

}

InputClass::~InputClass()
{

}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_mouseX = 0;
	m_mouseY = 0;

	// Init main direct input interface
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
		return false;

	// Init direct input interface for keyboard
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
		return false;

	// Set data format
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	// Set cooperative level of keyboard to share with other programs
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	// Acquire keyboard
	result = m_keyboard->Acquire();
	if (FAILED(result))
		return false;

	// Init direct input interface for mouse
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
		return false;

	// Set data format for mouse
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	// Set coop level of mouse to sahre with other programs
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	// Acquire mouse
	result = m_mouse->Acquire();
	if (FAILED(result))
		return false;

	return true;
}

void InputClass::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	result = ReadKeyboard();
	if (!result)
		return false;

	result = ReadMouse();
	if (!result)
		return false;

	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// Read keyboard device
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// If keyboard lost focus or was not acquired
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		} else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	// Read mouse device
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// If mouse lost focuse or was not acquired
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		} else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// Update location of mouse cursor based on change of mouse location during frame
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed screen width/height
	if (m_mouseX < 0)
		m_mouseX = 0;
	if (m_mouseY < 0)
		m_mouseY = 0;

	if (m_mouseX > m_screenWidth)
		m_mouseX = m_screenWidth;
	if (m_mouseY > m_screenHeight)
		m_mouseY = m_screenHeight;

	return;
}

bool InputClass::IsEscapePressed()
{
	// Bitwise and on keyboard state to check if certain key is currently being pressed
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftArrowPressed()
{
	if (m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsRightArrowPressed()
{
	if (m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;

	return;
}