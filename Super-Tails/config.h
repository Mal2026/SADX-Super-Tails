#pragma once
#include "stdafx.h"
#include "SADXModLoader.h"

FunctionPointer(ObjectMaster*, CheckLoadBird, (), 0x4C6820);

ObjectFunc(LoadBird, 0x4c6790);
ObjectFunc(Bird_Main, 0x4c63f0);
extern bool isSuper;


void __cdecl SuperAudio_Init(const char* path, const HelperFunctions& helperFunctions);
void __cdecl SuperTails_Init(const char* path, const HelperFunctions& helperFunctions);
void SuperTails();
ObjectFunc(UpdateSetDataAndDelete, 0x46C150);
void SuperTails_OnFrames();
void Transformation();
void SubRings();
void unSuper();
void CallFlicky();
void DeleteFlicky();
void SoftReset_R();

void FlickyAction_Main_r(ObjectMaster* obj);

VoidFunc(FUN_00412ad0, 0x412ad0);

FunctionPointer(void, FUN_004314d0, (int value), 0x4314d0);
DataPointer(int, CurrentCutscene, 0x3b2c560);