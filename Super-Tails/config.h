#pragma once
#include "stdafx.h"
#include "SADXModLoader.h"


extern bool isSuper;
extern int CurrentSuperMusic;
extern int RingsNeeded;
extern int delay;
extern bool TransfoDone;
extern int CurrentSFX;
extern bool AnimationTransfo;
extern ObjectMaster* flicky1;
extern bool RingDrop;
extern int ActualSong;

FunctionPointer(ObjectMaster*, CheckLoadBird, (), 0x4C6820);
ObjectFunc(LoadBird, 0x4c6790);
ObjectFunc(Bird_Main, 0x4c63f0);
ObjectFunc(UpdateSetDataAndDelete, 0x46C150);
VoidFunc(FUN_00412ad0, 0x412ad0);
FunctionPointer(void, FUN_004314d0, (int value), 0x4314d0);
DataPointer(int, CurrentCutscene, 0x3b2c560);

void __cdecl SuperAudio_Init(const char* path, const HelperFunctions& helperFunctions);
void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions);
void SuperTails();
void SuperTails_OnFrames();
void Transformation();
void SubRings();
void unSuper();
void CallFlicky();
void DeleteFlicky();
void SoftReset_R();
void RestoreMusic();
void FlickyAction_Main_r(ObjectMaster* obj);
void Play_SuperTailsMusic();

enum SuperTailsMusic {

	None,
	Sonic2,
	Sonic3,
	SonicAndKnuckles,
	SADX,
	SA2,
	Mania,
	Random
};

enum SoundEffect {

	SADX_SFX,
	DBZ_SFX
};

