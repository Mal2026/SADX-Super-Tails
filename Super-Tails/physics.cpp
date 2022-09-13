#include "stdafx.h"

PhysicsData_t milesPhysicsCopy = { 0 };
static FunctionHook<void, EntityData1*, EntityData2*, CharObj2*> ResetAngle_t((intptr_t)0x443AD0);

void __cdecl Miles_SuperPhysics_Delete(task* obj) {

	memcpy(&PhysicsArray[Characters_Tails], &milesPhysicsCopy, sizeof(PhysicsData_t));
}

void __cdecl Miles_SuperPhysics_Load(task* obj)
{
	auto awp = (anywk*)obj->awp;
	auto co2 = GetCharObj2(awp->work.ub[0]);

	if (co2)
	{
		memcpy(&milesPhysicsCopy, &PhysicsArray[Characters_Tails], sizeof(PhysicsData_t));

		if (!customPhysics || isPerfectChasoLevel()) { //use vanilla broken super sonic physics
			co2->PhysicsData.RollDecel = -0.001f;
			co2->PhysicsData.AirDecel = -0.0020000001f;
			co2->PhysicsData.AirAccel = 0.050000001f;
		}
		else {
			co2->PhysicsData.GroundAccel = 0.065f;
			co2->PhysicsData.RollDecel = -0.006f;
			co2->PhysicsData.AirDecel = -0.01799999999f;
			co2->PhysicsData.AirAccel = 0.040f;
			co2->PhysicsData.MaxAccel = 3.0f;
		}

		obj->dest = Miles_SuperPhysics_Delete;
		obj->exec = (TaskFuncPtr)Sonic_SuperPhysics_Main;
	}
	else
	{
		FreeTask(obj);
	}
}

void Load_SuperPhysics(taskwk* data1) {

	auto task = CreateElementalTask(LoadObj_UnknownB, 2, Miles_SuperPhysics_Load);
	if (task)
	{
		task->awp->work.ub[0] = data1->counter.b[0];
	}
}

//fix spring issue, only run if Super Sonic mod is disabled.
static void __cdecl ResetAngle_r(EntityData1* data, EntityData2* data2, CharObj2* co2)
{
	if (!isPerfectChasoLevel() && co2->Upgrades & Upgrades_SuperSonic)
	{
		NJS_VECTOR a2a = { 0 };
		taskwk* twk = (taskwk*)data;

		a2a.x = co2->Speed.x;
		a2a.y = co2->Speed.y;
		a2a.z = co2->Speed.z;
		if (co2->PhysicsData.Run2 * co2->PhysicsData.Run2 >= a2a.z * a2a.z + a2a.y * a2a.y + a2a.x * a2a.x)
		{
			PConvertVector_P2G(twk, &a2a);
			data->Rotation.x = BAMS_SubWrap(data->Rotation.x, GravityAngle_Z, 2048);
			data->Rotation.z = BAMS_SubWrap(data->Rotation.z, GravityAngle_X, 2048);
			PConvertVector_G2P(twk, &a2a);
			co2->Speed.x = a2a.x;
			co2->Speed.y = a2a.y;
			co2->Speed.z = a2a.z;
		}
	}
	else
	{
		ResetAngle_t.Original(data, data2, co2);
	}
}

void init_PhysicsHack()
{
	bool bSS = GetModuleHandle(L"Better-Super-Sonic");

	if (!bSS) {
		ResetAngle_t.Hook(ResetAngle_r);
	}

}