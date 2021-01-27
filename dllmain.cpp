#include "includes.h"

#include "csgoVector.h"
#include "csgotrace.h"

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
bool aimbot = false;

// data
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;

//trace data
tTraceRay TraceRay;
tCreateInterface CreateInterface;

int CenterX = GetSystemMetrics(0) / 2 - 1; // gets screen X resolution then cutting it in half to ##### the center.
int CenterY = GetSystemMetrics(1) / 2 - 1; // gets screen Y resolution then cutting it in half to ##### the center.

Player* GetClosestEnemy()
{
	LocalPlayer* localPlayer = LocalPlayer::Get();

	float closestDitance = 1000000;
	int closesDistanceIndex = -1;

	for (int i = 1; i < *Player::GetMaxPlayer(); i++)
	{
		Player* currentPlayer = Player::GetPlayer(i);

		if (!currentPlayer || !(*(uint32_t*)currentPlayer) || (uint32_t)currentPlayer == (uint32_t)localPlayer)
		{
			continue;
		}

		if (*currentPlayer->GetTeam() == *localPlayer->GetTeam())
		{
			continue;
		}

		if (*currentPlayer->GetHealth() < 1 || *localPlayer->GetHealth() < 1)
		{
			continue;
		}

		float currentDistance = localPlayer->GetDistance(currentPlayer->GetOrigin());
		if (currentDistance < closestDitance)
		{
			closestDitance = currentDistance;
			closesDistanceIndex = i;
		}
	}

	if (closesDistanceIndex == -1)
	{
		return NULL;
	}
	return Player::GetPlayer(closesDistanceIndex);
}

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
				draw_line(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, D3DCOLOR_ARGB(0, 255, 0, 0));
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

	////tracedata
	//CreateInterface = (tCreateInterface)GetProcAddress((HMODULE)hack->engine, "CreateInterface");
	//IEngineTrace* EngineTrace = (IEngineTrace*)GetInterface(CreateInterface, "EngineTraceClient004");

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
		if (GetAsyncKeyState(VK_F1) & 1)
			aimbot = !aimbot;

		if (GetAsyncKeyState(VK_F2) & 1)
			rcs = !rcs;

		if (GetAsyncKeyState(VK_F3) & 1)
			triggerbot = !triggerbot;

		if (GetAsyncKeyState(VK_F4) & 1)
			box_esp_2d = !box_esp_2d;

		if (GetAsyncKeyState(VK_F5) & 1)
			snapline_esp = !snapline_esp;

		if (GetAsyncKeyState(VK_F6) & 1)
			glow = !glow;

		if (GetAsyncKeyState(VK_F7) & 1)
			bunnyhop = !bunnyhop;

		if (GetAsyncKeyState(VK_F8) & 1)
			recoil_crosshair = !recoil_crosshair;

		hack->Update();
		
		// crosshairrecoil
		Vec3 pAng = hack->localEnt->aimPunchAngle;
		hack->crosshair2D.x = windowWidth / 2 - (windowWidth / 90 * pAng.y);
		hack->crosshair2D.y = windowHeight / 2 + (windowHeight / 90 * pAng.x);

		////trace
		//Ent* pLocal = *(Ent**)(hack->client + offsets::dwLocalPlayer);

		//if (GetAsyncKeyState(VK_SPACE) & 1)
		//{
		//	EntList* entList = (EntList*)(hack->client + offsets::dwEntityList);
		//	for (auto currEnt : entList->ents)
		//	{
		//		if (currEnt.ent && currEnt.ent->clientId != pLocal->clientId)
		//		{
		//			vec3 eyepos = pLocal->m_vecOrigin + pLocal->m_vecViewOffset;
		//			vec3 targeteyepos = currEnt.ent->m_vecOrigin + currEnt.ent->m_vecViewOffset;

		//			CGameTrace trace;
		//			Ray_t ray;
		//			CTraceFilter tracefilter;
		//			tracefilter.pSkip = (void*)pLocal;

		//			ray.Init(eyepos, targeteyepos);

		//			EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &tracefilter, &trace);

		//			if (currEnt.ent == trace.hit_entity)
		//			{
		//				draw_fill_rect(200, 200, 30, 30, D3DCOLOR_ARGB(255, 255, 255, 255));
		//			}
		//		}
		//	}
		//}

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

		if (bunnyhop)
		{
			if (localEntPtr)
			{
				uintptr_t localEnt = *localEntPtr;
				int localTeam = *(int*)(localEnt + offsets::m_iTeamNum);

				BYTE flag = *(BYTE*)(localEnt + offsets::m_fFlags);

				if (GetAsyncKeyState(VK_SPACE) && (flag & (1 << 0)))
				{
					*(DWORD*)(hack->client + offsets::dwForceJump) = 6;
				}
			}
		}

		if (aimbot)
		{
			if (localEntPtr)
			{
				if (GetAsyncKeyState(VK_LBUTTON))
				{
					Player* closestEnemy = GetClosestEnemy();

					if (closestEnemy)
					{
						LocalPlayer::Get()->AimAt(closestEnemy->GetBonePos(8));
					}
				}
			}
		}
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