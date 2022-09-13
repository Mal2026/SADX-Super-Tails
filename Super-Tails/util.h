#pragma once


#define ReplacePVM(SM, SMR) helperFunctions.ReplaceFile("system\\" SM ".pvm", "system\\textures\\" SMR ".pvmx")

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

bool isTailsCurChar();
bool isTailsAI(EntityData1* data1);
bool isPlayerOnSuperForm(int player);

#pragma pack(push, 1)
struct ObjUnknownB
{
	int Time;
	int Index;
	int Mode;
	int field_C;
};
#pragma pack(pop)

HomingAttackTarget GetClosestAttack(NJS_VECTOR* pos);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
void MoveForward(taskwk* entity, float speed);
NJS_VECTOR GetPlayerPoint(EntityData1* player);
NJS_VECTOR GetPointToFollow(NJS_VECTOR* pos, NJS_VECTOR* dir, Rotation3* rot);
void LoadAnimationFile(AnimationFile** info, const char* name);
bool isPerfectChasoLevel();