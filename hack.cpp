#include "includes.h"

void Hack::Init() {
	client = (uintptr_t)GetModuleHandle("client.dll");
	engine = (uintptr_t)GetModuleHandle("engine.dll");
	entList = (EntList*)(client + dwEntityList);
	localEnt = entList->ents[0].ent;
}

void Hack::Update() {
	memcpy(&viewMatrix, (PBYTE*)(client + dwViewMatrix), sizeof(viewMatrix));
}

bool Hack::CheckValidEnt(Ent* ent) {
	if (ent == nullptr)
		return false;
	if (ent == localEnt)
		return false;
	if (ent->iHealth <= 0)
		return false;
	if (ent->isDormant)
		return false;
	return true;
}

// 
bool Hack::WorldToScreen(Vec3 pos, Vec2& screen) {
	Vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	Vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

Vec3 Hack::GetBonePos(Ent* ent, int bone)
{
	uintptr_t bonePtr = ent->boneMatrix;
	Vec3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * bone + 0x0C); // each bone in 30 bytes * bone (8 == head) + 10p
	bonePos.y = *(float*)(bonePtr + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(bonePtr + 0x30 * bone + 0x2C);

	return bonePos;
}


//bool Hack::IsValidTarget(Ent* localPlayer, Ent* ent)
//{
//	if (ent && ent->iTeamNum != localPlayer->iTeamNum && ent->clientId != localPlayer->clientId && ent->m_lifeState == 0)
//	{
//		return true;
//	}
//	else return false;
//}
//
//Vec3 Hack::CalcAngle(Vec3 src, Vec3 dst)
//{
//	Vec3 angles;
//
//	Vec3 delta = src - dst;
//	float hyp = src.Distance(dst);
//	angles.x = -asin(delta.z / hyp) * 180.0f / pi;
//	angles.y = atan2(delta.y, delta.x) * 180.0f / pi;
//	angles.z = 0.0f;
//
//	if (delta.x >= 0.0f)
//		angles.y += 180.0f;
//
//	return angles;
//}
//
//Ent* Hack::GetBestTarget(Ent* localPlayer, Vec3* viewAngles, EntList* entList)
//{
//	float oldDistance = FLT_MAX;
//	float newDistance = 0;
//	Ent* target = nullptr;
//
//	for (auto curr : entList->ents)
//	{
//		if (IsValidTarget(localPlayer, curr.ent))
//		{
//			Vec3 eyepos = localPlayer->m_vecOrigin + localPlayer->m_vecViewOffset;
//			Vec3 angleTo = CalcAngle(eyepos, curr.ent->m_vecOrigin);
//			newDistance = viewAngles->Distance(angleTo);
//
//			if (newDistance < oldDistance)
//			{
//				oldDistance = newDistance;
//				target = curr.ent;
//			}
//		}
//	}
//	return target;
//}