#if defined call_session_chat_included
	#endinput
#endif

#define call_session_chat_included

#include <YSI_Coding\y_hooks>

hook Player_OnChat(playerid, badge[], text[]) {
	for(new i = 1, phone, Ref<LinkedList>:ssref; i != -1; i--) {
		phone = Player_GetHandItemId(playerid, i);
		if(phone != -1 && IPhone_Has(phone) && AppCall_GetPhoneSessionRef(phone) != Ref<LinkedList>:-1) {
			ssref = AppCall_GetPhoneSessionRef(phone);
			if(!isnull(badge)) {
				strcat(badge, ", phone", PLAYER_CHAT_BADGE_SIZE);
			} else {
				strcat(badge, "phone", PLAYER_CHAT_BADGE_SIZE);
			}

			if(AppCall_HasSessionRef(ssref)) {
				new LinkedList:ll = pawn_unref<LinkedList>(ssref);
				if(linked_list_valid(ll)) {
					new
						msg[PLAYER_CHAT_BADGE_SIZE + PLAYER_CHAT_BADGE_SIZE + MAX_PLAYER_NAME + 10],
						name[MAX_PLAYER_NAME + 1];
					for(new Iter:it = linked_list_iter(ll), it_phone; iter_inside(it); iter_move_next(it)) {
						it_phone = iter_get(it);
						if(it_phone == phone) {
							continue;
						}

						foreach(new p : Player) {
							if(GetPlayerSpecialAction(p) == SPECIAL_ACTION_USECELLPHONE) {
								if(Player_GetHandItemId(p, 1) == it_phone || Player_GetHandItemId(p, 0) == it_phone) {
									GetPlayerName(p, name);
									format(msg, sizeof(msg), "(%s)'s phone: %s", name, text);
									Player_Msg(p, COLOR_LIGHTYELLOW, msg);
								}
							}
						}
					}
				}
			}
			break;
		}
	}
	return 1;
}