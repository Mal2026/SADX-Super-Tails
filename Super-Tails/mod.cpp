#include "stdafx.h"

HelperFunctions help;

bool MultiModEnabled = false;

extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		if (helperFunctions.Version < 13)
		{
			MessageBox(WindowHandle,
				L"Please update SADX Mod Loader. Super Tails mod requires API version 13 or newer.",
				L"Super Tails Error", MB_OK | MB_ICONERROR);
		}

		help = helperFunctions;

		if (AlwaysSuperMiles)
			RemoveLimitations = true;

		initConfig(path);

		SuperTails_Init(path, helperFunctions); //Main Code to Load Super Miles
		Audio_Init(path, helperFunctions); //Everything related to music and sound
		WaterHack_Init(); // Hack to allow the player to float on water when using Super Form.
		init_PhysicsHack();

		MultiModEnabled = GetModuleHandle(L"sadx-multiplayer") != nullptr;
	}


	__declspec(dllexport) void __cdecl OnFrame()
	{

		//SetDebugFontSize(13.0f * (unsigned short)VerticalResolution / 480.0f);
		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Current bird: %d", birdCount);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

}