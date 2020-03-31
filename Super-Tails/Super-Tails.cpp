#include "stdafx.h"
#include "config.h"
#include <IniFile.hpp>


int GetSuperMusic = 4;
int GetVoice = 0;
int AnimationTransfo = 2;
int RingsNeeded = 50;
bool RingDrop = true;


int delay = 40;
bool TransfoDone = false;


extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{

		if (helperFunctions.Version < 7)
		{
			MessageBox(WindowHandle,
				L"Please update SADX Mod Loader. Super Tails mod requires API version 7 or newer.",
				L"Super Tails Error", MB_OK | MB_ICONERROR);
			Exit();
			return;
		}

		//Ini file configuration

		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		GetSuperMusic = config->getInt("General", "GetSuperMusic", 4);
		GetVoice = config->getInt("General", "GetVoice", 0);
		AnimationTransfo = config->getInt("General", "AnimationTransfo", 1);
		RingsNeeded = config->getInt("General", "RingsNeeded", 50);
		RingDrop = config->getBool("General", "RingDrop", true);
		delete config;

		
		SuperTails_Init(path, helperFunctions);
	}


	__declspec(dllexport) void __cdecl OnFrame()
	{

		//SetDebugFontSize(13.0f * (unsigned short)VerticalResolution / 480.0f);
		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Current bird: %d", birdCount);

		if (GameState == 15 && TimeThing)
			SuperTails_OnFrames();

		SubRings();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

}