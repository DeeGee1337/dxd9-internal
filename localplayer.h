#pragma once

class Vector3;
class LocalPlayer
{

private:

	LocalPlayer();

public:

	static LocalPlayer* Get();

	Vector3* GetOrigin();
	Vector3* GetEyePosition();
	Vector3* GetViewOffset();
	int* GetHealth();
	int* GetTeam();
	void AimAt(Vector3* target);
	float GetDistance(Vector3*);
	float GetAngle(Vector3* target);
	Vector3 setViewAngle(Vector3* target, int aimSpeed, bool rcsActive);
	Vector3 smoothRotation(Vector3 angle, float factor);
	float fovTo(Vector3* target);


	void CalcAngle(Vector3* target, Vector3* viewAngles);

};