#pragma once

#include "offsets.h"
#include "csgoVector.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define ABS(x) ((x < 0) ? (-x) : (x))

const float pi = 3.14159265358979323846f;

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

	//Vec3() {};
	//Vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	//Vec3 operator + (const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
	//Vec3 operator - (const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
	//Vec3 operator * (const float& rhs) const { return Vec3(x * rhs, y * rhs, z * rhs); }
	//Vec3 operator / (const float& rhs) const { return Vec3(x / rhs, y / rhs, z / rhs); }
	//Vec3& operator += (const Vec3& rhs) { return *this = *this + rhs; }
	//Vec3& operator -= (const Vec3& rhs) { return *this = *this - rhs; }
	//Vec3& operator *= (const float& rhs) { return *this = *this * rhs; }
	//Vec3& operator /= (const float& rhs) { return *this = *this / rhs; }
	//float Length() const { return sqrtf(x * x + y * y + z * z); }
	//Vec3 Normalize() const { return *this * (1 / Length()); }
	//float Distance(const Vec3& rhs) const { return (*this - rhs).Length(); }

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
		// clientID
		DEFINE_MEMBER_N(int, clientId, 0x64);

		//// m_vecOrigin -y ist dopplt
		DEFINE_MEMBER_N(vec3, m_vecOrigin, offsets::m_vecOrigin);

		//// m_vecViewOffset
		DEFINE_MEMBER_N(vec3, m_vecViewOffset, offsets::m_vecViewOffset);
		//// m_lifeState
		//DEFINE_MEMBER_N(char, m_lifeState, offsets::m_lifeState);

		DEFINE_MEMBER_N(bool, bHasHelmet, 0x1839);
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
	uintptr_t dwEntityList = offsets::dwEntityList;
	uintptr_t dwViewMatrix = offsets::dwViewMatrix;

	uintptr_t engine;
	uintptr_t client;
	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	//aimbot
	Ent* target;

	ID3DXLine* LineL;
	ID3DXFont* FontF;

	Vec2 crosshair2D;
	int crosshairSize = 4;

	void Init();
	void Update();
	bool CheckValidEnt(Ent* ent);
	bool WorldToScreen(Vec3 pos, Vec2& screen);
	Vec3 GetBonePos(Ent* ent, int bone);

	////aimbot
	//bool IsValidTarget(Ent* localPlayer, Ent* ent);
	//Ent* GetBestTarget(Ent* localPlayer, Vec3* viewAngles, EntList* entList);
	//Vec3 CalcAngle(Vec3 src, Vec3 dst);
};


//// Raytrace
//// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/gametrace.h#L30
//// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/public/cmodel.h#L61
//// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/bspflags.h
//// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/public/engine/IEngineTrace.h#L54
//
//class IClientEntityList
//{
//public:
//	// Get IClientNetworkable interface for specified entity
//	virtual void* GetClientNetworkable(int entnum) = 0;
//	virtual void* GetClientNetworkableFromHandle(int hEnt) = 0;
//	virtual void* GetClientUnknownFromHandle(int hEnt) = 0;
//
//	// NOTE: This function is only a convenience wrapper.
//	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
//	virtual void* GetClientEntity(int entnum) = 0;
//	virtual void* GetClientEntityFromHandle(int hEnt) = 0;
//
//	// Returns number of entities currently in use
//	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;
//
//	// Returns highest index actually used
//	virtual int					GetHighestEntityIndex(void) = 0;
//
//	// Sizes entity list to specified size
//	virtual void				SetMaxEntities(int maxents) = 0;
//	virtual int					GetMaxEntities() = 0;
//};
//

//void* GetInterface(const char* dllname, const char* interfacename)
//{
//	tCreateInterface CreateInterface = (tCreateInterface)GetProcAddress(GetModuleHandle(dllname), "CreateInterface");
//
//	int returnCode = 0;
//	void* interface2 = CreateInterface(interfacename, &returnCode);
//
//	return interface2;
//}