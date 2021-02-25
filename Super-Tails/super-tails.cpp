#include "stdafx.h"

#define ReplacePVM(SM, SMR) helperFunctions.ReplaceFile("system\\" SM ".pvm", "system\\textures\\" SMR ".pvmx")

int ActualSong = 0;

ObjectMaster* SuperTails_Main;
Trampoline* Tails_Main_t;
Trampoline* Miles_CheckNextActions_t;
Trampoline* Tails_Delete_t;

HelperFunctions help;
NJS_TEXNAME SUPERMILES_DC[25];
NJS_TEXNAME SUPERMILES_DX[16];

NJS_TEXLIST SuperMilesDC_TEXLIST = { arrayptrandlength(SUPERMILES_DC) };
NJS_TEXLIST SuperMiles_TEXLIST = { arrayptrandlength(SUPERMILES_DX) };

NJS_TEXNAME Flicky_DC[53];
NJS_TEXNAME Flicky_DX[31];

NJS_TEXLIST FlickyDX_TEXLIST = { arrayptrandlength(Flicky_DX) };
NJS_TEXLIST FlickyDC_TEXLIST = { arrayptrandlength(Flicky_DC) };
bool isDCCharUsed = false;
bool isSuperTails = false;

bool isTailsCurChar() {

	EntityData1* P1 = EntityData1Ptrs[0];
	EntityData1* AI = EntityData1Ptrs[1];

	if (P1 != nullptr && P1->CharID == Characters_Tails || AI != nullptr && AI->CharID == Characters_Tails)
		return true;

	return false;
}

bool isTailsAI(EntityData1* data1) {

	if (TailsAI_ptr != nullptr && data1->CharIndex == 1 && data1->CharID == Characters_Tails)
		return true;

	return false;
}

bool isSuperSonic(int player) {

	if (player == Characters_Sonic && CharObj2Ptrs[player]->Upgrades & Upgrades_SuperSonic)
		return true;

	return false;
}

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
		DeleteFlicky();


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
	//CallFlicky();
	data1->Action = 1;
	isSuperTails = true;

	return;
}



void SuperTails_Manager(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[obj->Data1->CharIndex];
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
		if (isSuperTails || co2->Upgrades & Upgrades_SuperSonic) {

			if (ControllerPointers[player->CharIndex]->PressedButtons & Buttons_Y || isTailsAI(player) && !isSuperSonic(0))
			{
				if (player->Action == Flying || player->Action == Jumping || player->Action == Standing) {
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

void SuperTails_PerformLightingThing() { //idk what it does exactly, but Sonic uses it for SS so...
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
			DeleteFlicky();
			isSuperTails = false;
		}
	}

	return DisableControl();
}


void LoadCharTextures_R() {

	LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);

	if (isDCCharUsed) {
		LoadPVM("SUPERMILES", &SuperMilesDC_TEXLIST);
		LoadPVM("AMY_R", &FlickyDC_TEXLIST);
	}
	else {
		LoadPVM("SUPERMILES", &SuperMiles_TEXLIST);
		LoadPVM("AMY_R", &FlickyDX_TEXLIST);
	}

	ResetGravity();
}


void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions)
{
	HMODULE SA1Char = GetModuleHandle(L"SA1_Chars");

	if (SA1Char)
	{
		ReplacePVM("SUPERMILES", "SUPERMILES_DC");
		ReplacePVM("AMY_R", "AMY_DC_R");
		isDCCharUsed = true;
	}
	else
	{
		ReplacePVM("SUPERMILES", "SUPERMILES_DX");
		ReplacePVM("AMY_R", "AMY_R");
		isDCCharUsed = false;
	}

	Tails_Main_t = new Trampoline((int)Tails_Main, (int)Tails_Main + 0x7, Tails_Main_r);
	Tails_Delete_t = new Trampoline((int)Tails_Delete, (int)Tails_Delete + 0x6, Tails_Delete_r);

	//Textures init
	WriteCall((void*)0x4c6316, NjSetTexture_Flicky);
	WriteCall((void*)0x415387, LoadCharTextures_R);

	WriteCall((void*)0x460C71, SuperTails_PerformLightingThing);
	WriteCall((void*)0x460CBC, setSuperTailsTexture);

	//Delete Super Tails properly when you die, restart, game over or before a cutscene start.
	WriteCall((void*)0x413728, SuperTailsCutscene_Delete);
}