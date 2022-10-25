#include "stdafx.h"


static void Miles_AfterImages(task* tsk)
{
	if (MissedFrames || (tsk->twp->wtimer & 2) != 0)
		return;

	auto wk = tsk->twp;
	auto co2 = CharObj2Ptrs[wk->value.b[0]];

	if (IsGamePaused() == false)
	{
		wk->scl.x -= 0.1f;

		if (wk->scl.x <= 0.0f)
		{
			FreeTask(tsk);
			return;
		}
	}

	if (wk->scl.x < 0.7f)
	{
		njSetTexture(getSuperTailsTex());
		BackupConstantAttr();

		njControl3D_Backup();
		njControl3D_Add(NJD_CONTROL_3D_OFFSET_MATERIAL);
		njControl3D_Add(NJD_CONTROL_3D_CONSTANT_MATERIAL);

		AddConstantAttr(0, NJD_FLAG_USE_ALPHA);
		njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
		njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
		SetMaterialAndSpriteColor_Float(wk->scl.x, 1.0f, 1.0f, 1.0f);

		njPushMatrixEx();
		njTranslateEx(&wk->pos);
		njRotateZ_(wk->ang.z);
		njRotateX_(wk->ang.x);
		njRotateY_(-0x8000 - LOWORD(wk->ang.y));

		if (co2->AnimationThing.State != 2)
			njAction_Queue(co2->AnimationThing.AnimData[co2->AnimationThing.Index].Animation, co2->AnimationThing.Frame, QueuedModelFlagsB_EnableZWrite);

		njPopMatrixEx();

		njControl3D_Restore();
		njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
		njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
		ClampGlobalColorThing_Thing();
		RestoreConstantAttr();

		if (tsk->twp->wtimer < USHRT_MAX)
			tsk->twp->wtimer++;
		else
			tsk->twp->wtimer = 0;
	}
}

static void LoadMilesAfterImages(EntityData1* data, CharObj2* co2)
{
	auto tsk = CreateElementalTask(LoadObj_Data1, tasklevel::LEV_4, Miles_AfterImages);

	if (tsk) {
		tsk->disp = Miles_AfterImages;

		auto wk = tsk->twp;

		wk->value.b[0] = data->CharIndex;
		wk->scl.x = 0.8f;
		wk->scl.y = co2->AnimationThing.Frame;
		wk->pos = data->Position;
		wk->pos.y += 4.9f;
		wk->ang = *(Angle3*)&data->Rotation;
	}
}

void CheckMilesAfterImages(EntityData1* data, CharObj2* co2)
{
	if (!data || !co2 || !IsIngame() || !isSuperTails)
		return;

	if (co2->Speed.x >= 3.0f || co2->Speed.x <= -3.0f || co2->Speed.y >= 5.0f || co2->Speed.y <= -4.0f)
	{
		LoadMilesAfterImages(data, co2);
	}
}