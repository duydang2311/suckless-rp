#if defined app_call_included
	#endinput
#endif

#define app_call_included

DEFINE_HOOK_REPLACEMENT(Contact, Cont);

#include ".\item\ability\phone\app\call\call_contact.inc"
#include ".\item\ability\phone\app\call\call_dial.inc"
#include ".\item\ability\phone\app\call\call_button.inc"
#include ".\item\ability\phone\app\call\call_dial_ui.inc"
#include ".\item\ability\phone\app\call\call_contact_ui.inc"