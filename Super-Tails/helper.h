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

extern HelperFunctions help;

#define TaskHook static FunctionHook<void, task*>
#define pNum data->counter.b[0]

ObjectFunc(UpdateSetDataAndDelete, 0x46C150);
VoidFunc(FUN_00412ad0, 0x412ad0);
TaskFunc(EV_ClrFace, 0x4310F0);
VoidFunc(crushLightOff, 0x6ED9A0);
FunctionPointer(void, crushLightOn, (float pos_x, float pos_y, float pos_z, int crushLightNum, int life,
	float start_width, float end_width, unsigned int start_col, unsigned int end_col), 0x6EDA20);
FunctionPointer(task*, COverlayCreate, (float s, float a, float r, float g, float b), 0x6EF480);

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