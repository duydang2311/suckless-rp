#if defined call_session_listen_included
	#endinput
#endif

#define call_session_listen_included

#include <YSI_Coding\y_hooks>

hook Player_OnChat(playerid, badge[], text[]) {
	for(new i = 1, phone; i != -1; i--) {
		phone = Player_GetHandItemId(playerid, i);
		if(phone != -1 && IPhone_Has(phone) && AppCall_GetPhoneSessionRef(phone) != Ref<LinkedList>:-1) {
			if(!isnull(badge)) {
				strcat(badge, ", phone", 32);
			} else {
				strcat(badge, "phone", 32);
			}
			break;
		}
	}
	return 1;
}