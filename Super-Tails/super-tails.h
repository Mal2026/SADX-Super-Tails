#pragma once

enum SuperTailsM {
	superTailsSetTask,
	playerInputCheck,
	superTailsInit,
	superTailsWait,
	superTailsTransfo,
	superTailsOnFrames,
	superTailsUntransfo,
};


NJS_TEXNAME SUPERMILES_DC[25];
NJS_TEXNAME SUPERMILES_DX[16];

NJS_TEXLIST SuperMilesDC_TEXLIST = { arrayptrandlength(SUPERMILES_DC) };
NJS_TEXLIST SuperMiles_TEXLIST = { arrayptrandlength(SUPERMILES_DX) };

NJS_TEXNAME Flicky_Tex[7];

NJS_TEXLIST Flicky_TEXLIST = { arrayptrandlength(Flicky_Tex) };

PVMEntry superTails_DCTex[] = {
	{"SUPERMILES", &SuperMilesDC_TEXLIST},
	{"flickyTex", &Flicky_TEXLIST  },
	{ "SUPERSONIC", &SUPERSONIC_TEXLIST},
};

PVMEntry superTails_DXTex[] = {
	{"SUPERMILES", &SuperMiles_TEXLIST},
	{"flickyTex", &Flicky_TEXLIST  },
	{ "SUPERSONIC", &SUPERSONIC_TEXLIST},
};

void Load_SuperPhysics(taskwk* data1);