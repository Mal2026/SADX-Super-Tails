#include "stdafx.h"
#include "super-tails.h"

int ActualSong = 0;

ObjectMaster* SuperTails_Main;
Trampoline* Tails_Main_t;
Trampoline* Miles_CheckNextActions_t;
Trampoline* Tails_Delete_t;

HelperFunctions help;

bool isDCCharUsed = false;
bool isSuperTails = false;


void SubRings(int player) {

	if (!RingDrop || EntityData1Ptrs[player]->CharID != Characters_Tails || !isSuperTails || isTailsAI(EntityData1Ptrs[player]) || !ControlEnabled || !TimeThing || GameState != 15)
		return;

	if (FrameCounterUnpaused % 60 == 0 && Rings > 0) {
		AddRings(-1);
	}

	if (Rings <= 0) {
		unSuper(player);
	}

	return;
}


void unSuper(int player) {

	EntityData1* data = EntityData1Ptrs[player];
	CharObj2* co2 = CharObj2Ptrs[player];
	if (isSuperTails)
	{
		TailsAnimData[30].AnimationSpeed = 0;
		co2->Upgrades &= ~Upgrades_SuperSonic;
		co2->Powerups &= 0x100u;
		data->Action = 1;
		co2->IdleTime = 0;
		co2->AnimationThing.Index = 1;

		int status = data->Status;
		bool v5 = (status & Status_Ball) == 0;

		if (!v5)
		{
			data->Status = status & ~(Status_Attack | Status_Ball);
		}
		data->Status &= ~Status_OnPath;

		if (GameState == 15 && ControlEnabled)
		{
			if (CurrentSFX)
				PlayVoice(7002);

			RestoreMusic();
		}

		isSuperTails = false;
	}

	return;
}

void EffBarrierPosSet(EntityData1* a1, EntityData1* Player)
{
	float v3; // eax
	Angle v4; // eax
	Angle v5; // eax
	NJS_VECTOR a2a; // [esp+4h] [ebp-Ch] BYREF

	v3 = Player->Scale.x;
	a2a.z = 0.0;
	a2a.y = 0.0;
	a2a.x = 0.0;
	a1->Scale.x = v3;
	a1->Scale.y = GetCharObj2(0)->PhysicsData.CollisionSize * a1->Scale.x * 0.60000002;
	njPushMatrix(_nj_unit_matrix_);
	njTranslateV(0, &Player->Position);
	v4 = Player->Rotation.z;
	if (v4)
	{
		njRotateZ(0, (unsigned __int16)v4);
	}
	v5 = Player->Rotation.x;
	if (v5)
	{
		njRotateX(0, (unsigned __int16)v5);
	}
	njTranslate(0, 0.0, a1->Scale.y, 0.0);
	njCalcPoint(0, &a2a, &a1->Position);
	njPopMatrix(1u);
}


void SetSuperMiles(CharObj2* co2, EntityData1* data1) {

	task* v10;
	task* v11;
	taskwk* taskw = (taskwk*)data1;

	PlayVoice(7001);
	co2->Upgrades |= Upgrades_SuperSonic;
	co2->Powerups |= Powerups_Invincibility;
	v10 = (task*)LoadObject(LoadObj_Data1, 2, Sonic_SuperAura_Load);
	if (v10)
	{
		v10->twp->counter.b[0] = taskw->counter.b[0];
	}
	v11 = (task*)LoadObject(LoadObj_UnknownB, 2, Sonic_SuperPhysics_Load);
	if (v11)
	{
		v11->awp->work.ub[0] = taskw->counter.b[0];
	}
	Call_Flickies(data1->CharIndex);
	data1->Action = 1;
	isSuperTails = true;

	return;
}

void MilesDoCollisionAttackStuff(EntityData1* data, motionwk2* data2, CharObj2* a2) {

	Angle v3; // eax
	Angle v4; // eax
	Angle v5; // eax
	ObjectMaster* v6; // eax
	ObjectMaster* v7; // eax
	ObjectMaster* v8; // eax
	ObjectMaster* v9; // eax
	float v10; // eax
	int v11; // ecx
	float v12; // edx
	float v13; // eax
	float v14; // ecx
	float v15; // [esp+0h] [ebp-30h]
	NJS_VECTOR v16; // [esp+0h] [ebp-30h]
	float v17; // [esp+4h] [ebp-2Ch]
	int v18; // [esp+8h] [ebp-28h]
	Rotation a3a; // [esp+Ch] [ebp-24h] BYREF
	Rotation vectorB; // [esp+18h] [ebp-18h] BYREF
	Rotation a2a; // [esp+24h] [ebp-Ch] BYREF

	data->Status |= Status_Attack;
	for (int i = 0; i < 4; i++) {
		data->CollisionInfo->CollisionArray[i].damage &= 0xFCu;
		data->CollisionInfo->CollisionArray[i].damage |= 0xCu;
		data->CollisionInfo->CollisionArray[i].damage |= 0xEF;
		data->CollisionInfo->CollisionArray[i].center = data->Position;
		data->CollisionInfo->CollisionArray[i].center.y += 5;
		data->CollisionInfo->CollisionArray[i].attr &= 0xFFFFFFEF;
	}
}

void SuperTails_Manager(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[obj->Data1->CharIndex];
	EntityData2* player2 = EntityData2Ptrs[obj->Data1->CharIndex];
	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];

	switch (data->Action) {
	case 0:
		data->Index = 0;
		TailsAnimData[30].AnimationSpeed = 0.8f; //increase speed animation
		player->Status &= ~(Status_OnPath | Status_Attack | Status_Ball);
		obj->DeleteSub = SuperTailsDelete;
		player->Action = 75;
		co2->Powerups |= Powerups_Invincibility;
		CharObj2Ptrs[player->CharIndex]->AnimationThing.Index = 30;
		data->Action = 1;
		break;
	case 1:
		if (++data->Index == 30)
		{
			data->Action = 2;
		}
		break;
	case 2:

		SetSuperMiles(co2, player);
			
		if (!isTailsAI(player)) {
			if (CurrentSuperMusic != None && CurrentSong != MusicIDs_sprsonic) 
			{
				ActualSong = LastSong;
				Play_SuperTailsMusic();
			}
		}
		data->Action = 3;
		break;
	case 3:
		SubRings(player->CharIndex);

		//Tails_LoadAttackEffect(player2, co2, player);
		if (isSuperTails || co2->Upgrades & Upgrades_SuperSonic) {

			if (ControllerPointers[player->CharIndex]->PressedButtons & Buttons_Y || isTailsAI(player) && !isSuperSonic(0))
			{
				if (player->Action == Flying || player->Action == Jumping || player->Action == Standing || isTailsAI(player) && !isSuperSonic(0)) {
					unSuper(player->CharIndex);
					data->Action = 4;
				}
			}
		}
		break;
	case 4:
		CheckThingButThenDeleteObject(obj);
		break;
	}


}

void SuperTailsDelete(ObjectMaster* obj) {

	unSuper(obj->Data1->CharIndex);

	if (SuperTails_Main)
		CheckThingButThenDeleteObject(SuperTails_Main);

	SuperTails_Main = nullptr;

}

void Tails_Delete_r(ObjectMaster* obj) {

	SuperTailsDelete(obj);

	ObjectFunc(origin, Tails_Delete_t->Target());
	origin(obj);
}

void SuperTails_PerformLightingThing() {
	if (isSuperTails)
		Direct3D_PerformLighting(4);
	else
		Direct3D_PerformLighting(2);
}

// Sets the texture list to use when rendering.
Sint32 __cdecl setSuperTailsTexture(NJS_TEXLIST* texlist)
{

	if (isSuperTails) {
		if (isDCCharUsed)
			texlist = &SuperMilesDC_TEXLIST;
		else
			texlist = &SuperMiles_TEXLIST;
	}

	return njSetTexture(texlist);
}

void Tails_Main_r(ObjectMaster* obj) {

	ObjectFunc(origin, Tails_Main_t->Target());
	origin(obj);

	EntityData1* data = obj->Data1;
	CharObj2* co2 = CharObj2Ptrs[data->CharIndex];
	EntityData2* data2 = EntityData2Ptrs[data->CharIndex];

	if (GameState == 15 && !EV_MainThread_ptr && ControlEnabled) {

		if (ControllerPointers[data->CharIndex]->PressedButtons & Buttons_Y && Rings >= RingsNeeded || isTailsAI(data) && isSuperSonic(0))
		{
			if (data->Action == Jumping || data->Action == Flying || data->Action >= BoardSlide && data->Action <= BoardJump || isTailsAI(data) && isSuperSonic(0)) {

				if (!SuperTails_Main) {
					SuperTails_Main = LoadObject((LoadObj)2, 1, SuperTails_Manager);
					SuperTails_Main->Data1->CharIndex = data->CharIndex;
				}
			}
		}
	}
}



void SuperTailsCutscene_Delete() { //There is probably a nicer way to do this.

	ObjectMaster* P1 = GetCharacterObject(0);
	if (GameState == 15)
	{
		if (isSuperTails && (isTailsCurChar()) || CurrentCharacter == Characters_Tails)
		{
			TailsAnimData[30].AnimationSpeed = 0;
			CharObj2Ptrs[0]->Upgrades &= ~Upgrades_SuperSonic; //unSuper
			CharObj2Ptrs[0]->Powerups &= 0x100u;
			CharObj2Ptrs[0]->PhysicsData = PhysicsArray[Characters_Tails];
			CharObj2Ptrs[0]->PhysicsData.AirAccel = 0.03099999949;
			isSuperTails = false;
		}
	}

	return DisableControl();
}



void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions)
{
	HMODULE SA1Char = GetModuleHandle(L"SA1_Chars");

	if (SA1Char)
	{
		ReplacePVM("SUPERMILES", "SUPERMILES_DC");

		for (int i = 0; i < LengthOfArray(superTails_DCTex); i++) {
			helperFunctions.RegisterCharacterPVM(Characters_Tails, superTails_DCTex[i]);
		}
		isDCCharUsed = true;
	}
	else
	{
		ReplacePVM("SUPERMILES", "SUPERMILES_DX");

		for (int i = 0; i < LengthOfArray(superTails_DXTex); i++) {
			helperFunctions.RegisterCharacterPVM(Characters_Tails, superTails_DXTex[i]);
		}
		isDCCharUsed = false;
	}

	Tails_Main_t = new Trampoline((int)Tails_Main, (int)Tails_Main + 0x7, Tails_Main_r);
	Tails_Delete_t = new Trampoline((int)Tails_Delete, (int)Tails_Delete + 0x6, Tails_Delete_r);


	//Textures init
	WriteCall((void*)0x460C71, SuperTails_PerformLightingThing);
	WriteCall((void*)0x460CBC, setSuperTailsTexture);

	//Delete Super Tails properly when you die, restart, game over or before a cutscene start.
	WriteCall((void*)0x413728, SuperTailsCutscene_Delete);
}