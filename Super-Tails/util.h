#pragma once


#define ReplacePVM(SM, SMR) helperFunctions.ReplaceFile("system\\" SM ".pvm", "system\\textures\\" SMR ".pvmx")

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
