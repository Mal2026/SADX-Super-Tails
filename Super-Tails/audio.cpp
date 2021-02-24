#include "stdafx.h"

#define ReplaceMusic(SS, SSR) helperFunctions.ReplaceFile("system\\sounddata\\bgm\\wma\\" SS ".wma", "system\\Audio\\" SSR ".adx")
#define AddSound1(A, B) helperFunctions.ReplaceFile("system\\sounddata\\voice_us\\wma\\" A ".wma", "system\\sfx\\" B ".adx");
#define AddSound2(C, D) helperFunctions.ReplaceFile("system\\sounddata\\voice_jp\\wma\\" C ".wma", "system\\sfx\\" D ".adx");


const char* SuperTailsMusic_Array[7] = {
	"none", "sonic2", "sonic3", "s&k",
	"sadx", "sa2", "mania"
};

const char* GetSuperTails_Music() {

	if (CurrentSuperMusic == Random)
		return SuperTailsMusic_Array[rand() % 6 + 1];

	return SuperTailsMusic_Array[(CurrentSuperMusic)];
}

void Play_SuperTailsMusic() {

	MusicList[MusicIDs_sprsonic].Name = GetSuperTails_Music();
	PlayMusic(MusicIDs_sprsonic);
	return;
}


void RestoreMusic() {

	if (CurrentSong != MusicIDs_sprsonic)
		return;
	else
	{
		CurrentSong = ActualSong;
		LastSong = CurrentSong;
	}
}

void __cdecl SuperAudio_Init(const char* path, const HelperFunctions& helperFunctions)
{
	if (CurrentSFX != SADX_SFX) //dbz sound effect
	{
		AddSound1("7001", "LoadAura");
		AddSound2("7001", "LoadAura");
		AddSound1("7002", "unsuper");
		AddSound2("7002", "unsuper");
	}
	else //Sonic sound effect
	{
		AddSound1("7001", "s3kTransfo");
		AddSound2("7001", "s3kTransfo");
	}

	return;
}