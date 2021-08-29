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

	if (CharObj2Ptrs[player]->Upgrades & Upgrades_SuperSonic)
		return true;

	return false;
}