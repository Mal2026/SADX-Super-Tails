#include "stdafx.h"

PhysicsData_t milesPhysicCopy;

void __cdecl Miles_SuperPhysics_Delete(ObjectMaster* obj) {

	memcpy(&PhysicsArray[Characters_Tails], &milesPhysicCopy, sizeof(PhysicsData_t));
}


void __cdecl Miles_SuperPhysics_Load(ObjectMaster* obj)
{

	ObjUnknownB* v1; // esi
	CharObj2* v2; // eax

	v1 = (ObjUnknownB*)obj->UnknownB_ptr;
	v2 = GetCharObj2(v1->Time);
	if (v2)
	{
		memcpy(&milesPhysicCopy, &PhysicsArray[Characters_Tails], sizeof(PhysicsData_t));

		if (!customPhysics) { //use vanilla broken super sonic physics
			v2->PhysicsData.RollDecel = -0.001f;
			v2->PhysicsData.AirDecel = -0.0020000001f;
			v2->PhysicsData.AirAccel = 0.050000001f;
		}
		else {
			v2->PhysicsData.GroundAccel = 0.065f;
			v2->PhysicsData.RollDecel = -0.006f;
			v2->PhysicsData.AirDecel = -0.01799999999f;
			v2->PhysicsData.AirAccel = 0.040f;
			v2->PhysicsData.MaxAccel = 3.0f;
		}

		obj->DeleteSub = Miles_SuperPhysics_Delete;
		obj->MainSub = Sonic_SuperPhysics_Main;
	}
	else
	{
		DeleteObjectMaster(obj);
	}
}



void Load_SuperPhysics(taskwk* data1) {

	task* v11 = (task*)LoadObject(LoadObj_UnknownB, 2, Miles_SuperPhysics_Load);
	if (v11)
	{
		v11->awp->work.ub[0] = data1->counter.b[0];
	}
}
