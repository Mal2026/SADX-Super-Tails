#include "stdafx.h"
#include "flickies.h"

ObjectMaster* flicky1;
ObjectMaster* flicky2;
ObjectMaster* flicky3;
ObjectMaster* flicky4;

ObjectMaster* flicky[4] = { flicky1, flicky2, flicky3, flicky4 };
CollisionData flickyCol = { 0, 0, 0x70, 0xE2, 0x10, {0}, 5.0, 2.0, 0.0, 0.0, 0, 0, 0 };

int randFlicky = 0;

float Flicky_GetFlightSpeed() {
	return 2.5f;
}

float Flicky_GetAttackRange() {
	return 1000.0f;
}

int turnFlicky = 0;
void TurnAndShift_EachFlicky(EntityData1* data1, EntityData1* player) {

	data1->Position.x = njSin(turnFlicky) * 5.0f;

	if (data1->CharID > 0)
		data1->Position.z = njCos(turnFlicky) * 5.0f + 4.0 * data1->CharID;
	else
		data1->Position.z = njCos(turnFlicky) * 5.0f;

	njAddVector(&data1->Position, &player->Position);
	data1->Position.y = player->Position.y + 12.0;
}

void flicky_doRotation(EntityData1* data, EntityData1* player) {

	data->Rotation.y = BAMS_SubWrap(data->Rotation.y, 0x8000 - player->Rotation.y, 1024);
	data->Rotation.x += 16;
	data->Rotation.z += 1024;
}

void flicky_TurnAround(EntityData1* data1, EntityData1* player) {

	flicky_doRotation(data1, player);

	if (CharObj2Ptrs[player->CharIndex]->Speed.x + CharObj2Ptrs[player->CharIndex]->Speed.y < 0.2f) {

		turnFlicky += 100;
		TurnAndShift_EachFlicky(data1, player);
	}
	else {
		turnFlicky = 0;
		data1->Action = flicky_followPlayer;
	}
}


void FollowPlayer(EntityData1* data, EntityData1* player, CharObj2* co2) {

	flicky_doRotation(data, player);

	NJS_VECTOR dest;

	dest.x = njCos(data->Rotation.x) * 3.0f + player->Position.x;
	dest.y = njSin(data->Rotation.z) + 12.0f + player->Position.y;

	if (data->CharID > 0) {
		dest.z = njSin(data->Rotation.x) * 3.0f + flicky[data->CharID - 1]->Data1->Position.z + 2.0f * data->CharID;
	}
	else {
		dest.z = njSin(data->Rotation.x) * 3.0f + player->Position.z;
	}

	float distance = sqrtf(powf(dest.x - data->Position.x, 2) + powf(dest.y - data->Position.y, 2) + powf(dest.z - data->Position.z, 2));

	if (distance >= 100.0f) {
		data->Position.x = dest.x;
		data->Position.y = dest.y;
		data->Position.z = dest.z;
	}
	else
	{
		data->Position.x = (dest.x - data->Position.x) * 0.25f + data->Position.x;
		data->Position.y = (dest.y - data->Position.y) * 0.25f + data->Position.y;
		data->Position.z = (dest.z - data->Position.z) * 0.25f + data->Position.z;
	}

	if (co2->Speed.x + co2->Speed.y < 0.2f) {
		data->Position = player->Position;
		data->Action = flicky_turnAround;
		return;
	}
}

void Flicky_Delete(ObjectMaster* obj) {
	for (int i = 0; i < LengthOfArray(flicky); i++) {
		if (flicky[i]) {
			CheckThingButThenDeleteObject(flicky[i]);
			flicky[i] = nullptr;
		}
	}
}

void CheckForAttack(EntityData1* data) {
	HomingAttackTarget target = GetClosestAttack(&data->Position);

	if (target.entity && target.distance < Flicky_GetAttackRange()) {
		randFlicky = rand() % 4; //unused for now
		data->InvulnerableTime = 0;
		data->Action = flicky_attack;
	}

}

void FlickyAttack(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;
	HomingAttackTarget target = GetClosestAttack(&data1->Position);

	if (data1->Status & StatusFlicky_Attacked) {
		if (rand() % (500 / max(1, min(50, 99))) == 0) {
			data1->Status &= ~(StatusFlicky_Attacked);
		}
		else {
			data1->Action = flicky_followPlayer;
			data1->Status &= ~(StatusFlicky_Attacked);
			return;
		}
	}

	if (target.entity && target.distance < Flicky_GetAttackRange()) {
		float dist = GetDistance(&data1->Position, &target.entity->Position) + 30.0f;
		LookAt(&data1->Position, &target.entity->Position, &data1->Rotation.x, &data1->Rotation.y);
		MoveForward(data1, Flicky_GetFlightSpeed());
		data1->Rotation.x = 0;

		if (dist < 50.0f) {
			data1->CollisionInfo->CollisionArray[0].attr &= 0xFFFFFFEF;
			data1->CollisionInfo->CollisionArray[0].damage |= 3u;

			if (++data1->InvulnerableTime == 120) { //failsafe to prevent softlock
				data1->CollisionInfo->CollisionArray[0].attr |= 0x400u;
				data1->CollisionInfo->CollisionArray[0].damage &= 0xFCu;
				data1->Action = flicky_followPlayer;
				data1->Status &= ~(StatusFlicky_Attacked);

			}
		}
		else {
			data1->CollisionInfo->CollisionArray[0].attr |= 0x400u;
			data1->CollisionInfo->CollisionArray[0].damage &= 0xFCu;
		}
	}
	else {
		data1->Action = flicky_followPlayer;
		data1->Status &= ~(StatusFlicky_Attacked);
	}
}

extern NJS_TEXLIST Flicky_TEXLIST;
void __cdecl Flicky_Display(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1 shadow;

	if (MissedFrames)
		return;

	njSetTexture(&Flicky_TEXLIST);
	SetMaterialAndSpriteColor_Float(1.0, 1.0, 1.0, 1.0);
	Direct3D_PerformLighting(2);

	njPushMatrixEx();
	njTranslateV(0, &data->Position);
	njRotateY(0, data->Rotation.y);
	NJS_ACTION Action = { obj->Data1->Object, WingAnim.motion };
	njAction_Queue(&Action, FrameCounterUnpaused % WingAnim.motion->nbFrame, QueuedModelFlagsB_EnableZWrite);
	njPopMatrixEx();

	ClampGlobalColorThing_Thing();
	Direct3D_PerformLighting(0);

	shadow.Position = data->Position;
	shadow.Rotation = data->Rotation;
	shadow.Rotation.y = -data->Rotation.y;
	DrawShadow(&shadow, 0.40000001);
}

void __cdecl Flicky_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[data->CharIndex];
	CharObj2* co2 = CharObj2Ptrs[data->CharIndex];

	if (!player || !isSuperTails) {
		DeleteObject_(obj);
		return;
	}

	switch (data->Action) {
	case flicky_turnAround:
		flicky_TurnAround(data, player);
		CheckForAttack(data);
		data->CollisionInfo->CollisionArray[0].damage = 0;
		break;
	case flicky_followPlayer:
		FollowPlayer(data, player, co2);
		CheckForAttack(data);
		data->CollisionInfo->CollisionArray[0].damage = 0;
		break;
	case flicky_attack:
		FlickyAttack(obj);
		AddToCollisionList(data);
		break;
	}

	obj->DisplaySub(obj);
}

void __cdecl Load_Miles_Flickies(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[data->CharIndex];

	if (!player) {
		DeleteObject_(obj);
		return;
	}

	data->Position = player->Position;

	if (data->CharID == 0)
		data->Position.y = data->Position.y + 12.0;

	data->Rotation.y = 0x8000 - player->Rotation.y;
	data->Object = &Wing_Model;
	Collision_Init(obj, &flickyCol, 1, 4u);
	obj->DisplaySub = Flicky_Display;
	obj->MainSub = Flicky_Main;
	obj->DeleteSub = Flicky_Delete;
}

void Call_Flickies(unsigned char player) {

	for (uint8_t i = 0; i < LengthOfArray(flicky); i++) {

		if (flicky[i]) //if flicky already exist, skip.
			continue;

		flicky[i] = LoadObject(LoadObj_Data1, 3, Load_Miles_Flickies);
		flicky[i]->Data1->CharIndex = player;
		flicky[i]->Data1->CharID = i;
	}

	return;
}

Trampoline* OhNoImDead2_t;

bool OhNoImDead2_r(EntityData1* a1, ObjectData2* a2) {

	if (a1->CollisionInfo->CollidingObject) {
		if (a1->CollisionInfo->CollidingObject->Object->MainSub == Flicky_Main) {
			EntityData1* data = a1->CollisionInfo->CollidingObject->Object->Data1;
			if (data->CollisionInfo->Object->MainSub != OParasol_Main) {
				data->Status |= StatusFlicky_Attacked;
				return true;
			}
		}
	}

	TARGET_DYNAMIC(OhNoImDead2)(a1, a2);
}

void initFlicky() {
	OhNoImDead2_t = new Trampoline(0x004CE030, 0x004CE036, OhNoImDead2_r);
	return;
}