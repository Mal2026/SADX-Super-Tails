#include "stdafx.h"
#include "super-tails.h"

int ActualSong = 0;

Trampoline* Tails_Main_t;
Trampoline* ResetAngle_t;

ObjectMaster* damageObj;
CollisionData damageCol = { 0, 0, 0x40, 0x41, 0x400, {0}, 4.0, 4.0, 0.0, 0.0, 0, 0, 0 };

bool isDCCharUsed = false;
bool isSuperTails = false;

void DeleteDamageCol(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;
	data->CollisionInfo = nullptr;
	Collision_Free(obj);
	damageObj = nullptr;
}

void DoDamageCol(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;
	EntityData1* player = EntityData1Ptrs[data->CharIndex];

	if (!player || !IsIngame() || EV_MainThread_ptr)
		CheckThingButThenDeleteObject(obj);

	CharObj2* co2 = CharObj2Ptrs[data->CharIndex];

	if (data->Action == 0) {

		obj->DeleteSub = DeleteDamageCol;
		Collision_Init(obj, &damageCol, 1, 4u);
		data->CollisionInfo->CollisionArray[0].attr &= 0xFFFFFFEF;
		data->CollisionInfo->CollisionArray[0].damage |= 3u;
		data->Action++;
	}

	if (data->Action == 1) {
		if (isSuperTails) {
			data->Position = player->CollisionInfo->CollisionArray->center;
			AddToCollisionList(data);
		}
		else {
			CheckThingButThenDeleteObject(obj);
		}
	}

}

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

	CheckThingButThenDeleteObject(damageObj);
	TailsAnimData[30].AnimationSpeed = 0;
	isSuperTails = false;

	EntityData1* data = EntityData1Ptrs[player];
	CharObj2* co2 = CharObj2Ptrs[player];

	if (!data)
		return;

	co2->PhysicsData = PhysicsArray[Characters_Tails];
	data->Status = 0;
	ForcePlayerAction(0, 24);


	if (IsIngame())
	{
		if (CurrentSFX == DBZ_SFX)
			PlayVoice(7002);

		RestoreMusic();
	}

	co2->Upgrades &= ~Upgrades_SuperSonic;
	co2->Powerups &= ~Powerups_Invincibility;
	return;
}


void Load_SuperAura(taskwk* data1) {

	if (!superAura) {
		return;
	}

	task* superAura = (task*)LoadObject(LoadObj_Data1, 2, Sonic_SuperAura_Load);
	if (superAura)
	{
		superAura->twp->counter.b[0] = data1->counter.b[0];
	}
}

void Load_SuperPhysics(taskwk* data1) {

	task* v11 = (task*)LoadObject(LoadObj_UnknownB, 2, Miles_SuperPhysics_Load);
	if (v11)
	{
		v11->awp->work.ub[0] = data1->counter.b[0];
	}
}

void SetSuperMiles(CharObj2* co2, EntityData1* data1) {


	taskwk* taskw = (taskwk*)data1;

	if (IsIngame() && CurrentSFX != None)
		PlayVoice(7001);

	co2->Upgrades |= Upgrades_SuperSonic;

	Load_SuperAura(taskw);
	Load_SuperPhysics(taskw);
	Call_Flickies(data1->CharIndex);

	if (!EV_MainThread_ptr && CurrentLevel >= 0 && CurrentLevel <= LevelIDs_E101R) {
		damageObj = LoadObject((LoadObj)2, 1, DoDamageCol);
		damageObj->Data1->CharIndex = data1->CharIndex;
	}
	data1->Action = 1;
	isSuperTails = true;

	return;
}

bool CheckUntransform_Input(int playerID) {

	EntityData1* player = EntityData1Ptrs[playerID];

	if (AlwaysSuperMiles)
		return false;

	if (isTailsAI(player) && !isPlayerOnSuperForm(0)) {
		unSuper(player->CharIndex);
		return true;
	}

	if (ControllerPointers[playerID]->PressedButtons & TransformButton)
	{
		if (player->Action == Flying || player->Action == Jumping) {
			unSuper(player->CharIndex);
			return true;
		}
	}

	return false;
}


bool CheckPlayer_Input(int playerID) {

	EntityData1* data = EntityData1Ptrs[playerID];

	if (isTailsAI(data) && isPlayerOnSuperForm(0)) {
		return true;
	}

	if (ControllerPointers[data->CharIndex]->PressedButtons & TransformButton && (Rings >= 50 || RemoveLimitations))
	{
		if (data->Action == Jumping || data->Action == Flying || data->Action == BoardFall || data->Action == BoardJump) {

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

	if (!player || !IsIngame() || EV_MainThread_ptr)
		return;

	if (player->CharID != Characters_Tails) //charsel fix
		CheckThingButThenDeleteObject(obj);

	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];
	int timer = 30;

	switch (data->Action) {

	case superTailsSetTask:
		obj->DeleteSub = SuperTailsDelete;
		data->Action++;
	case playerInputCheck:

		if (!AlwaysSuperMiles && !ControlEnabled)
			return;

		if (CheckPlayer_Input(data->CharIndex) || AlwaysSuperMiles)
			data->Action++;

		break;
	case superTailsInit:
		data->Index = 0;
		player->Status = 0;
		co2->Powerups |= Powerups_Invincibility;
		SuperMiles_PlayTransfoAnimation(player);

		data->Action++;
		break;
	case superTailsWait:

		if (AlwaysSuperMiles)
			timer = 10;

		if (++data->Index == timer)
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

		data->Action++;
		break;
	case superTailsOnFrames:
		SubRings(player->CharIndex);
		co2->TailsFlightTime = 0.0f;
		if (CheckUntransform_Input(player->CharIndex)) {

			data->Action = playerInputCheck;
		}
		CheckSuperMusic_Restart(player->CharIndex);
		break;
	default:
		CheckThingButThenDeleteObject(obj);
		break;
	}
}

void SuperTailsDelete(ObjectMaster* obj) {

	unSuper(obj->Data1->CharIndex);
	MusicList[MusicIDs_sprsonic].Name = "sprsonic";
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

	if (data->Action == 0) {
		ObjectMaster* SuperMiles_ObjManager = LoadObject((LoadObj)2, 0, SuperMiles_Manager);
		SuperMiles_ObjManager->Data1->CharIndex = data->CharIndex;
	}

	ObjectFunc(origin, Tails_Main_t->Target());
	origin(obj);
}

static void __cdecl ResetAngle_r(EntityData1* data, EntityData2* data2, CharObj2* co2)
{
	if (CurrentLevel != LevelIDs_PerfectChaos && co2->Upgrades & Upgrades_SuperSonic)
	{
		float v4; // ecx
		float v5; // eax
		float v6; // ecx
		NJS_VECTOR a2a; // [esp+4h] [ebp-Ch] BYREF
		taskwk* twk = (taskwk*)data;

		a2a.x = co2->Speed.x;
		v4 = co2->Speed.z;
		a2a.y = co2->Speed.y;
		a2a.z = v4;
		if (co2->PhysicsData.Run2 * co2->PhysicsData.Run2 >= a2a.z * a2a.z + a2a.y * a2a.y + a2a.x * a2a.x)
		{
			PConvertVector_P2G(twk, &a2a);
			data->Rotation.x = BAMS_SubWrap(data->Rotation.x, GravityAngle_Z, 2048);
			data->Rotation.z = BAMS_SubWrap(data->Rotation.z, GravityAngle_X, 2048);
			PConvertVector_G2P(twk, &a2a);
			v5 = a2a.y;
			v6 = a2a.z;
			co2->Speed.x = a2a.x;
			co2->Speed.y = v5;
			co2->Speed.z = v6;
		}
	}
	else
	{
		TARGET_DYNAMIC(ResetAngle)(data, data2, co2);
	}
}


void __cdecl Init_SuperTailsTextures(const char* path, const HelperFunctions& helperFunctions) {

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
}

void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions)
{

	Init_SuperTailsTextures(path, helperFunctions);
	Tails_Main_t = new Trampoline((int)Tails_Main, (int)Tails_Main + 0x7, Tails_Main_r);

	bool bSS = GetModuleHandle(L"Better-Super-Sonic");

	if (!bSS) {
		ResetAngle_t = new Trampoline(0x443AD0, 0x443AD7, ResetAngle_r);
	}

	initFlicky();

	//Textures init
	WriteCall((void*)0x460C71, SuperTails_PerformLightingThing);
	WriteCall((void*)0x460CBC, setSuperTailsTexture);
}