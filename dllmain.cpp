#include "includes.h"

// settings
bool static_crosshair = false;
bool recoil_crosshair = true;
bool snapline_esp = true;
bool box_esp_2d = true;

// data
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;

int CenterX = GetSystemMetrics(0) / 2 - 1; // gets screen X resolution then cutting it in half to ##### the center.
int CenterY = GetSystemMetrics(1) / 2 - 1; // gets screen Y resolution then cutting it in half to ##### the center.

// hook function
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {
	if (!pDevice)
		pDevice = o_pDevice;

	// drawing stuff

	for (int i = 1; i < 32; i++) {
		Ent* curEnt = hack->entList->ents[i].ent;
		if (!hack->CheckValidEnt(curEnt))
			continue;

		D3DCOLOR color;
		if (curEnt->iTeamNum == hack->localEnt->iTeamNum)
			color = D3DCOLOR_ARGB(255, 0, 255, 0);
		else
			color = D3DCOLOR_ARGB(255, 255, 0, 0);

		Vec3 entHead3D = hack->GetBonePos(curEnt, 8); // head
		entHead3D.z = entHead3D.z + 8;
		Vec2 entPos2D, entHead2D;

		if (snapline_esp)
		{
			if (hack->WorldToScreen(curEnt->vecOrigin, entPos2D))
				draw_line(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, color);
		}
		if (box_esp_2d)
		{
			if (hack->WorldToScreen(entHead3D, entHead2D))
				draw_esp_box_2d(entPos2D, entHead2D, 1, color);
		}

	}

	// crosshair

	//static crosshair
	if (static_crosshair)
	{
		draw_crosshair(windowWidth / 2, windowHeight / 2, 15, 1, D3DCOLOR_ARGB(255, 204, 0, 0)); // diagonal line
		draw_crosshair((windowWidth / 2) - 15, windowHeight / 2, 15, 1, D3DCOLOR_ARGB(255, 204, 0, 0)); // diagonal line
		draw_crosshair(windowWidth / 2, windowHeight / 2, 1, 15, D3DCOLOR_ARGB(255, 204, 0, 0)); // vertical line
		draw_crosshair(windowWidth / 2, (windowHeight / 2) - 15, 1, 15, D3DCOLOR_ARGB(255, 204, 0, 0)); // vertical line
	}
	//recoil crosshair
	if (recoil_crosshair)
	{
		Vec2 l, r, t, b;
		l = r = t = b = hack->crosshair2D;
		l.x = l.x - hack->crosshairSize;
		r.x = r.x + hack->crosshairSize;
		t.y = t.y - hack->crosshairSize;
		b.y = b.y + hack->crosshairSize;

		draw_line(l, r, 2, D3DCOLOR_ARGB(255, 255, 255, 255));
		draw_line(t, b, 2, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	// call og function
	oEndScene(pDevice);
}

DWORD WINAPI HackThread(HMODULE hModule) {

	// hook
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}

	hack = new Hack();
	hack->Init();

	// hack loop
	while (!GetAsyncKeyState(VK_END)) {
		hack->Update();

		// crosshairrecoil
		Vec3 pAng = hack->localEnt->aimPunchAngle;
		hack->crosshair2D.x = windowWidth / 2 - (windowWidth / 90 * pAng.y);
		hack->crosshair2D.y = windowHeight / 2 + (windowHeight / 90 * pAng.x);

		//vischeck raytrace
		//SOONTM

	}

	// unhook
	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) {
	if (reason == DLL_PROCESS_ATTACH)
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
	return TRUE;
}