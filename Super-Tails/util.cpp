#include "stdafx.h"



bool isTailsCurChar() {

	EntityData1* P1 = EntityData1Ptrs[0];
	EntityData1* AI = EntityData1Ptrs[1];

	if (P1 != nullptr && P1->CharID == Characters_Tails || AI != nullptr && AI->CharID == Characters_Tails)
		return true;

	return false;
}

bool isTailsAI(EntityData1* data1) {

	if (TailsAI_ptr != nullptr && data1->CharIndex == 1 && data1->CharID == Characters_Tails)
		return true;

	return false;
}

bool isPlayerOnSuperForm(int player) {

	if (CharObj2Ptrs[player] && CharObj2Ptrs[player]->Upgrades & Upgrades_SuperSonic)
		return true;

	return false;
}


float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2));
}


void MoveForward(EntityData1* entity, float speed) {
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&entity->Position);
	njRotateY(0, entity->Rotation.y);
	njRotateX(0, entity->Rotation.x);
	njTranslate(0, 0, 0, speed);
	njGetTranslation(0, &entity->Position);
	njPopMatrix(1u);
}

void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy) {
	NJS_VECTOR unit = *to;

	njSubVector(&unit, from);

	if (outy) {
		*outy = static_cast<Angle>(atan2f(unit.x, unit.z) * 65536.0f * 0.1591549762031479f);
	}

	if (outx) {
		if (from->y == to->y) {
			*outx = 0;
		}
		else {
			Float len = 1.0f / squareroot(unit.z * unit.z + unit.x * unit.x + unit.y * unit.y);

			*outx = static_cast<Angle>((acos(len * 3.3499999f) * 65536.0f * 0.1591549762031479f)
				- (acos(-(len * unit.y)) * 65536.0f * 0.1591549762031479f));
		}
	}
}

NJS_VECTOR GetPointToFollow(NJS_VECTOR* pos, NJS_VECTOR* dir, Rotation3* rot) {
	NJS_VECTOR point;
	njPushMatrix(_nj_unit_matrix_);
	njTranslateV(0, pos);
	njRotateZ(0, rot->z);
	njRotateX(0, rot->x);
	njRotateY(0, -rot->y);
	njCalcPoint(0, dir, &point);
	njPopMatrix(1u);
	return point;
}

NJS_VECTOR GetPlayerPoint(EntityData1* player) {
	NJS_VECTOR dir = { -6, 7, 5 };

	return GetPointToFollow(&player->Position, &dir, &player->Rotation);
}


HomingAttackTarget GetClosestAttack(NJS_VECTOR* pos) {
	HomingAttackTarget target = { 0, 1000000.0f };

	if (CurrentLevel < LevelIDs_Chaos0 || CurrentLevel > LevelIDs_ChaoRace) {
		for (int i = 0; i < HomingAttackTarget_Sonic_Index; ++i) {
			HomingAttackTarget* target_ = &HomingAttackTarget_Sonic[i];

			float dist = GetDistance(pos, &target_->entity->Position);

			if (dist < target.distance && target_->entity &&
				target_->entity->CollisionInfo->id == 3 &&
				target_->entity->CollisionInfo->Object->Data2 != nullptr) {
				target.distance = dist;
				target.entity = target_->entity;
			}
		}
	}

	return target;
}


void LoadAnimationFile(AnimationFile** info, const char* name) {
	PrintDebug("[Super Tails] Loading animation: %s... ", name);

	std::string fullPath = "system\\anim\\";
	fullPath = fullPath + name + ".saanim";

	AnimationFile* anm = new AnimationFile(help.GetReplaceablePath(fullPath.c_str()));

	if (anm->getmodelcount() == 0) {
		PrintDebug("Failed!\n");
		delete anm;
		*info = nullptr;
	}
	else {
		PrintDebug("Done.\n");
		*info = anm;
	}
}

bool isPerfectChasoLevel()
{
	return CurrentLevel == LevelIDs_PerfectChaos && CurrentAct == 0;
}