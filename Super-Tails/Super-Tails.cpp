#include "stdafx.h"
#include "config.h"
#include <string.h>
#define ReplacePVM(SM, SMR) helperFunctions.ReplaceFile("system\\" SM ".pvm", "system\\textures\\" SMR ".pvmx")

bool isSuper = false;
int RingCout = 0;
int ActualSong = 0;

HelperFunctions help;
NJS_TEXNAME SUPERMILES_DC[25];
NJS_TEXNAME SUPERMILES_DX[16];

bool isDCCharUsed = false;

bool isTailsCurChar() {

	ObjectMaster* P1 = GetCharacterObject(0);

	if (P1 != nullptr && P1->Data1->CharID == Characters_Tails)
		return true;

	return false;
}


void SuperTails_OnFrames()
{
	if (isTailsCurChar())
	{
		if (delay != 40)
			delay++;

		if (ControllerPointers[0]->PressedButtons & Buttons_Y && ControlEnabled)
		{
			if (!isSuper && !TransfoDone && delay == 40)
				Transformation();

			if (isSuper && delay > 10)
				unSuper();
		}

		if (TransfoDone && !isSuper)
			SuperTails();
	}

	return;
}


void unSuper() {

	if (isTailsCurChar() && isSuper)
	{
		TailsAnimData[30].AnimationSpeed = 0;
		CharObj2Ptrs[0]->Upgrades &= ~Upgrades_SuperSonic; //unSuper
		CharObj2Ptrs[0]->Powerups &= 0x100u; //Remove invincibility
		CharObj2Ptrs[0]->PhysicsData = PhysicsArray[Characters_Tails];
		CharObj2Ptrs[0]->PhysicsData.AirAccel = 0.03099999949;
		DeleteFlicky();
		delay = 0;

		njReleaseTexture(&MILES_TEXLIST);
		LoadPVM("MILES", &MILES_TEXLIST);
		njSetTexture(&MILES_TEXLIST);

		if (GameState == 15 && ControlEnabled)
		{
			if (CurrentSFX)
				PlayVoice(7002);

			RestoreMusic();
		}

		TransfoDone = false;
		isSuper = false;
	}

	return;
}


void SuperTails() {

	ObjectMaster* play1 = GetCharacterObject(0);

	if (play1 != nullptr && TransfoDone && !isSuper)
		if (AnimationTransfo && delay == 30 || !AnimationTransfo)
		{
			PlayVoice(7001);
			CallFlicky();
			
			if (AnimationTransfo)
			{
				TailsAnimData[30].AnimationSpeed = 0; //restore original value
				CharObj2Ptrs[0]->AnimationThing.Index = 19;
				ScreenFade_Run();
				play1->Data1->Action = 6;
			}
	

			njReleaseTexture(&MILES_TEXLIST);
			LoadPVM("SUPERMILES", &MILES_TEXLIST);
			njSetTexture(&MILES_TEXLIST);

			LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);
			CharObj2Ptrs[0]->Upgrades |= Upgrades_SuperSonic;
			play1 = LoadObject((LoadObj)2, 2, Sonic_SuperAura_Load);
			LoadObject((LoadObj)8, 2, Sonic_SuperPhysics_Load);
			CharObj2Ptrs[0]->Powerups |= Powerups_Invincibility;
			CharObj2Ptrs[0]->PhysicsData.AirAccel = 0,5;
	
	
			if (CurrentSuperMusic != None && CurrentSong != MusicIDs_sprsonic)
			{
				ActualSong = LastSong;
				Play_SuperTailsMusic();
			}
	
			TransfoDone = false;
			isSuper = true;
		}

	return;
}


void Transformation() {
	ObjectMaster* play1 = GetCharacterObject(0);
	int P1CurAction = play1->Data1->Action;

	if (isTailsCurChar())
	{
		if (P1CurAction != 6 && P1CurAction != 15 && P1CurAction != 52) //are we jumping or flying?
			return;

		if (!isSuper && Rings >= RingsNeeded) //transformation
		{
			if (P1CurAction == 52)
				play1->Data1->Action = 24; //Remove snowboard

			play1->Data1->Status &= ~(Status_Attack | Status_Ball | Status_LightDash | Status_Unknown3);

			if (AnimationTransfo)
			{
				TailsAnimData[30].AnimationSpeed = 0.8f; //incrase speed animation
				CharObj2Ptrs[0]->AnimationThing.Index = 30;
				play1->Data1->Action = 18;
				delay = 0;
			}
			
			TransfoDone = true;
		}
	}

	return;
}



void SubRings() {

	if (isTailsCurChar() && RingDrop && isSuper && ControlEnabled && GameState == 15)
	{
		++RingCout %= 60;

		if (!RingCout && Rings)
			AddRings(-1);

		if (!Rings)
			unSuper();
	}

	return;
}


void SoftReset_R() {

	if (LevelCopy != 0)
		unSuper();

	return FUN_00412ad0();
}


void SuperTailsDelete() {

	ObjectMaster* P1 = GetCharacterObject(0);

	if (isTailsCurChar() || CurrentCharacter == Characters_Tails)
		unSuper();

	return DisableControl();
}

void SuperTailsCutscene_Delete() { //There is probably a nicer way to do this.

	ObjectMaster* P1 = GetCharacterObject(0);
	if (GameState == 15)
	{
		if (isSuper && (isTailsCurChar()) || CurrentCharacter == Characters_Tails)
		{
			TailsAnimData[30].AnimationSpeed = 0;
			CharObj2Ptrs[0]->Upgrades &= ~Upgrades_SuperSonic; //unSuper
			CharObj2Ptrs[0]->Powerups &= 0x100u;
			CharObj2Ptrs[0]->PhysicsData = PhysicsArray[Characters_Tails];
			CharObj2Ptrs[0]->PhysicsData.AirAccel = 0.03099999949;
			DeleteFlicky();
			njReleaseTexture(&MILES_TEXLIST);
			delay = 0;
			LoadPVM("MILES", &MILES_TEXLIST);
			TransfoDone = false;
			isSuper = false;
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
		ReplacePVM("AMY_R", "AMY_DC_R");
		
		isDCCharUsed = true;
	}
	else
	{
		ReplacePVM("SUPERMILES", "SUPERMILES_DX");
		ReplacePVM("AMY_R", "AMY_R");
		isDCCharUsed = false;
	}

	SuperAudio_Init(path, helperFunctions);
	WriteCall((void*)0x42ca4f, SoftReset_R); //Reset value and stuff properly when you Soft Reset and quit.	

	//Delete Super Tails properly when you die, restart, game over or before a cutscene start.
	WriteCall((void*)0x4169dc, SuperTailsDelete);
	WriteCall((void*)0x416ce3, SuperTailsDelete);
	WriteCall((void*)0x41771c, SuperTailsDelete);
	WriteCall((void*)0x413728, SuperTailsCutscene_Delete);
}
