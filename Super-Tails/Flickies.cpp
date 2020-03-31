#include "stdafx.h"
#include "config.h"
#include "Trampoline.h"

ObjectMaster* flicky1 = nullptr;
ObjectMaster* flicky2 = nullptr;
ObjectMaster* flicky3 = nullptr;
ObjectMaster* flicky4 = nullptr;
NJS_VECTOR	bombpos;
float		bombsize;
extern int delay;



void CallFlicky() {

	if (flicky1)
		return;

	ObjectMaster* play1 = GetCharacterObject(0);
	LoadPVM("AMY_R", &AMY_TEXLIST); //Birdie texture


	flicky1 = LoadObject((LoadObj)2, 3, LoadBird);
	flicky2 = LoadObject((LoadObj)2, 3, LoadBird);
	flicky3 = LoadObject((LoadObj)2, 3, LoadBird);
	flicky4 = LoadObject((LoadObj)2, 3, LoadBird);
	

	return;
}


void DeleteFlicky() {

	CheckThingButThenDeleteObject(flicky1);
	CheckThingButThenDeleteObject(flicky2);
	CheckThingButThenDeleteObject(flicky3);
	CheckThingButThenDeleteObject(flicky4);
	flicky1 = nullptr;
	flicky2 = nullptr;
	flicky3 = nullptr;
	flicky4 = nullptr;

	return;
}

//Flicky uses Player position
void UpdateFlickyPosition() {

	ObjectMaster* play1 = GetCharacterObject(0);

	if (isSuper)
	{
		if (flicky2)
			flicky2->Data1->Position.x = play1->Data1->Position.x + 5;

		if (flicky3)
			flicky3->Data1->Position.x = play1->Data1->Position.x + 10;

		if (flicky4)
			flicky4->Data1->Position.x = play1->Data1->Position.x + 15;
	}

	return;
}


void KillEnemiesInSphere(NJS_VECTOR* pos, float radius) {
	bombpos = *pos;
	bombsize = radius;
}

float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2));
}

NJS_VECTOR GetPointToFollow(NJS_VECTOR* pos, NJS_VECTOR* dir, Rotation3* rot) {
	NJS_VECTOR point;
	njPushMatrix(_nj_unit_matrix_);
	njTranslateV(0, pos);
	njRotateY(0, -rot->y);
	njCalcPoint(0, dir, &point);
	njPopMatrix(1u);
	return point;
}

Rotation3 fPositionToRotation(NJS_VECTOR* orig, NJS_VECTOR* point) {
	NJS_VECTOR dist;
	Rotation3 result;

	dist.x = orig->x - point->x;
	dist.y = orig->y - point->y;
	dist.z = orig->z - point->z;

	result.x = atan2(dist.y, dist.z) * 65536.0 * -0.1591549762031479;
	result.y = atan2(dist.x, dist.z) * 65536.0 * 0.1591549762031479;

	result.y = -result.y - 0x4000;
	return result;
}

NJS_VECTOR GetPathPosition(NJS_VECTOR* orig, NJS_VECTOR* dest, float state) {
	NJS_VECTOR result;
	result.x = (dest->x - orig->x) * state + orig->x;
	result.y = (dest->y - orig->y) * state + orig->y;
	result.z = (dest->z - orig->z) * state + orig->z;

	return result;
}

bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius) {
	return (powf(pos->x - center->x, 2) + pow(pos->y - center->y, 2) + pow(pos->z - center->z, 2)) <= pow(radius, 2);
}

//Enemy attack functions 
ObjectMaster* Flicky_GetClosestEnemy(NJS_VECTOR* pos) {
	ObjectMaster* current = ObjectListThing[3];
	while (1) {
		if (current->MainSub == Kiki_Main || current->MainSub == RhinoTank_Main 
			|| current->MainSub == SpinnerA_Main || current->MainSub == SpinnerB_Main || current->MainSub == SpinnerC_Main
			|| current->MainSub == UnidusA_Main || current->MainSub == UnidusB_Main || current->MainSub == UnidusC_Main
			|| current->MainSub == Leon_Main || current->MainSub == BoaBoa_Main || current->MainSub == ESman) {

			float dist = GetDistance(pos, &current->Data1->Position);
			if (GetDistance(pos, &current->Data1->Position) < 200) 
				return current;
			else {
				if (current->Next) {
					current = current->Next;
					continue;
				}
				else break;
			}
		}
		else {
			if (current->Next) current = current->Next;
			else break;
		}
	}
	return nullptr;
}

bool Flicky_CheckEnemy(ObjectMaster* obj) {
	if (Flicky_GetClosestEnemy(&obj->Data1->Position)) {
			return true;
		}
	
	return false;
}

bool OhNoImDead2(EntityData1* a1, ObjectData2* a2);
Trampoline OhNoImDead2_t(0x004CE030, 0x004CE036, OhNoImDead2);
bool OhNoImDead2(EntityData1* a1, ObjectData2* a2) {
	if (a1->CollisionInfo->CollidingObject) {
		if (a1->CollisionInfo->CollidingObject->Object->MainSub == Chao_Main
			&& a1->CollisionInfo->CollidingObject->Object->Data1->Action == 4) 
			return 1;
	}

	if (bombsize && GetDistance(&bombpos, &a1->Position) < bombsize) {
		bombsize = 0;
		return 1;
	}

	FunctionPointer(bool, original, (EntityData1 * a1, ObjectData2 * a2), OhNoImDead2_t.Target());
	return original(a1, a2);
}


bool isRanddone = false;
//Using Trampoline to get the control of each Flicky Object.
Trampoline FlickyObj_Main_t(0x4c63f0, 0x4c63f8, FlickyAction_Main_r);

void FlickyAction_Main_r(ObjectMaster* obj) {

	ObjectMaster* play1 = GetCharacterObject(0);

	if (play1->Data1->CharID != Characters_Tails) //restore original function for Amy bird.
	{
		ObjectFunc(origin, FlickyObj_Main_t.Target());
		origin(obj);
		return;
	}


	EntityData1* CurrentFlicky = obj->Data1;
	EntityData1* data = flicky2->Data1;

	if (!isRanddone)
	{
		short getRandomFlick = rand() % 3 + 1;
		switch (getRandomFlick)
		{
		default:
		case 1:
			data = flicky2->Data1;
			break;
		case 2:
			data = flicky3->Data1;
			break;
		case 3:
			data = flicky4->Data1;
			break;
		}

		isRanddone = true;
	}


	if (data->NextAction == 0) {
		ObjectMaster* enemy = Flicky_GetClosestEnemy(&CurrentFlicky->Position);
		if (enemy) {
			data->LoopData = (Loop*)enemy;
			data->NextAction = 1;
		}
		else {
			data->NextAction = 0;
			UpdateFlickyPosition();
		}

	}
	else {
		if (data->LoopData) {
			ObjectMaster* enemy = (ObjectMaster*)data->LoopData;

			if (!enemy->Data1) {
				data->NextAction = 0;
				data->LoopData = nullptr;
				data->Action = 4;
				return;
			}

			if (IsPointInsideSphere(&enemy->Data1->Position, &CurrentFlicky->Position, 15)) {
				KillEnemiesInSphere(&enemy->Data1->Position, 5);

				if (++data->InvulnerableTime > 200 || enemy->MainSub == BoaBoa_Main) {
					data->LoopData = nullptr;
					data->InvulnerableTime = 0;

					if (enemy->MainSub) {
						if (enemy->SETData.SETData) {
							UpdateSetDataAndDelete(enemy);
							isRanddone = false;
						}
						else {
							CheckThingButThenDeleteObject(enemy);
							isRanddone = false;
						}
					}

					return;
				}
			}

			data->Position = enemy->Data1->Position;
			float dist = GetDistance(&data->Position, &CurrentFlicky->Position);

			dist = fmax(fmin(dist, 200), 80);


			CurrentFlicky->Position = GetPathPosition(&CurrentFlicky->Position, &data->Position, dist / 3000);
			CurrentFlicky->Rotation.y = -fPositionToRotation(&CurrentFlicky->Position, &data->Position).y + 0x4000;
		}
		else {
			data->NextAction = 0;
			data->Action = 4;
			UpdateFlickyPosition();
		}
	}
	
	ObjectFunc(origin, FlickyObj_Main_t.Target());
	origin(obj);
}