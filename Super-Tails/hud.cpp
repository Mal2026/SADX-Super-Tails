#include "stdafx.h"
#include "ScaleInfo.h"

extern NJS_TEXLIST SuperMilesLifeIcon_Textlist;

static NJS_TEXANIM SUPERMILES_EXTRA_TEXANIM = { 0x20, 0x20, 0, 0, 0, 0, 0x100, 0x100, 0, 0x20 };
static NJS_SPRITE SUPERMILES_EXTRA_SPRITE = { {0}, 1.0f, 1.0f, 0, &SuperMilesLifeIcon_Textlist, &SUPERMILES_EXTRA_TEXANIM };

bool isInActionStage()
{
	return CurrentLevel < LevelIDs_StationSquare || CurrentLevel > LevelIDs_Past && CurrentLevel < LevelIDs_SSGarden;
}

void __cdecl DisplaySuperMiles_Icon()
{
	if (!lifeIcon || !IsIngame() && GameState != 16 || !playertwp[0] || playertwp[0]->counter.b[1] != Characters_Tails || !isSuperTails || !isInActionStage())
		return;

	help.PushScaleUI(uiscale::Align_Automatic, false, 1.0f, 1.0f);
	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

	njSetTexture(&SuperMilesLifeIcon_Textlist);

	if (HideLives >= 0)
	{
		SUPERMILES_EXTRA_SPRITE.p.x = 16.0f;

		if (isDCConv && !hudPlus)
			SUPERMILES_EXTRA_SPRITE.p.y = VerticalStretch * 480.0f - 80.0f;
		else
			SUPERMILES_EXTRA_SPRITE.p.y = VerticalStretch * 480.0f - 64.0f;

		njDrawSprite2D_ForcePriority(&SUPERMILES_EXTRA_SPRITE, 0, -1.501f, NJD_SPRITE_ALPHA);
		ClampGlobalColorThing_Thing();
	}

	help.PopScaleUI();
}