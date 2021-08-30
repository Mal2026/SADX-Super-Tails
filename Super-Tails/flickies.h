#pragma once

DataPointer(NJS_OBJECT, Wing_Model, 0x2E1CD6C);
DataPointer(NJS_ACTION, WingAnim, 0x2DB3344);

enum flickyState {
	flicky_turnAround,
	flicky_followPlayer,
	flicky_attack,
};

enum StatusFlicky: __int16 {
	StatusFlicky_Held = 0x1000,

	// Custom ones:
	StatusFlicky_FlyPlayer = 0x2000,
	StatusFlicky_Attacked = 0x4000
};