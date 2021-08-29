#pragma once


#define ReplacePVM(SM, SMR) helperFunctions.ReplaceFile("system\\" SM ".pvm", "system\\textures\\" SMR ".pvmx")

bool isTailsCurChar();
bool isTailsAI(EntityData1* data1);
bool isSuperSonic(int player);