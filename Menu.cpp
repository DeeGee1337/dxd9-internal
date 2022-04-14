#include "pch.h"
#include "Menu.h"

Menu g_Menu;

static HWND GetWindowHandleFromProcessId(DWORD dwProcessID)
{
	HWND hCurWnd = NULL;
	do
	{
		hCurWnd = FindWindowExA(NULL, hCurWnd, GAME_WINDOW_CLASS_NAME, NULL);

		DWORD dwProcID = 0;
		GetWindowThreadProcessId(hCurWnd, &dwProcID);

		if (dwProcID == dwProcessID)
			return hCurWnd;

	} while (hCurWnd != NULL);

	return NULL;
}

static LRESULT CALLBACK MainTimerLoop(HWND hwnd, UINT uMsg, int32_t timerId, DWORD dwTime)
{
	g_Menu.RenderDirectXFrame();

	//after this run all of your features such as aimbot, esp, etc..

	return TRUE;
}

void Menu::HandleMessages() noexcept
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		if (GetMessageA(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
			Sleep(1);
		}
	}
}


WindowRect GetGameRect(HWND gameWindowHandle)
{
	WINDOWINFO windowInfo;
	windowInfo.cbSize = sizeof(windowInfo);

	GetWindowInfo(gameWindowHandle, &windowInfo);

	int32_t width = (windowInfo.rcWindow.right - windowInfo.rcWindow.left);
	int32_t height = (windowInfo.rcWindow.bottom - windowInfo.rcWindow.top);

	int32_t windowTitleHeight = 0;
	if (windowInfo.dwStyle & WS_BORDER)
	{
		windowTitleHeight = (windowInfo.rcWindow.bottom - windowInfo.rcWindow.top) - (windowInfo.rcClient.bottom - windowInfo.rcClient.top);
	}

	return WindowRect(windowInfo.rcWindow.left + windowInfo.cxWindowBorders, windowInfo.rcWindow.top + (windowTitleHeight - windowInfo.cyWindowBorders), width, height);
}

static LRESULT CALLBACK WinProcedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//delay between timer execution
	//it's like Sleep() in while true loop
	static constexpr uint32_t DELAY = (1000 / 30);

	switch (Message)
	{
	case WM_CREATE:
		//we create a timer that will execute MainTimerLoop function every DELAY milliseconds
		//this is your while true loop where you put all your cheat functions inside
		SetTimer(hWnd, 0, DELAY, reinterpret_cast<TIMERPROC>(&MainTimerLoop));
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 0);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcA(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

WNDCLASSEXA wClass;

static ATOM RegMyWindowClass(HINSTANCE hInst, const char* windowClassName, const char* windowName)
{
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEXA);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wClass.hInstance = hInst;
	wClass.lpfnWndProc = WinProcedure;
	wClass.lpszClassName = windowClassName;
	wClass.lpszMenuName = windowName;
	wClass.style = CS_VREDRAW | CS_HREDRAW;

	return RegisterClassExA(&wClass);
}

static bool hasWindowPositionChanged = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
static LRESULT __stdcall wnd_proc(const HWND hwnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param)
{
	static bool bWasMenuClicked = false, bWasKeyPressed = true;

	if (u_msg == WM_WINDOWPOSCHANGED)
	{
		//if game window position or size changes we want to get the new size and position of it by using l_param and GetWindowInfo for additional
		//information such as width and heigh of window title bar and borders so we can properly adjust our overlay to be on top of the window
		WINDOWPOS* windowPositionStructure = reinterpret_cast<WINDOWPOS*>(l_param);
		WINDOWINFO windowInfo;
		windowInfo.cbSize = sizeof(windowInfo);

		GetWindowInfo(windowPositionStructure->hwnd, &windowInfo);

		if (windowInfo.dwStyle & WS_BORDER)
		{
			int32_t windowTitleHeight = (windowInfo.rcWindow.bottom - windowInfo.rcWindow.top) - (windowInfo.rcClient.bottom - windowInfo.rcClient.top);
			windowPositionStructure->y += windowTitleHeight;
			windowPositionStructure->cy -= windowTitleHeight;
		}

		auto d3d9PresentParams = g_Menu.GetD3DPresentParameters();

		d3d9PresentParams->BackBufferWidth = windowPositionStructure->cx;
		d3d9PresentParams->BackBufferHeight = windowPositionStructure->cy;

		ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(windowPositionStructure->cx), static_cast<float>(windowPositionStructure->cy));

		//move our overlay window to the new position of the game window
		MoveWindow(g_Menu.GetOverlayWindowHandle(), windowPositionStructure->x + windowInfo.cxWindowBorders, windowPositionStructure->y - windowInfo.cyWindowBorders, windowPositionStructure->cx, windowPositionStructure->cy, false);

		//we set this bool to true so our render function can reset the D3D9device
		//I tried to reset the device in here, dx9 documentation say that we need to invalidate and create device objects right before we call beginScene()
		hasWindowPositionChanged = true;
	}

	if (u_msg == WM_KEYUP && w_param == VK_INSERT)
	{
		bool openedFlag = g_Menu.IsMenuOpened();
		g_Menu.SetMenuOpenedBool(!openedFlag);
	}

	if (g_Menu.IsMenuOpened())
	{
		//I wanted to have menu active and only accept inputs once I pressed left click on it
		//this way I can see my menu while still passing inputs to the game until I press on my menu

		auto& io = ImGui::GetIO();

		if (io.WantCaptureMouse && (u_msg == WM_LBUTTONDOWN && w_param == VK_LBUTTON))
			bWasMenuClicked = true;

		if (!io.WantCaptureMouse)
			bWasMenuClicked = false;

		if (bWasMenuClicked)
		{
			if (u_msg == WM_KEYDOWN && bWasKeyPressed)
			{
				bWasKeyPressed = false;
				return CallWindowProcA(reinterpret_cast<WNDPROC>(g_Menu.GetGameWindowProc()), hwnd, WM_KEYUP, w_param, l_param);
			}

			ImGui_ImplWin32_WndProcHandler(hwnd, u_msg, w_param, l_param);

			return true;
		}
	}

	bWasKeyPressed = true;

	return CallWindowProcA(reinterpret_cast<WNDPROC>(g_Menu.GetGameWindowProc()), hwnd, u_msg, w_param, l_param);
}

void Menu::InitilizeWindow(HMODULE hInstance, const char* windowClassName, const char* windowName) noexcept
{
	this->m_D3D9Device = nullptr;
	this->m_D3D9PresentParams = { 0 };
	this->m_OverlayWindowHandle = 0;
	this->m_WindowClassName = windowClassName;
	this->m_WindowName = windowName;
	this->m_HInstance = hInstance;
	this->m_GameProcessId = GetCurrentProcessId();
	this->m_GameWindowHandle = GetWindowHandleFromProcessId(this->m_GameProcessId);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = 1.0f / 60.0f;
	io.IniFilename = nullptr;
	io.Fonts->AddFontDefault();

	if (!RegMyWindowClass(hInstance, windowClassName, windowName))
		return;

	WindowRect gameWindowRect = GetGameRect(this->m_GameWindowHandle);

	//we create TOP_MOST window that's transparent and layerd which means all the mouse and keyboard events will go through it to the game
	//WS_EX_TOOLWINDOW will make this window as a toolwindow which will hide it from being seen in task manager
	//finally, we set our new window to be the child of the game, this is why you will be able to use this overlay in fullscreen
	//once you click to maximize the game, all the child windows are drawn after it which will help draw our overlay
	this->m_OverlayWindowHandle = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, windowClassName, windowName, WS_POPUP, gameWindowRect.m_WindowXPos, gameWindowRect.m_WindowYPos, gameWindowRect.m_WindowWidth, gameWindowRect.m_WindowHeight, this->m_GameWindowHandle, NULL, hInstance, NULL);
	SetLayeredWindowAttributes(this->m_OverlayWindowHandle, 0, 255, LWA_ALPHA);
	SetLayeredWindowAttributes(this->m_OverlayWindowHandle, 0, RGB(0, 0, 0), LWA_COLORKEY);

	io.DisplaySize = ImVec2(static_cast<float>(gameWindowRect.m_WindowWidth), static_cast<float>(gameWindowRect.m_WindowHeight));

	LPDIRECT3D9EX pD3D;
	auto result = Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D);

	if (FAILED(result))
		return;

	ZeroMemory(&this->m_D3D9PresentParams, sizeof(this->m_D3D9PresentParams));
	this->m_D3D9PresentParams.Windowed = TRUE;
	this->m_D3D9PresentParams.hDeviceWindow = this->m_OverlayWindowHandle;
	this->m_D3D9PresentParams.BackBufferWidth = gameWindowRect.m_WindowWidth;
	this->m_D3D9PresentParams.BackBufferHeight = gameWindowRect.m_WindowHeight;
	this->m_D3D9PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	this->m_D3D9PresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
	this->m_D3D9PresentParams.EnableAutoDepthStencil = TRUE;
	this->m_D3D9PresentParams.AutoDepthStencilFormat = D3DFMT_D16;
	this->m_D3D9PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->m_OverlayWindowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &this->m_D3D9PresentParams, 0, &this->m_D3D9Device) != D3D_OK)
	{
		pD3D->Release();
		return;
	}

	ImGui_ImplWin32_Init(this->m_GameWindowHandle);
	ImGui_ImplDX9_Init(this->m_D3D9Device);

	ImGui::StyleColorsDark();

	UpdateWindow(this->m_OverlayWindowHandle);
	ShowWindow(this->m_OverlayWindowHandle, SW_SHOW);

	//we hook game's window_procedure and point it to the ImGui's so we have control over mouse and keyboard inputs
	//this way we can select which events we want to pass to our overlay window or to the the game window
	this->m_GameWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(this->m_GameWindowHandle, GWLP_WNDPROC, LONG_PTR(wnd_proc)));
}

void Menu::RenderDirectXFrame() noexcept
{
	auto device = this->m_D3D9Device;

	//we check whether game window has focus, if not we won't draw our overlay
	if ((GetForegroundWindow() == this->m_GameWindowHandle) && (this->m_MenuOpened))
	{
		//if you change game window's size or position, you need to reset the D3D9device, otherwise you will get some problems where ImGui will be huge or very small
		//and you will lose ability to pass inputs to it
		if (hasWindowPositionChanged)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();

			device->Reset(&this->m_D3D9PresentParams);

			ImGui_ImplDX9_CreateDeviceObjects();
		}

		device->BeginScene();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		this->RenderImGuiWindow();

		ImGui::EndFrame();

		device->SetRenderState(D3DRS_ZENABLE, false);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		device->EndScene();
	}

	device->Present(0, 0, 0, 0);
	device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
}

void Menu::RenderImGuiWindow() noexcept
{
	if (ImGui::Begin("GH Internal ImGui Overlay", &this->m_MenuOpened))
	{
		ImGui::Text("Some bullshit text");

		ImGui::End();
	}
}
