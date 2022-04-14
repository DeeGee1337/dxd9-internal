#pragma once

//get game window_class_name with tools such as AutoIt3Info.exe
static constexpr const char* GAME_WINDOW_CLASS_NAME = "Valve001";

struct WindowRect
{
	int32_t m_WindowXPos, m_WindowYPos, m_WindowWidth, m_WindowHeight;

	WindowRect(int32_t windowXPos, int32_t windowYPos, int32_t windowWidth, int32_t windowHeight)
	{
		this->m_WindowXPos = windowXPos;
		this->m_WindowYPos = windowYPos;
		this->m_WindowWidth = windowWidth;
		this->m_WindowHeight = windowHeight;
	}
};

class Menu
{
public:
	Menu() = default;
	~Menu() = default;
	static void HandleMessages() noexcept;

	void RenderDirectXFrame() noexcept;

	void InitilizeWindow(HMODULE hInstance, const char* windowClassName, const char* windowName) noexcept;


	HWND GetOverlayWindowHandle()						const noexcept	{ return this->m_OverlayWindowHandle; }
	HWND GetGameWindowHandle()							const noexcept	{ return this->m_GameWindowHandle; }
	WNDPROC GetGameWindowProc()							const noexcept	{ return this->m_GameWindowProc; }
	D3DPRESENT_PARAMETERS* GetD3DPresentParameters()	noexcept		{ return &this->m_D3D9PresentParams; }
	bool IsMenuOpened()									const noexcept	{ return this->m_MenuOpened; }

	void SetMenuOpenedBool(bool opened)					noexcept { this->m_MenuOpened = opened; }
private:
	void RenderImGuiWindow()	noexcept;
private:
	HWND m_OverlayWindowHandle;
	HWND m_GameWindowHandle;
	WNDPROC m_GameWindowProc;
	D3DPRESENT_PARAMETERS m_D3D9PresentParams;
	const char* m_WindowClassName;
	const char* m_WindowName;
	DWORD m_GameProcessId;
	HMODULE m_HInstance;
	LPDIRECT3DDEVICE9EX m_D3D9Device;
	bool m_MenuOpened = true;
};

extern Menu g_Menu;

