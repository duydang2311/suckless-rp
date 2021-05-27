#if defined app_call_forwards_included
	#endinput
#endif

#define app_call_forwards_included

forward Task:AppCall_UpdateSessionUI(id, playerid);
forward Ref<LinkedList>:AppCall_GetPhoneSessionRef(id);