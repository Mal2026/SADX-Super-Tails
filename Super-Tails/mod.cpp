#include "stdafx.h"

int CurrentSuperMusic = Random;
int CurrentSFX = 0;
bool AnimationTransfo = true;
bool RemoveLimitations = false;
bool AlwaysSuperMiles = false;

extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{

		if (helperFunctions.Version < 7)
		{
			MessageBox(WindowHandle,
				L"Please update SADX Mod Loader. Super Tails mod requires API version 7 or newer.",
				L"Super Tails Error", MB_OK | MB_ICONERROR);
		}

		//Ini file configuration

		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		CurrentSuperMusic = config->getInt("General", "CurrentSuperMusic", Random);
		CurrentSFX = config->getInt("General", "GetVoice", SADX_SFX);
		AnimationTransfo = config->getBool("General", "AnimationTransfo", true);
		RemoveLimitations = config->getBool("General", "RemoveLimitations", false);
		AlwaysSuperMiles = config->getBool("General", "AlwaysSuperMiles", false);
		delete config;

		if (AlwaysSuperMiles)
			RemoveLimitations = true;

		SuperTails_Init(path, helperFunctions);
		Audio_Init(path, helperFunctions);
	}


	__declspec(dllexport) void __cdecl OnFrame()
	{

		//SetDebugFontSize(13.0f * (unsigned short)VerticalResolution / 480.0f);
		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Current bird: %d", birdCount);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

}