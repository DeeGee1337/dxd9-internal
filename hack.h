#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define ABS(x) ((x < 0) ? (-x) : (x))

// last update 06.12.2020

struct Vec2 {
	float x, y;
};
struct Vec3 {
	float x, y, z;
};
struct Vec4 {
	float x, y, z, w;
};

class Ent {
public:
	union {
		// isDormant
		DEFINE_MEMBER_N(bool, isDormant, 0xED); // offset
		// iHealth
		DEFINE_MEMBER_N(int, iHealth, 0x100); // offset
		// vecOrigin
		DEFINE_MEMBER_N(Vec3, vecOrigin, 0x138); // offset
		// iTeamNum
		DEFINE_MEMBER_N(int, iTeamNum, 0xF4); // offset
		// boneMatrix
		DEFINE_MEMBER_N(int, boneMatrix, 0x26A8); // offset
		// armorValue
		DEFINE_MEMBER_N(int, ArmorValue, 0xB378); // offset
		// aimPunchAngle
		DEFINE_MEMBER_N(Vec3, aimPunchAngle, 0x302C); // offset

		//vischeck maybe soon TM
		//origin
		DEFINE_MEMBER_N(Vec3, origin, 0x138); // offset
		//clientId
		DEFINE_MEMBER_N(int, clientId, 0x64); // offset
		//m_vecViewOffset
		DEFINE_MEMBER_N(Vec3, m_vecViewOffset, 0x108); // offset
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