#pragma once

#include "offsets.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define ABS(x) ((x < 0) ? (-x) : (x))

struct Vec2 
{
	float x, y;
};

struct Vec3 
{
	float x, y, z;

	Vec3 operator+(Vec3 d) 
	{
		return { x + d.x, y + d.y, z + d.z };
	}
	Vec3 operator-(Vec3 d) 
	{
		return { x - d.x, y - d.y, z - d.z };
	}
	Vec3 operator*(float d) 
	{
		return { x * d, y * d, z * d };
	}

	void Normalize() 
	{
		while (y < -180) { y += 360; }
		while (y > 180) { y -= 360; }
		if (x > 89) { x = 89; }
		if (x < -89) { x = -89; }
	}
};

struct Vec4 
{
	float x, y, z, w;
};

class Ent {
public:
	union {
		// isDormant
		DEFINE_MEMBER_N(bool, isDormant, offsets::m_bDormant); // offset
		// iHealth
		DEFINE_MEMBER_N(int, iHealth, offsets::m_iHealth); // offset
		// vecOrigin
		DEFINE_MEMBER_N(Vec3, vecOrigin, offsets::m_vecOrigin); // offset
		// iTeamNum
		DEFINE_MEMBER_N(int, iTeamNum, offsets::m_iTeamNum); // offset
		// boneMatrix
		DEFINE_MEMBER_N(int, boneMatrix, offsets::m_dwBoneMatrix); // offset
		// armorValue
		DEFINE_MEMBER_N(int, ArmorValue, offsets::m_ArmorValue); // offset
		// aimPunchAngle
		DEFINE_MEMBER_N(Vec3, aimPunchAngle, offsets::m_aimPunchAngle); // offset

		////vischeck maybe soon TM
		////origin
		//DEFINE_MEMBER_N(Vec3, origin, 0x138); // offset
		////clientId
		//DEFINE_MEMBER_N(int, clientId, 0x64); // offset
		////m_vecViewOffset
		//DEFINE_MEMBER_N(Vec3, m_vecViewOffset, 0x108); // offset
	};
};

class EntListObj {
public:
	Ent* ent;
	char padding[12];
};

class EntList {
public:
	EntListObj ents[32];
};

class Hack {
public:
	uintptr_t dwEntityList = 0x4D9EA34; // offset
	uintptr_t dwViewMatrix = 0x4D90334; // offset

	uintptr_t engine;
	uintptr_t client;
	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	ID3DXLine* LineL;

	Vec2 crosshair2D;
	int crosshairSize = 4;

	void Init();
	void Update();
	bool CheckValidEnt(Ent* ent);
	bool WorldToScreen(Vec3 pos, Vec2& screen);
	Vec3 GetBonePos(Ent* ent, int bone);
};