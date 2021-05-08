#if defined phone_model_included
	#endinput
#endif

#define phone_model_included

enum _:IPhoneModel {
	Nokia
}

enum _:PhoneButtonSignal {
	PhoneButtonUp,
	PhoneButtonDown,
	PhoneButtonLeft,
	PhoneButtonRight,
	PhoneButtonCall,
	PhoneButtonHangup
}

stock IPModel_GetSafeScreenOffset(id, &Float:min_x, &Float:min_y, &Float:max_x, &Float:max_y) {
	new result = 0;
	CallLocalFunction("IPM_OnSafeScreenGet", "iiiiii", id, ref(_:min_x), ref(_:min_y), ref(_:max_x), ref(_:max_y), ref(result));
	return result;
}

#include ".\ability\phone\model\nokia\nokia_body.inc"
#include ".\ability\phone\model\nokia\nokia_ui.inc"