#include "stdafx.h"
#include "flickies.h"


ObjectMaster* flicky1;
ObjectMaster* flicky2;
ObjectMaster* flicky3;
ObjectMaster* flicky4;

ObjectMaster* flicky[4] = { flicky1, flicky2, flicky3, flicky4 };

NJS_VECTOR	bombpos;	
float		bombsize;
extern int delay;
int randFlicky = 0;

void Flicky_Delete(ObjectMaster* obj) {
	for (int i = 0; i < LengthOfArray(flicky); i++) {
		CheckThingButThenDeleteObject(flicky[i]);
		flicky[i] = nullptr;
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
	NJS_ACTION Action = { obj->Data1->Object, WingAnim.motion};
	njAction_Queue(&Action, FrameCounterUnpaused % WingAnim.motion->nbFrame, QueuedModelFlagsB_EnableZWrite);
	njPopMatrixEx();

	ClampGlobalColorThing_Thing();
	Direct3D_PerformLighting(0);

	if (IsGamePaused())
	{
		shadow.Position.y = data->Position.y;
		shadow.Position.z = data->Position.z;
		shadow.Position.x = data->Position.x;
		shadow.Rotation = data->Rotation;
		shadow.Rotation.y = -data->Rotation.y;
		DrawShadow(&shadow, 0.40000001);
	}
}

void __cdecl Flicky_Main(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[data->CharIndex];
	CharObj2* co2 = CharObj2Ptrs[data->CharIndex];

	if (!player || !isSuperTails) {
		DeleteObject_(obj);
		return;
	}


	data->Rotation.y = BAMS_SubWrap(data->Rotation.y, 0x8000 - player->Rotation.y, 1024);
	data->Rotation.x += 16;
	data->Rotation.z += 1024;

	NJS_VECTOR dest;

	if (data->CharID == 0) {
		dest.x = njCos(data->Rotation.x) * 3.0f + player->Position.x;
		dest.y = njSin(data->Rotation.z) + 12.0f + player->Position.y;
		dest.z = njSin(data->Rotation.x) * 3.0f + player->Position.z;
	}
	else {

		if (!flicky[1] || !flicky[2] || !flicky[3])
			return;

		dest.x = njCos(data->Rotation.x) * 3.0f + flicky[data->CharID - 1]->Data1->Position.x;
		dest.y = njSin(data->Rotation.z) + flicky[data->CharID - 1]->Data1->Position.y;
		dest.z = njSin(data->Rotation.x) * 3.0f + flicky[data->CharID - 1]->Data1->Position.z;
	}

	float distance = sqrtf(powf(dest.x - data->Position.x, 2) + powf(dest.y - data->Position.y, 2) + powf(dest.z - data->Position.z, 2));

	if (distance >= 200.0f) {
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

	obj->DisplaySub(obj);
}

void __cdecl Load_Miles_Flickies(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[data->CharIndex];

	if (!player) {
		DeleteObject_(obj);
		return;
	}

	data->Position.x = player->Position.x;
	data->Position.y = player->Position.y;
	data->Position.z = player->Position.z;

	if (data->CharID == 0)
		data->Position.y = data->Position.y + 12.0;

	data->Rotation.y = 0x8000 - player->Rotation.y;
	data->Object = &Wing_Model;
	obj->DisplaySub = Flicky_Display;
	obj->MainSub = Flicky_Main;
	obj->DeleteSub = Flicky_Delete;
}

void Call_Flickies(int player) {

	for (uint8_t i = 0; i < LengthOfArray(flicky); i++) {
		flicky[i] = LoadObject(LoadObj_Data1, 3, Load_Miles_Flickies);
		flicky[i]->Data1->CharIndex = player;
		flicky[i]->Data1->CharID = i;
	}

	return;
}