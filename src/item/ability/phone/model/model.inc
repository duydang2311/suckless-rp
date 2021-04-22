#if defined phone_model_included
	#endinput
#endif

#define phone_model_included

enum _:IPhoneModel {
	Nokia
}

stock IPModel_GetBodyOffset(id, index, &Float:x, &Float:y, &Float:size) {
	new result = 0;
	CallLocalFunction("IPM_OnBodyOffsetGet", "iifffi", id, index, ref(_:x), ref(_:y), ref(_:size), ref(result));
	return result;
}

stock IPModel_GetScreenOffset(id, &Float:min_x, &Float:min_y, &Float:max_x, &Float:max_y) {
	new result = 0;
	CallLocalFunction("IPM_OnScreenOffsetGet", "iffffi", id, ref(_:min_x), ref(_:min_y), ref(_:max_x), ref(_:max_y), ref(result));
	return result;
}

#include "./ability/phone/model/nokia/nokia_body.inc"
#include ".\ability\phone\model\nokia\nokia_ui.inc"