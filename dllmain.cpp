#include "includes.h"

// settings
bool static_crosshair = false;
bool recoil_crosshair = true;
bool snapline_esp = true;
bool box_esp_2d = true;
bool healtharmorbar = true;
bool glow = true;
bool rcs = true;
bool triggerbot = true;
bool bunnyhop = true;
bool aimbot = true;

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
			{
				draw_esp_box_2d(entPos2D, entHead2D, 1, color);

				if (healtharmorbar)
				{
					int height = ABS(entPos2D.y - entHead2D.y);
					int dX = (entPos2D.x - entHead2D.x);

					float healthPct = curEnt->iHealth / 100.0f;
					float armorPct = curEnt->ArmorValue / 100.0f;

					Vec2 botHealth, topHealth, botArmor, topArmor;
					int healthHeight = height * healthPct;
					int armorHeight = height * armorPct;

					int offset = 2; // thickness

					botHealth.y = botArmor.y = entPos2D.y;
					botHealth.x = entPos2D.x - (height / 4) - offset;
					botArmor.x = entPos2D.x + (height / 4) - offset; 
					topHealth.y = entHead2D.y + height - healthHeight;
					topArmor.y = entHead2D.y + height - armorHeight;
					topHealth.x = entPos2D.x - (height / 4) - offset - (dX * healthPct);
					topArmor.x = entPos2D.x + (height / 4) + offset - (dX * armorPct);

					draw_line(botHealth, topHealth, offset, D3DCOLOR_ARGB(255, 0, 255, 0));
					draw_line(botArmor, topArmor, offset, D3DCOLOR_ARGB(255, 0, 255, 255));
				}
			}
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

	//---DATA---
	//rcs v2.0 data
	Vec3 oPunch = { 0,0,0 };
	uintptr_t localPlayer = *(uintptr_t*)(hack->client + offsets::dwLocalPlayer);
	Vec3* viewAngles = (Vec3*)(*(uintptr_t*)(hack->engine + offsets::dwClientState) + offsets::dwClientState_ViewAngles);
	int* iShotsFired = (int*)(localPlayer + offsets::m_iShotsFired);
	Vec3* aimPunchAngle = (Vec3*)(localPlayer + offsets::m_aimPunchAngle);

	//triggerbot data
	uintptr_t* localEntPtr = (uintptr_t*)(hack->client + offsets::dwLocalPlayer);

	// hack loop
	while (!GetAsyncKeyState(VK_END)) 
	{
		hack->Update();

		// crosshairrecoil
		Vec3 pAng = hack->localEnt->aimPunchAngle;
		hack->crosshair2D.x = windowWidth / 2 - (windowWidth / 90 * pAng.y);
		hack->crosshair2D.y = windowHeight / 2 + (windowHeight / 90 * pAng.x);

		if (glow)
		{
			if (localEntPtr)
			{
				uintptr_t localPlayer = *localEntPtr;
				int glowManager = *(int*)(hack->client + offsets::dwGlowObjectManager);
				int playerTeam = *(int*)(localPlayer + offsets::m_iTeamNum);

				for (int i = 1; i < 32; i++) 
				{
					uintptr_t* entityPtr = (uintptr_t*)(hack->client + offsets::dwEntityList + i * 0x10);
					uintptr_t entity = *entityPtr;

					if (entity) 
					{
						int entityTeam = *(int*)(entity + offsets::m_iTeamNum);
						// if in diffrent team
						if (playerTeam != entityTeam) 
						{
							int entityGlowIdx = *(int*)(entity + offsets::m_iGlowIndex);
							*(float*)(glowManager + entityGlowIdx * 0x38 + 0x8) = 1.f;
							*(float*)(glowManager + entityGlowIdx * 0x38 + 0x10) = 1.f;
							*(int*)(glowManager + entityGlowIdx * 0x38 + 0x24) = 1;
						}
					}
				}
			}
		}

		if (rcs)
		{
			Vec3 punchAngle = *aimPunchAngle * 2;
			if (*iShotsFired > 1) {
				// calc rcs
				Vec3 newAngle = *viewAngles + oPunch - punchAngle;
				// normalize
				newAngle.Normalize();
				// set
				*viewAngles = newAngle;
			}
			// fix
			oPunch = punchAngle;
		}

		if (triggerbot)
		{
			if (localEntPtr)
			{
				uintptr_t localEnt = *localEntPtr;
				int localTeam = *(int*)(localEnt + offsets::m_iTeamNum);

				if (GetAsyncKeyState(VK_XBUTTON1))
				{
					int crosshairId = *(int*)(localEnt + offsets::m_iCrosshairId);

					if (crosshairId <= 64 && crosshairId != 0)
					{
						uintptr_t crossHairEnt = *(uintptr_t*)(hack->client + offsets::dwEntityList + (crosshairId - 1) * 0x10);

						if (crossHairEnt)
						{
							int crosshairTeam = *(int*)(crossHairEnt + offsets::m_iTeamNum);
							int crosshairLifeState = *(int*)(crossHairEnt + offsets::m_lifeState);

							if ((localTeam != crosshairTeam) && (crosshairLifeState == 0))
							{
								*(int*)(hack->client + offsets::dwForceAttack) = 5;
								Sleep(30);
								*(int*)(hack->client + offsets::dwForceAttack) = 4;
							}
							else *(int*)(hack->client + offsets::dwForceAttack) = 4;
						}
					}
				}
			}
		}

		//if (aimbot)
		//{
		//	while (GetAsyncKeyState(VK_SPACE))
		//	{
		//		Ent* localPlayer = *(Ent**)(hack->client + offsets::dwLocalPlayer);
		//		uintptr_t playerStatePtr = hack->engine + offsets::dwClientState;
		//		Vec3* viewAngles = (Vec3*)(*(uintptr_t*)(playerStatePtr)+offsets::dwClientState_ViewAngles);
		//		EntList* entList = (EntList*)(hack->client + offsets::dwEntityList);

		//		Ent* target = hack->GetBestTarget(localPlayer, viewAngles, entList);

		//		if (target)
		//		{
		//			Vec3 body = target->m_vecOrigin;
		//			body.z -= 10;
		//			*viewAngles = hack->CalcAngle(localPlayer->m_vecOrigin, body);
		//		}
		//	}
		//}
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