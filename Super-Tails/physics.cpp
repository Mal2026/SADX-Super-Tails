#include "stdafx.h"

PhysicsData_t milesPhysicCopy;

void __cdecl Miles_SuperPhysics_Delete(ObjectMaster* obj) {

	memcpy(&milesPhysicCopy, &PhysicsArray[Characters_Tails], sizeof(PhysicsData_t));
}

void __cdecl Miles_SuperPhysics_Load(ObjectMaster* obj)
{

	ObjUnknownB* v1; // esi
	CharObj2* v2; // eax

	v1 = (ObjUnknownB*)obj->UnknownB_ptr;
	v2 = GetCharObj2(v1->Time);
	if (v2)
	{
		memcpy(&milesPhysicCopy, &PhysicsArray[Characters_Sonic], sizeof(PhysicsData_t));
		v1->Index = v2->PhysicsData.RollDecel;
		v1->Mode = v2->PhysicsData.AirDecel;
		v1->field_C = v2->PhysicsData.AirAccel;
		v2->PhysicsData = PhysicsArray[Characters_Sonic];
		v2->PhysicsData.RollDecel = -0.001;
		v2->PhysicsData.AirDecel = -0.0020000001;
		v2->PhysicsData.AirAccel = 0.035;
		v2->PhysicsData.YOff = 6.4000001;
		obj->DeleteSub = Miles_SuperPhysics_Delete;
		obj->MainSub = Sonic_SuperPhysics_Main;
	}
	else
	{
		DeleteObjectMaster(obj);
	}
}
