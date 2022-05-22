#include "stdafx.h"
#include "super-tails.h"

int ActualSong = 0;

Trampoline* Tails_Main_t = nullptr;
Trampoline* Tails_Display_t = nullptr;
Trampoline* Invincibility_restart_t = nullptr;

bool isDCCharUsed = false;
bool isSuperTails = false;


static void Tails_Display_r(ObjectMaster* tsk)
{
	EntityData1* data = tsk->Data1;

	isSuperTails = isPlayerOnSuperForm(data->CharIndex) == true ? 1 : 0;

	TARGET_DYNAMIC(Tails_Display)(tsk);
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

	if (isSuperTails && textureChanges) {

		if (isDCCharUsed)
			texlist = &SuperMilesDC_TEXLIST;
		else
			texlist = &SuperMilesDX_TEXLIST;
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

void unSuper(unsigned char player) {

	if (AlwaysSuperMiles)
		return;

	TailsAnimData[30].AnimationSpeed = 0;
	isSuperTails = false;

	EntityData1* data = EntityData1Ptrs[player];
	CharObj2* co2 = CharObj2Ptrs[player];

	if (!data)
		return;

	if (data->CharID == Characters_Tails) //fix an issue with charsel
		co2->PhysicsData = PhysicsArray[Characters_Tails];

	data->Status = 0;
	ForcePlayerAction(0, 24);
	EV_ClrFace(PlayerPtrs[player]);


	if (IsIngame())
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

	if (IsIngame() && CurrentSFX != None && !isPerfectChasoLevel())
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

facewk* face = 0;
uint8_t randomAngryFace = 0;
void Miles_SetAngryFace(unsigned char playerID) {

	if (!IsIngame() || EV_MainThread_ptr)
		return;

	task* player = (task*)PlayerPtrs[playerID];

	if (!player)
		return;

	int curchar = PlayerPtrs[playerID]->Data1->CharID;

	if (curchar != Characters_Tails)
		return;

	if (player->twp->mode == 1) {
		randomAngryFace = 0;
		return;
	}

	int faceaddress = (int)&player->twp->ewp->face;
	faceaddress = faceaddress + 8; //Adjust address because this is 8 bytes off
	face = (facewk*)faceaddress;
	int number = randomAngryFace + 13;
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
	EntityData1* player = EntityData1Ptrs[obj->Data1->CharIndex];

	if (!player || !IsIngame() || EV_MainThread_ptr)
		return;

	if (player->CharID != Characters_Tails) //charsel fix
		CheckThingButThenDeleteObject(obj);

	unsigned char playerID = data->CharIndex;
	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];
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

		if (!isPerfectChasoLevel()) {

			SubRings(playerID, data);

			if (CheckUntransform_Input(playerID)) {

				data->Action = playerInputCheck;
			}
			CheckSuperMusic_Restart(playerID);
		}

		co2->TailsFlightTime = 0.0f;
		Miles_SetAngryFace(playerID);
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


void Tails_Main_r(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;

	if (data->Action == 0) {
		ObjectMaster* SuperMiles_ObjManager = LoadObject((LoadObj)2, 0, SuperMiles_Manager);
		SuperMiles_ObjManager->Data1->CharIndex = data->CharIndex;
	}

	ObjectFunc(origin, Tails_Main_t->Target());
	origin(obj);

	CollisionInfo* collision_info = data->CollisionInfo;
	if (collision_info)
	{
		CollisionData* collision_data = collision_info->CollisionArray;
		if (CharObj2Ptrs[data->CharIndex]->Powerups & Powerups_Invincibility)
		{
			//fix an issue where Invincibility doesn't make the character able to damage an enemy (in Vanilla this works for everyone but tails.)
			collision_data->damage = 3 & 3 | collision_data->damage & 0xF0 | (4 * (3 & 3));
			data->CollisionInfo->CollisionArray = collision_data;
		}
	}

}

void __cdecl Init_SuperTailsTextures(const char* path, const HelperFunctions& helperFunctions) {

	HMODULE SA1Char = GetModuleHandle(L"SA1_Chars");

	if (SA1Char)
	{
		for (int i = 0; i < LengthOfArray(superTails_DCEntry); i++) {
			helperFunctions.RegisterCharacterPVM(Characters_Tails, superTails_DCEntry[i]);
		}
		isDCCharUsed = true;
	}
	else
	{
		for (int i = 0; i < LengthOfArray(superTails_DXEntry); i++) {
			helperFunctions.RegisterCharacterPVM(Characters_Tails, superTails_DXEntry[i]);
		}
		isDCCharUsed = false;
	}
}


//fix character not invincibile in superform after a restart lol
void InvincibilityRestart_r(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	char pID = data->CharIndex;

	if (CharObj2Ptrs[pID] && CharObj2Ptrs[pID]->Upgrades & Upgrades_SuperSonic)
	{
		CheckThingButThenDeleteObject(obj);
		return;
	}

	ObjectFunc(origin, Invincibility_restart_t->Target());
	origin(obj);
}


void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions)
{

	Init_SuperTailsTextures(path, helperFunctions);
	Tails_Main_t = new Trampoline((int)Tails_Main, (int)Tails_Main + 0x7, Tails_Main_r);
	Tails_Display_t = new Trampoline((int)Tails_Display, (int)Tails_Display + 0x7, Tails_Display_r);

	initFlicky();

	Invincibility_restart_t = new Trampoline((int)0x441F80, (int)0x441F85, InvincibilityRestart_r);

	//Textures init
	WriteCall((void*)0x460C71, SuperTails_PerformLightingThing);
	WriteCall((void*)0x460CBC, setSuperTailsTexture);
}