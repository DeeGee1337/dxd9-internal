#include "includes.h"

LocalPlayer* LocalPlayer::Get()
{
	static uint32_t clientModule = (uint32_t)(GetModuleHandle("client.dll"));
	static LocalPlayer* localPlayer = (LocalPlayer*)(clientModule + offsets::dwLocalPlayer);

	return localPlayer;
}

Vector3* LocalPlayer::GetOrigin()
{
	return (Vector3*)(*(uintptr_t*)this + offsets::m_vecOrigin);
}

Vector3* LocalPlayer::GetViewOffset()
{

	return (Vector3*)(*(uintptr_t*)this + offsets::m_vecViewOffset);
}

int* LocalPlayer::GetHealth()
{
	return (int*)(*(uintptr_t*)this + offsets::m_iHealth);
}

int* LocalPlayer::GetTeam()
{
	return (int*)(*(uintptr_t*)this + offsets::m_iTeamNum);
}

float LocalPlayer::GetDistance(Vector3* other)
{
	Vector3* myPos = GetOrigin();
	Vector3 delta = Vector3(other->x - myPos->x, other->y - myPos->y, other->z - myPos->z);

	return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}


double PI = 3.14159265358;
void LocalPlayer::AimAt(Vector3* target)
{
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + offsets::dwClientState) + offsets::dwClientState_ViewAngles);

	Vector3 origin = *GetOrigin();
	Vector3 viewOffset = *GetViewOffset();
	Vector3* myPos = &(origin + viewOffset);

	Vector3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

	float pitch = (-asin(deltaVec.z / deltaVecLength) * (180 / PI));
	float yaw = (atan2(deltaVec.y, deltaVec.x) * (180 / PI));

	if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180)
	{
		viewAngles->x = pitch;
		viewAngles->y = yaw;
	}
}

Vector3 clampAngles(Vector3 angle)
{
	if (angle.x > 89.0f && angle.x <= 180.0f)
		angle.x = 89.0f;
	if (angle.x > 180.0f)
		angle.x = angle.x - 360.0f;
	if (angle.x < -89.0f)
		angle.x = -89.0f;
	if (angle.y > 180.0f)
		angle.y = angle.y - 360.0f;
	if (angle.y < -180.0f)
		angle.y = angle.y + 360.0f;
	return angle;
}
Vector3 normalizeAngle(Vector3 angle)
{
	if (angle.x > 180)
		angle.x -= 360.0f;
	if (angle.x < 180)
		angle.x += 360.0f;
	if (angle.y > 180)
		angle.y -= 360.0f;
	if (angle.y < 180)
		angle.y += 360.0f;
	return angle;
}



void LocalPlayer::CalcAngle(Vector3* target, Vector3* viewAngles)
{
	Vector3 origin = *GetOrigin();
	Vector3 viewOffset = *GetViewOffset();
	Vector3* myPos = &(origin + viewOffset);

	Vector3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

	float pitch = (-asin(deltaVec.z / deltaVecLength) * (180 / PI));
	float yaw = (atan2(deltaVec.y, deltaVec.x) * (180 / PI));

	viewAngles->x = pitch;
	viewAngles->y = yaw;

	*viewAngles = clampAngles(*viewAngles);
}
void clamp(Vector3& angle)
{
	if (angle.x > 180)
		angle.x -= 360;
	else if (angle.x < -180)
		angle.x += 360;

	if (angle.y > 180)
		angle.y -= 360;
	else if (angle.y < -180)
		angle.y += 360;

	if (angle.x < -89)
		angle.x = -89;

	if (angle.x > 89)
		angle.x = 89;

	while (angle.y < -180.0f)
		angle.y += 360.0f;

	while (angle.y > 180.0f)
		angle.y -= 360.0f;

	angle.z = 0;

}

Vector3 LocalPlayer::smoothRotation(Vector3 angle, float factor)
{	
	clamp(angle);

	angle.x  /= factor;
	angle.y  /= factor;

	return angle;
}
Vector3 vecToAngle(Vector3 src, Vector3 dst)
{
	Vector3 angle;
	Vector3 delta = src - dst;

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	angle.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angle.x = (atan(delta.z / hyp) * 57.295779513082f);
	if (delta.x >= 0.0) angle.y += 180.0f;

	return angle;
}

float LocalPlayer::fovTo(Vector3* target)
{
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + offsets::dwClientState) + offsets::dwClientState_ViewAngles);

	Vector3 currentAngle = *viewAngles;

	Vector3 origin = *GetOrigin();
	Vector3 viewOffset = *GetViewOffset();
	Vector3* myPos = &(origin + viewOffset);


	//Vector3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	//float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

	//float pitch = (-asin(deltaVec.z / deltaVecLength) * (180 / PI));
	//float yaw = (atan2(deltaVec.y, deltaVec.x) * (180 / PI));

	auto newRot = vecToAngle(*myPos, *target);

	Vector3 deltaAngle;
	deltaAngle.x = newRot.x - currentAngle.x;
	deltaAngle.y = newRot.y - currentAngle.y;


	clamp(deltaAngle);

	return (fabs(deltaAngle.x) + fabs(deltaAngle.y) / 2);
}

Vector3 oldPunch(0, 0, 0);
int gLastShots = 0;

//HUSOFUNC
Vector3 LocalPlayer::setViewAngle(Vector3* target, int aimSpeed, bool rcsActive)
{
	static uint32_t engineModule = (uint32_t)GetModuleHandle("engine.dll");
	static Vector3* viewAngles = (Vector3*)(*(uint32_t*)(engineModule + offsets::dwClientState) + offsets::dwClientState_ViewAngles);


	Vector3 currentAngle = *viewAngles;

	Vector3 origin = *GetOrigin();
	Vector3 viewOffset = *GetViewOffset();
	Vector3* myPos = &(origin + viewOffset);

	uintptr_t localPlayer = *(uintptr_t*)(hack->client + offsets::dwLocalPlayer);
	if (!localPlayer)
		return {};

	//Vector3 deltaVec = { target->x - myPos->x, target->y - myPos->y, target->z - myPos->z };
	//float deltaVecLength = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);

	//float pitch = (-asin(deltaVec.z / deltaVecLength) * (180 / PI));
	//float yaw = (atan2(deltaVec.y, deltaVec.x) * (180 / PI));

	auto newRot = vecToAngle(*myPos, *target);

	Vector3 deltaAngle;
	deltaAngle.x = newRot.x - currentAngle.x;
	deltaAngle.y = newRot.y - currentAngle.y;

	//g_before_smooth = deltaAngle;

	//g_calc_angle.x = pitch;
	//g_calc_angle.y = yaw;


	deltaAngle = smoothRotation(deltaAngle, ((0.10f * ((float)aimSpeed)) + 0.5f)); //*1000 for legit

	/*g_after_smooth.x = deltaAngle.x;
	g_after_smooth.y = deltaAngle.y;*/

	Vector3 finalRotation;
	finalRotation.x = deltaAngle.x + currentAngle.x;
	finalRotation.y = deltaAngle.y + currentAngle.y;


	clamp(finalRotation);


	if (finalRotation.x >= -89 && finalRotation.x <= 89 &&
		finalRotation.y >= -180 && finalRotation.y <= 180)
	{

		//g_is_blasting = true;

		//rcs = true
		if (true)
		{
			Vector3 aimPunchAngle = *(Vector3*)(localPlayer + offsets::m_aimPunchAngle);
			aimPunchAngle.x *= 0.55; //fixme
			aimPunchAngle.y *= 0.55; //fixme

			int* iShotsFired = (int*)(localPlayer + offsets::m_iShotsFired);

			if (*iShotsFired > 1)
			{
				finalRotation.x -= aimPunchAngle.x;
				finalRotation.y -= aimPunchAngle.y;
			}


			//	oldPunch.x = aimPunchAngle.x;
			//	oldPunch.y = aimPunchAngle.y;
			//	gLastShots = *iShotsFired;

			//	// fix
			//}
			//else
			//{
			//	oldPunch = { 0,0,0 };
			//}

			clamp(finalRotation);
			*viewAngles = finalRotation;
			return finalRotation;
		}
		else
		{
			//OutputDebugStringA("ALAAAAAAARM!!");
		}
		return *viewAngles;
	}
}
