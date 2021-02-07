#include "includes.h"

// settings
bool static_crosshair = false;
bool recoil_crosshair = true;
bool box_esp_2d = true;
bool textesp = true;
bool healtharmorbar = true;
bool glow = true;
bool rcs = true;
bool triggerbot = true;
bool bunnyhop = true;
bool aimbot = true;
bool noflash = false;
bool radarhack = false;
bool customFOVtoggle = false;
bool show_menu = true;

// data
int aimSpeed = 30;
float setFOV = 1.0f;
int customFOV = 110;
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;

int CenterX = GetSystemMetrics(0) / 2 - 1; // gets screen X resolution then cutting it in half to ##### the center.
int CenterY = GetSystemMetrics(1) / 2 - 1; // gets screen Y resolution then cutting it in half to ##### the center.

typedef void* (__cdecl* tCreateInterface)(const char* name, int* pReturnCode);

void* GetInterface(tCreateInterface fn, const char* name)
{
	return (fn)(name, 0);
}

tTraceRay TraceRay;
tCreateInterface CreateInterface;

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
Player* GetClosestCrosshairEnemy(int bonepos, Vector3& targetPos, float& FOV)
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

		auto position = currentPlayer->GetBonePos(bonepos);
		float currentDistance = localPlayer->fovTo(position);
		if (currentDistance < closestDitance && currentDistance < setFOV)
		{
			FOV = currentDistance;
			targetPos = *position;
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

	//---DATA---
	//rcs v2.0 data
	Vec3 oPunch{ 0,0,0 };
	uintptr_t localPlayer = *(uintptr_t*)(hack->client + offsets::dwLocalPlayer);
	Vec3* viewAngles = (Vec3*)(*(uintptr_t*)(hack->engine + offsets::dwClientState) + offsets::dwClientState_ViewAngles);
	int* iShotsFired = (int*)(localPlayer + offsets::m_iShotsFired);
	Vec3* aimPunchAngle = (Vec3*)(localPlayer + offsets::m_aimPunchAngle);

	CreateInterface = (tCreateInterface)GetProcAddress((HMODULE)hack->engine, "CreateInterface");
	IEngineTrace* EngineTrace = (IEngineTrace*)GetInterface(CreateInterface, "EngineTraceClient004");

	// drawing stuff

	//menu
	int mennuOffX = 100;
	int menuOffY = windowHeight / 2;

	D3DCOLOR enabled = D3DCOLOR_ARGB(255,255,255,255);
	D3DCOLOR disabled = D3DCOLOR_ARGB(255, 255, 80, 0);

	if (show_menu)
	{
		draw_text("Open Menu (Insert)", mennuOffX, menuOffY, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else
	{
		bool isTrue = true;
		std::string answer = isTrue ? "true" : "false";

		char dbg_buff[100];
		sprintf_s<100>(dbg_buff, "AIMSPEED %i", aimSpeed);

		char dbg_buff2[100];
		sprintf_s<100>(dbg_buff2, "FOV %0.2f", setFOV);

		char dbg_buff3[100];
		sprintf_s<100>(dbg_buff3, "FOV CHANGER %i", customFOV);
		
		draw_text("_         ",  mennuOffX, menuOffY - 5 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
		draw_text(">(.)__      ",  mennuOffX, menuOffY - 4 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
		draw_text("______ (___/  ENTE",  mennuOffX, menuOffY - 3 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));

		draw_text("[F1] Aimbot",	mennuOffX, menuOffY + 0 * 12, aimbot ? enabled : disabled);
		draw_text("[F2] RCS",	mennuOffX, menuOffY + 1 * 12, rcs ? enabled : disabled);
		draw_text("[F3] Triggerbot",	mennuOffX, menuOffY + 2 * 12, triggerbot ? enabled : disabled);
		draw_text("[F4] Player ESP",	mennuOffX, menuOffY + 3 * 12, box_esp_2d ? enabled : disabled);
		draw_text("[F5] Radarhack",	mennuOffX, menuOffY + 4 * 12, radarhack ? enabled : disabled);
		draw_text("[F6] Glow",	mennuOffX, menuOffY + 5 * 12, glow ? enabled : disabled);
		draw_text("[F7] Bunnyhop",	mennuOffX, menuOffY + 6 * 12, bunnyhop ? enabled : disabled);
		draw_text("[F8] Recoil Crosshair",	mennuOffX, menuOffY + 7 * 12, recoil_crosshair ? enabled : disabled);
		draw_text("[F9] No Flash", mennuOffX, menuOffY + 8 * 12, noflash ? enabled : disabled);
		draw_text("[F10] FOV Changer", mennuOffX, menuOffY + 9 * 12, customFOVtoggle ? enabled : disabled);
		draw_text("-------------", mennuOffX, menuOffY + 10 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
		draw_text(dbg_buff, mennuOffX, menuOffY + 11 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
		draw_text(dbg_buff2, mennuOffX, menuOffY + 12 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
		draw_text("-------------", mennuOffX, menuOffY + 13 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
		draw_text(dbg_buff3, mennuOffX, menuOffY + 14 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	//Watermark
	draw_text("-- DeeGee Negerhook --", windowWidth / 2, windowHeight - 20, D3DCOLOR_ARGB(255, 255, 255, 255));

	for (int i = 1; i < 32; i++) 
	{
		Ent* curEnt = hack->entList->ents[i].ent;
		if (!hack->CheckValidEnt(curEnt))
			continue;

		D3DCOLOR color;
		Vec3 entHead3D = hack->GetBonePos(curEnt, 8); // head
		entHead3D.z = entHead3D.z + 8;
		Vec2 entPos2D, entHead2D;
		//test
		Vec3 entTEST3D = hack->GetBonePos(curEnt, 1);
		
		if (box_esp_2d)
		{
			if (hack->WorldToScreen(entHead3D, entHead2D) && hack->WorldToScreen(entTEST3D, entPos2D))
			{
				uintptr_t* localEntPtr = (uintptr_t*)(hack->client + offsets::dwLocalPlayer);
				uintptr_t* entityPtr = (uintptr_t*)(hack->client + offsets::dwEntityList + i * 0x10);
				uintptr_t entity = *entityPtr;
				int entityTeam = *(int*)(entity + offsets::m_iTeamNum);
				uintptr_t localPlayer = *localEntPtr;
				int glowManager = *(int*)(hack->client + offsets::dwGlowObjectManager);
				int playerTeam = *(int*)(localPlayer + offsets::m_iTeamNum);

				// if in diffrent team
				if (playerTeam != entityTeam)
				{
					vec3 eyepos;
					eyepos.x = LocalPlayer::Get()->GetOrigin()->x + LocalPlayer::Get()->GetViewOffset()->x;
					eyepos.y = LocalPlayer::Get()->GetOrigin()->y + LocalPlayer::Get()->GetViewOffset()->y;
					eyepos.z = LocalPlayer::Get()->GetOrigin()->z + LocalPlayer::Get()->GetViewOffset()->z;
					//vec3 targeteyepos = curEnt->m_vecOrigin + curEnt->m_vecViewOffset;

					vec3 bonePos; //head
					bonePos.x = *(float*)(curEnt->boneMatrix + 0x30 * 8 + 0x0C);
					bonePos.y = *(float*)(curEnt->boneMatrix + 0x30 * 8 + 0x1C);
					bonePos.z = *(float*)(curEnt->boneMatrix + 0x30 * 8 + 0x2C);

					CGameTrace trace;
					Ray_t ray;
					CTraceFilter tracefilter;
					tracefilter.pSkip = (void*)LocalPlayer::Get();

					ray.Init(eyepos, bonePos);

					EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &tracefilter, &trace);

					if (curEnt == trace.hit_entity)
					{
						//std::cout << "Ent 0x" << std::hex << currEnt.ent << " is visible\n";
						//char dbg_buffvis[100];
						int mennuOffX2 = 500;
						int menuOffY2 = windowHeight / 2;
						//sprintf_s<100>(dbg_buffvis, "is visable 0x", currEnt.ent);
						draw_text("visable", entHead2D.x, entHead2D.y - 14, D3DCOLOR_ARGB(255, 255, 255, 255));
						color = D3DCOLOR_ARGB(255, 0, 255, 0);
					}
					else 
					{
						color = D3DCOLOR_ARGB(255, 255, 0, 0);
					}

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

						int offset = 3; // thickness

						botHealth.y = botArmor.y = entPos2D.y;
						botHealth.x = entPos2D.x - (height / 4) - offset;
						botArmor.x = entPos2D.x + (height / 4) + offset;
						topHealth.y = entHead2D.y + height - healthHeight;
						topArmor.y = entHead2D.y + height - armorHeight;
						topHealth.x = entPos2D.x - (height / 4) - offset - (dX * healthPct);
						topArmor.x = entPos2D.x + (height / 4) + offset - (dX * armorPct);

						//draw_line(botHealth, topHealth, offset, D3DCOLOR_ARGB(255, 0, 255, 0));
						draw_line(topHealth.x - 1.5f, topHealth.y, topHealth.x - 1.5f, botHealth.y, 1, D3DCOLOR_ARGB(255, 0, 0, 0));
						draw_line(topHealth.x, topHealth.y, topHealth.x, botHealth.y, offset, D3DCOLOR_ARGB(255, 0, 255, 0));
						draw_line(topHealth.x + 1.5f, topHealth.y, topHealth.x + 1.5f, botHealth.y, 1, D3DCOLOR_ARGB(255, 0, 0, 0));

						//draw_line(botArmor, topArmor, offset, D3DCOLOR_ARGB(255, 0, 255, 255));
						draw_line(topArmor.x - 1.5f, topArmor.y, topArmor.x - 1.5f, botArmor.y, 1, D3DCOLOR_ARGB(255, 0, 0, 0));
						draw_line(topArmor.x, topArmor.y, topArmor.x, botArmor.y, offset, D3DCOLOR_ARGB(255, 0, 255, 255));
						draw_line(topArmor.x + 1.5f, topArmor.y, topArmor.x + 1.5f, botArmor.y, 1, D3DCOLOR_ARGB(255, 0, 0, 0));
					}

					if (textesp)
					{
						std::stringstream s1, s2;
						s1 << curEnt->iHealth;
						s2 << curEnt->ArmorValue;
						std::string t1 = "Health: " + s1.str();
						std::string t2 = "Armor: " + s2.str();
						char* healthMsg = (char*)t1.c_str();
						char* armorMsg = (char*)t2.c_str();

						draw_text(healthMsg, entPos2D.x, entPos2D.y, D3DCOLOR_ARGB(255, 255, 255, 255));
						draw_text(armorMsg, entPos2D.x, entPos2D.y + 12, D3DCOLOR_ARGB(255, 255, 255, 255));

						if (!curEnt->bHasHelmet)
							draw_text("No Helmet", entPos2D.x, entPos2D.y + 24, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}
			}
		}
	}

	if (aimbot)
	{
		uintptr_t* localEntPtr = (uintptr_t*)(hack->client + offsets::dwLocalPlayer);
		if (localEntPtr)
		{

			Vector3 targetPos;
			float FOV;
			Player* closestEnemy = GetClosestCrosshairEnemy(8, targetPos, FOV);

			if (closestEnemy)
			{

				Vec2 pos2D;
				Vec3 targetPos3;
				targetPos3.x = targetPos.x;
				targetPos3.y = targetPos.y;
				targetPos3.z = targetPos.z;

				/*if (hack->WorldToScreen(targetPos3, pos2D))
				{
					draw_text(dbg_txt, pos2D.x, pos2D.y, D3DCOLOR_ARGB(255, 255, 255, 255));
				}*/

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					//LocalPlayer::Get()->AimAt(closestEnemy->GetBonePos(8));
					//Sleep(1);
					LocalPlayer::Get()->setViewAngle(&targetPos, aimSpeed, true);
				}
			}
		}
	}

	//if (rcs)
	//{
	//	Vec3 punchAngle = *aimPunchAngle * 2;
	//	if (*iShotsFired > 1) {
	//		// calc rcs
	//		Vec3 newAngle = *viewAngles + oPunch - punchAngle;
	//		// normalize
	//		newAngle.Normalize();
	//		// set

	//		*viewAngles = newAngle;
	//	}
	//	// fix
	//	oPunch = punchAngle;
	//}


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

	Vec3 oPunch{ 0,0,0 };
	//---DATA---
	//rcs v2.0 data
	uintptr_t localPlayer = *(uintptr_t*)(hack->client + offsets::dwLocalPlayer);
	Vec3* viewAngles = (Vec3*)(*(uintptr_t*)(hack->engine + offsets::dwClientState) + offsets::dwClientState_ViewAngles);
	int* iShotsFired = (int*)(localPlayer + offsets::m_iShotsFired);
	Vec3* aimPunchAngle = (Vec3*)(localPlayer + offsets::m_aimPunchAngle);

	//triggerbot data
	//uintptr_t* localEntPtr = (uintptr_t*)(hack->client + offsets::dwLocalPlayer);

	// hack loop
	while (!GetAsyncKeyState(VK_END))
	{
		//triggerbot data
		uintptr_t* localEntPtr = (uintptr_t*)(hack->client + offsets::dwLocalPlayer);
		
		//maybe crash -> ingame check needed
		/*if (*reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(hack->engine + offsets::dwClientState) + offsets::dwClientState_State) != 6)
			continue;*/

		if (GetAsyncKeyState(VK_INSERT) & 1)
			show_menu = !show_menu;

		if (GetAsyncKeyState(VK_F1) & 1)
			aimbot = !aimbot;

		if (GetAsyncKeyState(VK_F2) & 1)
			rcs = !rcs;

		if (GetAsyncKeyState(VK_F3) & 1)
			triggerbot = !triggerbot;

		if (GetAsyncKeyState(VK_F4) & 1)
			box_esp_2d = !box_esp_2d;

		if (GetAsyncKeyState(VK_F5) & 1)
			radarhack = !radarhack;

		if (GetAsyncKeyState(VK_F6) & 1)
			glow = !glow;

		if (GetAsyncKeyState(VK_F7) & 1)
			bunnyhop = !bunnyhop;

		if (GetAsyncKeyState(VK_F8) & 1)
			recoil_crosshair = !recoil_crosshair;

		if (GetAsyncKeyState(VK_F9) & 1)
			noflash = !noflash;

		if (GetAsyncKeyState(VK_F10) & 1)
			customFOVtoggle = !customFOVtoggle;

		if (GetAsyncKeyState(VK_ADD) & 1)
			customFOV++;

		if (GetAsyncKeyState(VK_SUBTRACT) & 1)
			customFOV--;

		if (GetAsyncKeyState(VK_UP) & 1)
			aimSpeed++;

		if (GetAsyncKeyState(VK_DOWN) & 1)
			aimSpeed--;

		if (GetAsyncKeyState(VK_LEFT) & 1)
			setFOV = setFOV - 0.1f;

		if (GetAsyncKeyState(VK_RIGHT) & 1)
			setFOV = setFOV + 0.1f;

		hack->Update();

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
					Ent* curEnt = hack->entList->ents[i].ent;
					if (!hack->CheckValidEnt(curEnt))
						continue;

					uintptr_t* entityPtr = (uintptr_t*)(hack->client + offsets::dwEntityList + i * 0x10);
					uintptr_t entity = *entityPtr;

					if (entity)
					{
						int entityTeam = *(int*)(entity + offsets::m_iTeamNum);

						// if in diffrent team
						if (playerTeam != entityTeam)
						{
							int G = curEnt->iHealth / 100.f * 255.f;
							int R = 255.f - G;

							int entityGlowIdx = *(int*)(entity + offsets::m_iGlowIndex);
							*(float*)(glowManager + entityGlowIdx * 0x38 + 0x4) = R; //red
							*(float*)(glowManager + entityGlowIdx * 0x38 + 0x8) = G; //green
							*(float*)(glowManager + entityGlowIdx * 0x38 + 0x10) = 1.f; //alpha
							*(int*)(glowManager + entityGlowIdx * 0x38 + 0x24) = 1; //enable
						}
					}
				}

			}
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
								Sleep(30); // der einzige lmao
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

		if (noflash)
		{
			if (localEntPtr)
			{
				uintptr_t localEnt = *localEntPtr;
				auto flash_max_alpha = *(int*)(localEnt + offsets::m_flFlashMaxAlpha);

				if (flash_max_alpha != 0)
					flash_max_alpha = 0;
			}
		}

		if (radarhack)
		{
			for (int i = 1; i < 32; i++)
			{
				uintptr_t ent = *(uintptr_t*)(hack->client + offsets::dwEntityList + i * 0x10);
				if (ent)
				{
					*(uintptr_t*)(ent + offsets::m_bSpotted) = true;
				}
			}
		}

		if (customFOVtoggle)
		{
			if (localEntPtr)
			{
				uintptr_t localPlayer = *(uintptr_t*)(hack->client + offsets::dwLocalPlayer);
				*(int*)(localPlayer + offsets::m_iFOV) = customFOV;
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