#pragma once

extern bool isDCCharUsed;
extern int CurrentSuperMusic;
extern int RingsNeeded;
extern int CurrentSFX;
extern bool AnimationTransfo;
extern ObjectMaster* flicky1;
extern bool RingDrop;
extern int ActualSong;

extern NJS_TEXLIST FlickyDX_TEXLIST;
extern NJS_TEXLIST FlickyDC_TEXLIST;
FunctionPointer(ObjectMaster*, CheckLoadBird, (), 0x4C6820);
ObjectFunc(LoadBird, 0x4c6790);
ObjectFunc(Bird_Main, 0x4c63f0);
ObjectFunc(UpdateSetDataAndDelete, 0x46C150);
VoidFunc(FUN_00412ad0, 0x412ad0);
FunctionPointer(void, FUN_004314d0, (int value), 0x4314d0);

void __cdecl Audio_Init(const char* path, const HelperFunctions& helperFunctions);
void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions);

void SuperTailsDelete(ObjectMaster* obj);

void unSuper(int player);
void CallFlicky();
void DeleteFlicky();
void RestoreMusic();
void FlickyAction_Main_r(ObjectMaster* obj);
void Play_SuperTailsMusic();
void NjSetTexture_Flicky();
bool isTailsCurChar();

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

enum TailsActions {
	Standing = 1,
	Jumping = 6,
	Flying = 15,
	BoardSlide = 48,
	BoardJump = 52

};

