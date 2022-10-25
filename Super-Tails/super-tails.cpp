#include "stdafx.h"
#include "super-tails.h"

int ActualSong = 0;

TaskHook Tails_Main_t(MilesTalesPrower);
TaskHook Tails_Display_t(MilesDisplay);
TaskHook Invincibility_restart_t((intptr_t)0x441F80);

bool isSuperTails = false;

static void Miles_Display_r(task* tsk)
{
	auto data = tsk->twp;

	isSuperTails = isPlayerOnSuperForm(pNum) == true ? 1 : 0;

	Tails_Display_t.Original(tsk);
}

NJS_TEXLIST* getSuperTailsTex()
{
	switch (charType)
	{
	case Dreamcast:
		return &SuperMilesDC_TEXLIST;
	case DX:
		return &SuperMilesDX_TEXLIST;
	case none:
	default:
		return &MILES_TEXLIST;
	}
}

// Sets the texture list to use when rendering.
Sint32 __cdecl setSuperTailsTexture(NJS_TEXLIST* texlist)
{
	if (isSuperTails && charType != none) {
		texlist = getSuperTailsTex();
	}

	return njSetTexture(texlist);
}

void SubRings(unsigned char player, EntityData1* data) {

	if (RemoveLimitations || AlwaysSuperMiles || EntityData1Ptrs[player]->CharID != Characters_Tails || !isSuperTails || isTailsAI(EntityData1Ptrs[player]) || !ControlEnabled || !TimeThing || GameState != 15)
		return;

	if (FrameCounterUnpaused % 60 == 0 && Rings > 0) {
		AddRings(-1);
	}

	if (Rings <= 0) {
		data->Action = superTailsUntransfo;
	}

	return;
}

void unSuper(unsigned char pnum) {

	if (AlwaysSuperMiles)
		return;

	TailsAnimData[30].AnimationSpeed = 0;
	isSuperTails = false;

	auto player = playertp[pnum];

	EntityData1* data = EntityData1Ptrs[pnum];
	CharObj2* co2 = CharObj2Ptrs[pnum];

	if (!data)
		return;

	if (data->CharID == Characters_Tails) //fix an issue with charsel
		co2->PhysicsData = PhysicsArray[Characters_Tails];

	data->Status = 0;
	EV_ClrFace(player);
	ForcePlayerAction(0, 24);

	if (IsIngame() && !pnum)
	{
		if (CurrentSFX == DBZ_SFX)
			PlayVoice(7002);

		RestoreMusic();

		co2->Upgrades &= ~Upgrades_SuperSonic;
		co2->Powerups &= ~Powerups_Invincibility;
	}

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


void SetSuperMiles(CharObj2* co2, EntityData1* data1) {


	taskwk* taskw = (taskwk*)data1;

	if (IsIngame() && CurrentSFX != None && !isPerfectChasoLevel() && !isTailsAI(data1))
		PlayVoice(7001);

	co2->Upgrades |= Upgrades_SuperSonic;

	Load_SuperAura(taskw);
	Load_SuperPhysics(taskw);
	Call_Flickies(data1->CharIndex);
	data1->Action = 1;

	return;
}

void SuperMiles_PlayTransfoAnimation(EntityData1* player) {

	if (AlwaysSuperMiles || !AnimationTransfo)
		return;

	TailsAnimData[30].AnimationSpeed = 0.8f; //increase speed animation
	player->Action = 75;
	CharObj2Ptrs[player->CharIndex]->AnimationThing.Index = 30;
}

bool CheckUntransform_Input(unsigned char playerID) {

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


bool CheckPlayer_Input(unsigned char playerID) {

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


void Miles_SetAngryFace(unsigned char playerID) {

	if (!IsIngame() || EV_MainThread_ptr)
		return;

	auto player = playertp[playerID];

	if (!player)
		return;

	auto data = player->twp;

	char curchar = playertwp[playerID]->counter.b[1];

	if (curchar != Characters_Tails)
		return;

	auto face = &player->twp->ewp->face;
	char number = 13;

	if (data->mode > 2)
		number = 0;

	face->old = number;
	face->__new = number;
	face->frame = 1;
	face->nbFrame = 90000;
}

void SuperTailsDelete(ObjectMaster* obj) {

	unSuper(obj->Data1->CharIndex);
	MusicList[MusicIDs_sprsonic].Name = "sprsonic";
}

void SuperMiles_Manager(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;
	auto pnum = obj->Data1->CharIndex;
	EntityData1* player = EntityData1Ptrs[pnum];


	if (!player || !IsIngame() || EV_MainThread_ptr)
		return;


	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];

	if (player->CharID != Characters_Tails || !co2) { //charsel fix
		CheckThingButThenDeleteObject(obj);
		return;
	}

	unsigned char playerID = data->CharIndex;

	int timer = 30;


	switch (data->Action) {

	case superTailsSetTask:
		obj->DeleteSub = SuperTailsDelete;
		data->Action++;
		break;
	case playerInputCheck:

		if (!AlwaysSuperMiles && !ControlEnabled)
			return;

		if (CheckPlayer_Input(playerID) || AlwaysSuperMiles || isPerfectChasoLevel())
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

		if (!isTailsAI(player) && !pnum) {
			if (CurrentSuperMusic != None && CurrentSong != MusicIDs_sprsonic)
			{
				ActualSong = LastSong;
				Play_SuperTailsMusic();
			}
		}

		data->Action++;
		break;
	case superTailsOnFrames:

		if (!isPerfectChasoLevel()) {

			SubRings(playerID, data);

			if (CheckUntransform_Input(playerID)) {

				data->Action = playerInputCheck;
			}
			CheckSuperMusic_Restart(0);
		}

		co2->TailsFlightTime = 0.0f;
		Miles_SetAngryFace(playerID);
		CheckMilesAfterImages(player, co2);
		break;
	case superTailsUntransfo:
		unSuper(playerID);
		data->Action = playerInputCheck;
		break;
	default:
		CheckThingButThenDeleteObject(obj);
		break;
	}
}


void Tails_Main_r(task* obj) {

	auto data = obj->twp;
	char pnum = pNum;

	if (!data->mode) {
		ObjectMaster* SuperMiles_ObjManager = LoadObject((LoadObj)2, 0, SuperMiles_Manager);
		SuperMiles_ObjManager->Data1->CharIndex = pnum;
	}

	Tails_Main_t.Original(obj);

	auto col = data->cwp;
	if (col)
	{
		auto collision_data = col->info;
		if (playerpwp[pnum]->item & Powerups_Invincibility)
		{
			//fix an issue where Invincibility doesn't make the character able to damage an enemy (in Vanilla, this works for everyone but tails.)
			collision_data->damage = 3 & 3 | collision_data->damage & 0xF0 | (4 * (3 & 3));
			data->cwp->info = collision_data;
		}
	}

}

void __cdecl Init_SuperTailsTextures(const char* path, const HelperFunctions& helperFunctions) {

	if (charType == Dreamcast)
	{
		for (int i = 0; i < LengthOfArray(superTails_DCEntry); i++) {
			helperFunctions.RegisterCharacterPVM(Characters_Tails, superTails_DCEntry[i]);
		}
	}
	else if (charType == DX)
	{
		for (int i = 0; i < LengthOfArray(superTails_DXEntry); i++) {
			helperFunctions.RegisterCharacterPVM(Characters_Tails, superTails_DXEntry[i]);
		}
	}
	else {
		for (int i = 0; i < LengthOfArray(superTails_Entry); i++) {
			helperFunctions.RegisterCharacterPVM(Characters_Tails, superTails_Entry[i]);
		}
	}
}


//fix character not invincibile in superform after a restart lol
void InvincibilityRestart_r(task* obj)
{
	auto data = obj->twp;
	char pID = pNum;

	if (playerpwp[pID] && playerpwp[pID]->equipment & Upgrades_SuperSonic)
	{
		FreeTask(obj);
		return;
	}

	Invincibility_restart_t.Original(obj);
}


void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions)
{
	Init_SuperTailsTextures(path, helperFunctions);
	Tails_Main_t.Hook(Tails_Main_r);
	Tails_Display_t.Hook(Miles_Display_r);

	initFlicky();

	Invincibility_restart_t.Hook(InvincibilityRestart_r);

	//Textures init
	WriteCall((void*)0x460CBC, setSuperTailsTexture);
}