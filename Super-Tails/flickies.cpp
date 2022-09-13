#include "stdafx.h"
#include "flickies.h"

task* flicky1 = nullptr;
task* flicky2 = nullptr;
task* flicky3 = nullptr;
task* flicky4 = nullptr;

task* flicky[4] = { flicky1, flicky2, flicky3, flicky4 };
CCL_INFO flickyCol = { 0, 0, 0x70, 0xE2, 0x10, {0}, 5.0, 2.0, 0.0, 0.0, 0, 0, 0 };

static FunctionHook<int, EntityData1*, enemywk*> OhNoImDead2_t((intptr_t)OhNoImDead);
int randFlicky = 0;

float Flicky_GetFlightSpeed() {
	return 2.5f;
}

float Flicky_GetAttackRange() {
	return 1000.0f;
}

int turnFlicky = 0;
void TurnAndShift_EachFlicky(taskwk* data1, taskwk* player) {

	data1->pos.x = njSin(turnFlicky) * 5.0f;

	if (data1->counter.b[1] > 0)
		data1->pos.z = njCos(turnFlicky) * 5.0f + 4.0 * data1->counter.b[1];
	else
		data1->pos.z = njCos(turnFlicky) * 5.0f;

	njAddVector(&data1->pos, &player->pos);
	data1->pos.y = player->pos.y + 12.0f;
}

void flicky_doRotation(taskwk* data, taskwk* player) {

	data->ang.y = BAMS_SubWrap(data->ang.y, 0x8000 - player->ang.y, 1024);
	data->ang.x += 16;
	data->ang.z += 1024;
}

void flicky_TurnAround(taskwk* data1, taskwk* player) {

	flicky_doRotation(data1, player);

	char pnum = player->counter.b[0];
	if (CharObj2Ptrs[pnum]->Speed.x + CharObj2Ptrs[pnum]->Speed.y < 0.2f) {

		turnFlicky += 100;
		TurnAndShift_EachFlicky(data1, player);
	}
	else {
		turnFlicky = 0;
		data1->mode = flicky_followPlayer;
	}
}


void FollowPlayer(taskwk* data, taskwk* player, playerwk* co2) {

	flicky_doRotation(data, player);

	NJS_VECTOR dest;

	dest.x = njCos(data->ang.x) * 3.0f + player->pos.x;
	dest.y = njSin(data->ang.z) + 12.0f + player->pos.y;

	if (data->counter.b[1] > 0) {
		dest.z = njSin(data->ang.x) * 3.0f + flicky[data->counter.b[1] - 1]->twp->pos.z + 2.0f * data->counter.b[1];
	}
	else {
		dest.z = njSin(data->ang.x) * 3.0f + player->pos.z;
	}

	float distance = sqrtf(powf(dest.x - data->pos.x, 2) + powf(dest.y - data->pos.y, 2) + powf(dest.z - data->pos.z, 2));

	if (distance >= 100.0f) {
		data->pos.x = dest.x;
		data->pos.y = dest.y;
		data->pos.z = dest.z;
	}
	else
	{
		data->pos.x = (dest.x - data->pos.x) * 0.25f + data->pos.x;
		data->pos.y = (dest.y - data->pos.y) * 0.25f + data->pos.y;
		data->pos.z = (dest.z - data->pos.z) * 0.25f + data->pos.z;
	}

	if (co2->spd.x + co2->spd.y < 0.2f) {
		data->pos = player->pos;
		data->mode = flicky_turnAround;
		return;
	}
}

void Flicky_Delete(task* obj) {
	for (int i = 0; i < LengthOfArray(flicky); i++) {
		if (flicky[i]) {
			FreeTask(flicky[i]);
			flicky[i] = nullptr;
		}
	}
}

void CheckForAttack(taskwk* data) {
	HomingAttackTarget target = GetClosestAttack(&data->pos);

	if (target.entity && target.distance < Flicky_GetAttackRange()) {
		randFlicky = rand() % 4; //unused for now
		data->wtimer = 0;
		data->mode = flicky_attack;
	}

}

void FlickyAttack(task* obj) {

	taskwk* data1 = obj->twp;
	HomingAttackTarget target = GetClosestAttack(&data1->pos);

	if (data1->flag & StatusFlicky_Attacked) {
		if (rand() % (500 / max(1, min(50, 99))) == 0) {
			data1->flag &= ~(StatusFlicky_Attacked);
		}
		else {
			data1->mode = flicky_followPlayer;
			data1->flag &= ~(StatusFlicky_Attacked);
			return;
		}
	}

	if (target.entity && target.distance < Flicky_GetAttackRange()) {
		float dist = GetDistance(&data1->pos, &target.entity->Position) + 30.0f;
		LookAt(&data1->pos, &target.entity->Position, &data1->ang.x, &data1->ang.y);
		MoveForward(data1, Flicky_GetFlightSpeed());
		data1->ang.x = 0;

		if (dist < 50.0f) {
			data1->cwp->info[0].attr &= 0xFFFFFFEF;
			data1->cwp->info[0].damage |= 3u;

			if (++data1->wtimer == 120) { //failsafe to prevent softlock
				data1->cwp->info[0].attr |= 0x400u;
				data1->cwp->info[0].damage &= 0xFCu;
				data1->mode = flicky_followPlayer;
				data1->flag &= ~(StatusFlicky_Attacked);

			}
		}
		else {
			data1->cwp->info[0].attr |= 0x400u;
			data1->cwp->info[0].damage &= 0xFCu;
		}
	}
	else {
		data1->mode = flicky_followPlayer;
		data1->flag &= ~(StatusFlicky_Attacked);
	}
}

extern NJS_TEXLIST Flicky_TEXLIST;
void __cdecl Flicky_Display(task* obj) {

	auto data = obj->twp;
	taskwk shadow;

	if (MissedFrames)
		return;

	njSetTexture(&Flicky_TEXLIST);
	SetMaterialAndSpriteColor_Float(1.0, 1.0, 1.0, 1.0);
	Direct3D_PerformLighting(2);

	njPushMatrixEx();
	njTranslateV(0, &data->pos);
	njRotateY(0, data->ang.y);
	NJS_ACTION Action = { &Wing_Model, WingAnim.motion };
	njAction_Queue(&Action, FrameCounterUnpaused % WingAnim.motion->nbFrame, QueuedModelFlagsB_EnableZWrite);
	njPopMatrixEx();

	ClampGlobalColorThing_Thing();
	Direct3D_PerformLighting(0);

	shadow.pos = data->pos;
	shadow.ang = data->ang;
	shadow.ang.y = -data->ang.y;
	DrawShadow((EntityData1*)&shadow, 0.40000001f);
}

void __cdecl Flicky_Main(task* obj) {
	auto data = obj->twp;
	auto player = playertwp[pNum];
	auto co2 = playerpwp[pNum];

	if (!player || !isSuperTails) {
		FreeTask(obj);
		return;
	}

	switch (data->mode) {
	case flicky_turnAround:
		flicky_TurnAround(data, player);
		CheckForAttack(data);
		data->cwp->info[0].damage = 0;
		break;
	case flicky_followPlayer:
		FollowPlayer(data, player, co2);
		CheckForAttack(data);
		data->cwp->info[0].damage = 0;
		break;
	case flicky_attack:
		FlickyAttack(obj);
		EntryColliList(data);
		break;
	}

	obj->disp(obj);
}

void __cdecl Load_Miles_Flickies(task* obj) {
	auto data = obj->twp;
	auto player = EntityData1Ptrs[pNum];

	if (!player) {
		FreeTask(obj);
		return;
	}

	data->pos = player->Position;

	if (data->counter.b[1] == 0)
		data->pos.y = data->pos.y + 12.0;

	data->ang.y = 0x8000 - player->Rotation.y;
	data->timer.ptr = &Wing_Model;
	CCL_Init(obj, &flickyCol, 1, 4u);
	obj->disp = Flicky_Display;
	obj->exec = Flicky_Main;
	obj->dest = Flicky_Delete;
}

void Call_Flickies(unsigned char player) {

	for (uint8_t i = 0; i < LengthOfArray(flicky); i++) {

		if (flicky[i]) //if flicky already exist, skip.
			continue;

		flicky[i] = CreateElementalTask(LoadObj_Data1, 3, Load_Miles_Flickies);
		flicky[i]->twp->counter.b[0] = player;
		flicky[i]->twp->counter.b[1] = i;
	}

	return;
}



int OhNoImDead2_r(EntityData1* a1, enemywk* a2) {

	if (isSuperTails) {

		if (a1 && a1->CollisionInfo && a1->CollisionInfo->CollidingObject && a1->CollisionInfo->CollidingObject->Object) {

			if (a1->CollisionInfo->CollidingObject->Object->MainSub == (ObjectFuncPtr)Flicky_Main) {

				EntityData1* data = a1->CollisionInfo->CollidingObject->Object->Data1;

				if (data->CollisionInfo->Object->MainSub != OParasol_Main) {
					data->Status |= StatusFlicky_Attacked;
					return 1;
				}
			}

		}
	}

	return OhNoImDead2_t.Original(a1, a2);
}

void initFlicky() {
	OhNoImDead2_t.Hook(OhNoImDead2_r);
	return;
}