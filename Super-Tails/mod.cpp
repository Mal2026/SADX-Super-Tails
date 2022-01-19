#include "stdafx.h"

int CurrentSuperMusic = Random;
int CurrentSFX = SADX_SFX;
bool AnimationTransfo = true;
bool RemoveLimitations = false;
bool AlwaysSuperMiles = false;
bool superAura = true;
bool customPhysics = true;
bool textureChanges = true;

Buttons TransformButton = Buttons_Y;
HelperFunctions help;


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

		help = helperFunctions;

		//Ini file configuration
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");

		textureChanges = config->getBool("General", "textureChanges", true);
		TransformButton = ButtonsList[config->getInt("General", "TransformButton", 1)];
		AnimationTransfo = config->getBool("General", "AnimationTransfo", true);
		RemoveLimitations = config->getBool("General", "RemoveLimitations", false);
		AlwaysSuperMiles = config->getBool("General", "AlwaysSuperMiles", false);
		superAura = config->getBool("General", "superAura", true);
		customPhysics = config->getBool("General", "customPhysics", true);

		CurrentSuperMusic = config->getInt("Audio", "CurrentSuperMusic", Random);
		CurrentSFX = config->getInt("Audio", "GetVoice", SADX_SFX);
		delete config;

		if (AlwaysSuperMiles)
			RemoveLimitations = true;

		SuperTails_Init(path, helperFunctions); //Main Code to Load Super Miles
		Audio_Init(path, helperFunctions); //Everything related to music and sound
		WaterHack_Init(); // Hack to allow the player to float on water when using Super Form.
	}


	__declspec(dllexport) void __cdecl OnFrame()
	{

		//SetDebugFontSize(13.0f * (unsigned short)VerticalResolution / 480.0f);
		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Current bird: %d", birdCount);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

}