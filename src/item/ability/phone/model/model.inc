#if defined phone_model_included
	#endinput
#endif

#define phone_model_included

enum _:IPhoneModel {
	Nokia
}

#include "./ability/phone/model/nokia/nokia_body.inc"
#include ".\ability\phone\model\nokia\nokia_ui.inc"