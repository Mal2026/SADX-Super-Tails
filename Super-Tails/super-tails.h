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

NJS_TEXNAME Miles_LifeIconTex[1];
NJS_TEXNAME SUPERMILES_DC[25];
NJS_TEXNAME SUPERMILES_DX[16];

NJS_TEXLIST SuperMilesDC_TEXLIST = { arrayptrandlength(SUPERMILES_DC) };
NJS_TEXLIST SuperMilesDX_TEXLIST = { arrayptrandlength(SUPERMILES_DX) };

NJS_TEXNAME Flicky_Tex[7];

NJS_TEXLIST Flicky_TEXLIST = { arrayptrandlength(Flicky_Tex) };

NJS_TEXLIST SuperMilesLifeIcon_Textlist = { arrayptrandlength(Miles_LifeIconTex) };

PVMEntry superTails_DCEntry[] = {
	{"SUPERMILES_DC", &SuperMilesDC_TEXLIST},
	{"flickyTex", &Flicky_TEXLIST  },
	{"SUPERSONIC", &SUPERSONIC_TEXLIST},
	{"MilesLifeIcon", &SuperMilesLifeIcon_Textlist}
};

PVMEntry superTails_DXEntry[] = {
	{"SUPERMILES_DX", &SuperMilesDX_TEXLIST},
	{"flickyTex", &Flicky_TEXLIST  },
	{"SUPERSONIC", &SUPERSONIC_TEXLIST},
	{"MilesLifeIcon", &SuperMilesLifeIcon_Textlist}
};

//if player set "no textures changes"
PVMEntry superTails_Entry[] = {
	{"flickyTex", &Flicky_TEXLIST  },
	{"SUPERSONIC", &SUPERSONIC_TEXLIST},
	{"MilesLifeIcon", &SuperMilesLifeIcon_Textlist},
};

void Load_SuperPhysics(taskwk* data1);
void CheckMilesAfterImages(EntityData1* data, CharObj2* co2);
