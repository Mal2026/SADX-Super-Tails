#include "stdafx.h"

int CurrentSuperMusic = Random;
int CurrentSFX = SADX_SFX;
bool AnimationTransfo = true;
bool RemoveLimitations = false;
bool AlwaysSuperMiles = false;
Buttons TransformButton = Buttons_Y;

static const Buttons ButtonsList[]
{
	Buttons_B,
	Buttons_Y,
	Buttons_X,
};

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

		TransformButton = ButtonsList[config->getInt("General", "TransformButton", 1)];
		AnimationTransfo = config->getBool("General", "AnimationTransfo", true);
		RemoveLimitations = config->getBool("General", "RemoveLimitations", false);
		AlwaysSuperMiles = config->getBool("General", "AlwaysSuperMiles", false);

		CurrentSuperMusic = config->getInt("Audio", "CurrentSuperMusic", Random);
		CurrentSFX = config->getInt("Audio", "GetVoice", SADX_SFX);
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