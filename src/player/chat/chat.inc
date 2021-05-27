#if defined player_chat_included
	#endinput
#endif

#define player_chat_included

const
	PLAYER_CHAT_BADGE_SIZE = 32,
	PLAYER_CHAT_TEXT_SIZE = 144;

forward Player_OnChat(playerid, badge[], text[]);
public Player_OnChat(playerid, badge[], text[]) {
	if(!isnull(badge)) {
		new
			format_text[PLAYER_CHAT_BADGE_SIZE + PLAYER_CHAT_BADGE_SIZE + MAX_PLAYER_NAME + 10],
			name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name);
		format(format_text, sizeof(format_text), "%s (%s): %s", name, badge, text);
		Player_BroadcastMsg(playerid, 20.0, COLOR_WHITE, COLOR_FADE7, format_text);
	} else {
		new
			format_text[PLAYER_CHAT_BADGE_SIZE + MAX_PLAYER_NAME + 10],
			name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name);
		format(format_text, sizeof(format_text), "%s: %s", name, text);
		Player_BroadcastMsg(playerid, 20.0, COLOR_WHITE, COLOR_FADE7, format_text);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerText(playerid, text[]) {
	new
		safe_text[PLAYER_CHAT_TEXT_SIZE] = "",
		badge[PLAYER_CHAT_BADGE_SIZE] = "";
	strcat(safe_text, text);
	CallLocalFunction("Player_OnChat", "iii", playerid, ref(badge), ref(safe_text));
	return 0;
}