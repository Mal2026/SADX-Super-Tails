#include "stdafx.h"
#include "super-tails.h"

int ActualSong = 0;

ObjectMaster* SuperMiles_ObjManager;
Trampoline* Tails_Main_t;
Trampoline* Miles_CheckNextActions_t;
Trampoline* Tails_Delete_t;

HelperFunctions help;

bool isDCCharUsed = false;
bool isSuperTails = false;

void SubRings(int player) {

	if (RemoveLimitations || AlwaysSuperMiles || EntityData1Ptrs[player]->CharID != Characters_Tails || !isSuperTails || isTailsAI(EntityData1Ptrs[player]) || !ControlEnabled || !TimeThing || GameState != 15)
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

	if (AlwaysSuperMiles)
		return;


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
		co2->PhysicsData = PhysicsArray[Characters_Tails];

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



void SetSuperMiles(CharObj2* co2, EntityData1* data1) {

	task* v10;
	task* v11;
	taskwk* taskw = (taskwk*)data1;


	if (IsIngame())
		PlayVoice(7001);

	co2->Upgrades |= Upgrades_SuperSonic;
	co2->Powerups |= Powerups_Invincibility;
	v10 = (task*)LoadObject(LoadObj_Data1, 2, Sonic_SuperAura_Load);
	if (v10)
	{
		v10->twp->counter.b[0] = taskw->counter.b[0];
	}
	v11 = (task*)LoadObject(LoadObj_UnknownB, 2, Miles_SuperPhysics_Load);
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

bool CheckPlayer_Input(int playerID) {

	EntityData1* data = EntityData1Ptrs[playerID];

	if (ControllerPointers[data->CharIndex]->PressedButtons & Buttons_Y && (Rings >= 50 || RemoveLimitations) || isTailsAI(data) && isPlayerOnSuperForm(0))
	{
		if (data->Action == Jumping || data->Action == Flying || data->Action >= BoardSlide && data->Action <= BoardJump || isTailsAI(data) && isPlayerOnSuperForm(0)) {

			return true;
		}
	}

	return false;
}

void SuperMiles_PlayTransfoAnimation(EntityData1* player) {

	if (AlwaysSuperMiles || !AnimationTransfo)
		return;

	TailsAnimData[30].AnimationSpeed = 0.8f; //increase speed animation
	player->Action = 75;
	CharObj2Ptrs[player->CharIndex]->AnimationThing.Index = 30;
}

void SuperMiles_Manager(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[obj->Data1->CharIndex];
	EntityData2* player2 = EntityData2Ptrs[obj->Data1->CharIndex];
	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];

	if (!player)
		return;

	switch (data->Action) {

	case superTailsSetTask:
		obj->DeleteSub = SuperTailsDelete;
		data->Action++;
	case playerInputCheck:

		if (!IsIngame() || EV_MainThread_ptr || !AlwaysSuperMiles && !ControlEnabled)
			return;

		if (CheckPlayer_Input(data->CharIndex) || AlwaysSuperMiles)
			data->Action++;

		break;
	case superTailsInit:
		data->Index = 0;
		player->Status &= ~(Status_OnPath | Status_Attack | Status_Ball);
		co2->Powerups |= Powerups_Invincibility;
		SuperMiles_PlayTransfoAnimation(player);
		data->Action++;
		break;
	case superTailsWait:
		if (++data->Index == 30)
		{
			data->Action++;
		}
		break;
	case superTailsTransfo:

		SetSuperMiles(co2, player);

		if (!isTailsAI(player)) {
			if (CurrentSuperMusic != None && CurrentSong != MusicIDs_sprsonic)
			{
				ActualSong = LastSong;
				Play_SuperTailsMusic();
			}
		}

		if (AlwaysSuperMiles)
			data->Action = 6;
		else
			data->Action++;

		break;
	case superTailsOnFrames:
		SubRings(player->CharIndex);

		//Tails_LoadAttackEffect(player2, co2, player);
		if (isSuperTails || co2->Upgrades & Upgrades_SuperSonic) {

			if (ControllerPointers[player->CharIndex]->PressedButtons & Buttons_Y || isTailsAI(player) && !isPlayerOnSuperForm(0))
			{
				if (player->Action == Flying || player->Action == Jumping || player->Action == Standing || isTailsAI(player) && !isPlayerOnSuperForm(0)) {
					unSuper(player->CharIndex);
					data->Action = playerInputCheck;
				}
			}
		}
		break;
	default:
		CheckThingButThenDeleteObject(obj);
		break;
	}
}

void SuperTailsDelete(ObjectMaster* obj) {

	unSuper(obj->Data1->CharIndex);

	if (SuperMiles_ObjManager)
		CheckThingButThenDeleteObject(SuperMiles_ObjManager);

	SuperMiles_ObjManager = nullptr;

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

	EntityData1* data = obj->Data1;
	CharObj2* co2 = CharObj2Ptrs[data->CharIndex];
	EntityData2* data2 = EntityData2Ptrs[data->CharIndex];


	if (data->Action == 0) {
		if (!SuperMiles_ObjManager) {
			SuperMiles_ObjManager = LoadObject((LoadObj)2, 1, SuperMiles_Manager);
			SuperMiles_ObjManager->Data1->CharIndex = data->CharIndex;
		}
	}

	ObjectFunc(origin, Tails_Main_t->Target());
	origin(obj);


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