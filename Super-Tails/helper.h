#pragma once

extern bool isDCCharUsed;
extern int CurrentSuperMusic;
extern bool RemoveLimitations;
extern int CurrentSFX;
extern bool AnimationTransfo;
extern int ActualSong;
extern bool isSuperTails;
extern bool AlwaysSuperMiles;
extern bool superAura;
extern bool customPhysics;
extern Buttons TransformButton;

FunctionPointer(ObjectMaster*, CheckLoadBird, (), 0x4C6820);
ObjectFunc(LoadBird, 0x4c6790);
ObjectFunc(Bird_Main, 0x4c63f0);
ObjectFunc(UpdateSetDataAndDelete, 0x46C150);
VoidFunc(FUN_00412ad0, 0x412ad0);
FunctionPointer(void, FUN_004314d0, (int value), 0x4314d0);
FunctionPointer(bool, Check_CutsceneSkip, (), 0x42FB00);

void __cdecl Audio_Init(const char* path, const HelperFunctions& helperFunctions);
void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions);

void SuperTailsDelete(ObjectMaster* obj);

void unSuper(int player);
void Call_Flickies(int player);
void RestoreMusic();
void Play_SuperTailsMusic();
bool isTailsCurChar();
void initFlicky();
void CheckSuperMusic_Restart(int playerID);

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

	SADX_SFX = 1,
	DBZ_SFX
};

enum TailsActions {
	Standing = 1,
	Jumping = 6,
	Flying = 15,
	BoardSlide = 48,
	BoardJump = 52

};
