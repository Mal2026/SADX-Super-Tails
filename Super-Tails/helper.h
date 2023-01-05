#pragma once

extern int CurrentSuperMusic;
extern bool RemoveLimitations;
extern int CurrentSFX;
extern bool AnimationTransfo;
extern int ActualSong;
extern bool isSuperTails;
extern bool AlwaysSuperMiles;
extern bool superAura;
extern bool customPhysics;
extern int charType;
extern Buttons TransformButton;
extern bool MultiModEnabled;
extern bool lifeIcon;

extern bool isDCConv;
extern bool hudPlus;

extern HelperFunctions help;

#define TaskHook static FunctionHook<void, task*>
#define pNum data->counter.b[0]

ObjectFunc(UpdateSetDataAndDelete, 0x46C150);
VoidFunc(FUN_00412ad0, 0x412ad0);


void __cdecl Audio_Init(const char* path, const HelperFunctions& helperFunctions);
void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions);

void unSuper(unsigned char player);
void Call_Flickies(unsigned char player);
void RestoreMusic();
void Play_SuperTailsMusic();
bool isTailsCurChar();
void initFlicky();
void CheckSuperMusic_Restart(int playerID);
void WaterHack_Init();
void init_PhysicsHack();
void __cdecl DisplaySuperMiles_Icon();
void initConfig(const char* path);

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
	BoardFall = 51,
	BoardJump

};


enum charTypeE
{
	none,
	Dreamcast,
	DX,
};