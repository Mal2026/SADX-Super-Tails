#include "stdafx.h"
#include "config.h"
using namespace std;
#include <string.h>


#define ReplaceSet(SS, SSR) helperFunctions.ReplaceFile("system\\sounddata\\bgm\\wma\\" SS ".wma", "system\\Audio\\" SSR ".adx")
#define ReplacePVM(SM, SMR) helperFunctions.ReplaceFile("system\\" SM ".pvm", "system\\textures\\" SMR ".pvmx")
bool isSuper = false;

extern int GetSuperMusic;
extern int RingsNeeded;
int RingCout = 0;
int ActualSong = 0;
extern bool RingDrop;
HelperFunctions help;
extern ObjectMaster* flicky1;

extern int delay;
extern bool TransfoDone;
extern int GetVoice;
extern int AnimationTransfo;
NJS_TEXNAME SUPERMILES_DC[25];
NJS_TEXNAME SUPERMILES_DX[16];
short blink = 0;
bool isDCCharUsed = false;




void RestoreMusic() {

	if (CurrentSong != MusicIDs_sprsonic)
		return;
	else
	{
		CurrentSong = ActualSong;
		LastSong = CurrentSong;
	}
}


void SuperTails_OnFrames()
{
	ObjectMaster* P1 = GetCharacterObject(0);

	if (P1 != nullptr && P1->Data1->CharID == Characters_Tails)
	{
		if (delay != 40)
			delay++;

		if (ControllerPointers[0]->PressedButtons & Buttons_Y && ControlEnabled)
		{
			if (!isSuper && !TransfoDone && delay == 40)
				Transformation();

			if (isSuper)
				unSuper();
		}

		if (TransfoDone && !isSuper)
			SuperTails();

	}
	return;
}


void unSuper() {

	ObjectMaster* play1 = GetCharacterObject(0);
	if (play1 != nullptr && play1->Data1->CharID == Characters_Tails && isSuper)
	{
		TailsAnimData[30].AnimationSpeed = 0;
		CharObj2Ptrs[0]->Upgrades &= ~Upgrades_SuperSonic; //unSuper
		CharObj2Ptrs[0]->Powerups &= 0x100u; //Remove invincibility
		CharObj2Ptrs[0]->PhysicsData = PhysicsArray[Characters_Tails];
		CharObj2Ptrs[0]->PhysicsData.AirAccel = 0.03099999949;
		DeleteFlicky();

		delay = 0;
		LoadPVM("MILES", &MILES_TEXLIST);

		if (GameState == 15 && ControlEnabled)
		{
			if (GetVoice)
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

	if (delay == 30 && play1 != nullptr && TransfoDone && !isSuper)
	{
		PlayVoice(7001);
		CallFlicky();
		
		if (AnimationTransfo >= 1)
		{
			TailsAnimData[30].AnimationSpeed = 0; //restore original value
			CharObj2Ptrs[0]->AnimationThing.Index = 19;
			ScreenFade_Run();
			play1->Data1->Action = 6;
		}

		if (isDCCharUsed)
		{
			MILES_TEXLIST.textures = SUPERMILES_DC;
			MILES_TEXLIST.nbTexture = LengthOfArray(SUPERMILES_DC);
			LoadPVM("SUPERMILES", &MILES_TEXLIST);
		}
		else
		{
			MILES_TEXLIST.textures = SUPERMILES_DX;
			MILES_TEXLIST.nbTexture = LengthOfArray(SUPERMILES_DX);
			LoadPVM("SUPERMILES", &MILES_TEXLIST);
		}

		LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);
		CharObj2Ptrs[0]->Upgrades |= Upgrades_SuperSonic;
		play1 = LoadObject((LoadObj)2, 2, Sonic_SuperAura_Load);
		LoadObject((LoadObj)8, 2, Sonic_SuperPhysics_Load);
		CharObj2Ptrs[0]->Powerups |= Powerups_Invincibility;
		CharObj2Ptrs[0]->PhysicsData.AirAccel = 0,5;


		if (GetSuperMusic != 0 && CurrentSong != MusicIDs_sprsonic)
		{
			ActualSong = LastSong;
			PlayMusic(MusicIDs_ThemeOfSuperSonic);
		}


		TransfoDone = false;
		isSuper = true;
	}

	return;
}


void Transformation() {

	ObjectMaster* play1 = GetCharacterObject(0);
	
	if (play1 != nullptr && play1->Data1->CharID == Characters_Tails)
	{
		if (play1->Data1->Action != 6 && play1->Data1->Action != 15) //are we jumping or flying?
			return;

		if (!isSuper && Rings >= RingsNeeded) //transformation
		{
			play1->Data1->Status &= ~(Status_Attack | Status_Ball | Status_LightDash | Status_Unknown3);

			if (AnimationTransfo >= 1)
			{
				TailsAnimData[30].AnimationSpeed = 0.8f; //incrase speed animation
				CharObj2Ptrs[0]->AnimationThing.Index = 30;
				play1->Data1->Action = 18;
				delay = 0;
			}
			else
			{
				delay = 40;
			}
			
			TransfoDone = true;
		}
	}

	return;
}



void SubRings() {

	if (RingDrop && isSuper && ControlEnabled && GameState == 15)
	{
		++RingCout %= 60;

		if (!RingCout && Rings)
		{
			AddRings(-1);
		}

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

	if (P1 != nullptr && P1->Data1->CharID == Characters_Tails || CurrentCharacter == Characters_Tails)
		unSuper();

	return DisableControl();
}

void SuperTailsCutscene_Delete() { //There is probably a nicer way to do this.

	ObjectMaster* P1 = GetCharacterObject(0);
	if (GameState == 15)
	{
		if (isSuper && (P1 != nullptr && P1->Data1->CharID == Characters_Tails) || CurrentCharacter == Characters_Tails)
		{
			TailsAnimData[30].AnimationSpeed = 0;
			CharObj2Ptrs[0]->Upgrades &= ~Upgrades_SuperSonic; //unSuper
			CharObj2Ptrs[0]->Powerups &= 0x100u;
			CharObj2Ptrs[0]->PhysicsData = PhysicsArray[Characters_Tails];
			CharObj2Ptrs[0]->PhysicsData.AirAccel = 0.03099999949;
			DeleteFlicky();
			delay = 0;
			LoadPVM("MILES", &MILES_TEXLIST);
			TransfoDone = false;
			isSuper = false;
		}
	}

	return DisableControl();
}

void __cdecl SuperAudio_Init(const char* path, const HelperFunctions& helperFunctions)
{
	switch (GetSuperMusic)
	{
	case 1: //Sonic 2
		ReplaceSet("sprsonic", "sonic2");
		break;
	case 2: //Sonic 3
		ReplaceSet("sprsonic", "sonic3");
		break;
	case 3: //Sonic & Knuckles
		ReplaceSet("sprsonic", "s&k");
		break;
	case 4: //SADX
	default:
		ReplaceSet("sprsonic", "sadx");
		break;
	case 5: //SA2
		ReplaceSet("sprsonic", "sa2");
		break;
	case 6: //Mania
		ReplaceSet("sprsonic", "mania");
		break;
	}


	if (GetVoice != 0) //dbz sound effect
	{
		helperFunctions.ReplaceFile("system\\sounddata\\voice_us\\wma\\7001.wma", "system\\sfx\\LoadAura.adx");
		helperFunctions.ReplaceFile("system\\sounddata\\voice_us\\wma\\7002.wma", "system\\sfx\\unsuper.adx");
	}
	else //Sonic sound effect
	{
		helperFunctions.ReplaceFile("system\\sounddata\\voice_us\\wma\\7001.wma", "system\\sfx\\s3kTransfo.adx");
	}

	return;
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
