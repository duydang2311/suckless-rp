#if defined player_utility_included
	#endinput
#endif

#define player_utility_included

stock Player_Msg(playerid, color, const message[]) {
	ZMsg_SendClientMessage(playerid, color, message);
	return 1;
}

stock Player_KickMsg(playerid, const message[]) {
	if(!IsPlayerConnected(playerid)) return 1;
	Player_Msg(playerid, COLOR_RED, message);
	task_unblock(1);
	await task_ms(5);
	Kick(playerid);
	return 1;
}

stock bool:Player_IsValidName(const name[]) {
	new
		first_name[MAX_PLAYER_NAME + 1],
        last_name[MAX_PLAYER_NAME + 1];
    if(sscanf(name, "p<_>s[32]s[32]", first_name, last_name)) {
        return false;
    }

    if(!strlen(first_name) || !strlen(last_name)) {
        return false;
    }

	new len = strlen(name);
    while(len--) {
		if(!((name[len] >= 'a' && name[len] <= 'z') || (name[len] >= 'A' && name[len] >= 'Z') || name[len] == '_')) {
			return false;
		}
    }
    return true;
}

stock bool:Player_IsValidPassword(const password[]) {
	return !(isnull(password) || strlen(password) < 8 || strlen(password) > 64);
}