#if defined player_utility_included
	#endinput
#endif

#define player_utility_included

stock Player_Msg(playerid, color, const message[]) {
	ZMsg_SendClientMessage(playerid, color, message);
	return 1;
}

stock Player_BroadcastMsg(playerid, Float:range, from_color, to_color, const string[], source_color = -1) {
    new
		Float:player_pos[3],
        Float:dist,
        new_color,
        Float:ratio,
        from_r,
        from_g,
        from_b,
        to_r,
        to_g,
        to_b,
        unused;

    Util_RGBA(from_color, from_r, from_g, from_b, unused);
    Util_RGBA(to_color, to_r, to_g, to_b, unused);

    GetPlayerPos(playerid, player_pos[0], player_pos[1], player_pos[2]);
    foreach(new i : Player) {
        if(Player_IsStatsLoaded(i)) {
			if(i == playerid) {
				if(source_color != -1) {
					Player_Msg(i, source_color, string);
				} else {
					Player_Msg(i, from_color, string);
				}
				continue;
			}
            dist = GetPlayerDistanceFromPoint(i, player_pos[0], player_pos[1], player_pos[2]);
            if(dist <= range) {
                ratio = dist / range;
                Util_Hex(new_color, from_r + floatround((to_r - from_r) * ratio), from_g + floatround((to_g - from_g) * ratio), from_b + floatround((to_b - from_b) * ratio), 255);
				Player_Msg(i, new_color, string);
            }
        }
    }
    return 1;
}

stock Player_BroadcastDamagedMsg(playerid, Float:range, from_color, to_color, const string[], const damaged[], source_color = -1) {
    new
		Float:player_pos[3],
        Float:dist,
        new_color,
        Float:ratio,
        from_r,
        from_g,
        from_b,
        to_r,
        to_g,
        to_b,
        unused;

    Util_RGBA(from_color, from_r, from_g, from_b, unused);
    Util_RGBA(to_color, to_r, to_g, to_b, unused);

    GetPlayerPos(playerid, player_pos[0], player_pos[1], player_pos[2]);
    foreach(new i : Player) {
        if(Player_IsStatsLoaded(i)) {
			if(i == playerid) {
				if(source_color != -1) {
					Player_Msg(i, source_color, string);
				} else {
					Player_Msg(i, from_color, string);
				}
				continue;
			}
            dist = GetPlayerDistanceFromPoint(i, player_pos[0], player_pos[1], player_pos[2]);
            if(dist <= range) {
                ratio = dist / range;
                Util_Hex(new_color, from_r + floatround((to_r - from_r) * ratio), from_g + floatround((to_g - from_g) * ratio), from_b + floatround((to_b - from_b) * ratio), 255);
				Player_Msg(i, new_color, string);
            } else if(dist <= (range * 1.2)) {
				Player_Msg(i, to_color, damaged);
			}
        }
    }
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

stock Player_StreamToPosition(playerid, Float:x, Float:y, Float:z, Float:a, int, vw) {
	SetPlayerPos(playerid, x, y, z);
	SetPlayerFacingAngle(playerid, a);
	SetPlayerInterior(playerid, int);
	SetPlayerVirtualWorld(playerid, vw);
	SetCameraBehindPlayer(playerid);
	return 1;
}

stock Player_SyncedStreamToPosition(playerid, Float:x, Float:y, Float:z, Float:a, int, vw) {
	Player_StreamToPosition(playerid, x, y, z, a, int, vw);
	Streamer_UpdateEx(playerid, x, y, z, vw, int, STREAMER_TYPE_OBJECT);
	return 1;
}

stock Player_GetFreeAttachmentSlot(playerid, bool:use = false) {
	for(new i = 0; i != 10; i++) {
		if(!IsPlayerAttachedObjectSlotUsed(playerid, i)) {
			if(use) {
				SetPlayerAttachedObject(playerid, i, 0, 0);
			}
			return i;
		}
	}
	return -1;
}

stock Player_PutInVehicle(playerid, vehicleid, seatid) {
	foreach(new i : Player) {
		if(i != playerid) {
			if(GetPlayerVehicleID(i) == vehicleid) {
				if(GetPlayerVehicleSeat(i) == seatid) {
					return 0;
				}
			}
		} 
	}

	PutPlayerInVehicle(playerid, vehicleid, seatid);
	return 1;
}

forward Player_ToggleControllable(playerid, bool:toggle);
public Player_ToggleControllable(playerid, bool:toggle) {
	TogglePlayerControllable(playerid, toggle);
	return 1;
}
