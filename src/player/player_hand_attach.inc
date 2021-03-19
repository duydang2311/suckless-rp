#if defined player_hand_attach_included
	#endinput
#endif

#define player_hand_attach_included

static playerHandAttachSlot[MAX_PLAYERS][2] = {{-1, -1}, ...};

stock Task:Player_AttachItemToHand(playerid, hand_index, slot, item_id) {
	new Task:t = task_new();
	task_detach();

	inline const QueryFinished() {
		printf("Player_AttachItemToHand query finished, rows %d", cache_num_rows());
		playerHandAttachSlot[playerid][hand_index] = slot;
		if(cache_num_rows()) {
			new
				Float:offset[3],
				Float:rot[3],
				Float:scale[3];
			cache_get_value_name_float(0, "offset_x", offset[0]);
			cache_get_value_name_float(0, "offset_y", offset[1]);
			cache_get_value_name_float(0, "offset_z", offset[2]);
			cache_get_value_name_float(0, "rot_x", rot[0]);
			cache_get_value_name_float(0, "rot_y", rot[1]);
			cache_get_value_name_float(0, "rot_z", rot[2]);
			cache_get_value_name_float(0, "scale_x", scale[0]);
			cache_get_value_name_float(0, "scale_y", scale[1]);
			cache_get_value_name_float(0, "scale_z", scale[2]);
			SetPlayerAttachedObject(playerid, slot, Item_GetModel(item_id), 5 + hand_index, offset[0], offset[1], offset[2], rot[0], rot[1], rot[2], scale[0], scale[1], scale[2]);
		} else {
			new query[150];
			mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_hand_attachments(player_id, hand_index, item_id) VALUES (%d, %d, %d)", Player_GetId(playerid), hand_index, item_id);
			mysql_pquery(MySQL_Connection(), query);
			SetPlayerAttachedObject(playerid, slot, Item_GetModel(item_id), 5 + hand_index);
		}
		task_set_result(t, 1);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_hand_attachments WHERE player_id = %d AND hand_index = %d AND item_id = %d LIMIT 1", Player_GetId(playerid), hand_index, item_id);
	print("Player_AttachItemToHand send query");
	return t;
}

stock Player_GetHandAttachSlot(playerid, hand_index) {
	return playerHandAttachSlot[playerid][hand_index];
}

stock Player_UpdateHandAttach(playerid, hand_index, item_id, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ) {
	new query[384];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE player_hand_attachments SET offset_x = %.4f, offset_y = %.4f, offset_z = %.4f, rot_x = %.4f, rot_y = %.4f, rot_z = %.4f, scale_x = %.4f, scale_y = %.4f, scale_z = %.4f WHERE player_id = %d AND hand_index = %d AND item_id = %d LIMIT 1", fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ, Player_GetId(playerid), hand_index, item_id);
	mysql_pquery(MySQL_Connection(), query);
	print(query);
	return 1;
}

forward Player_OnItemEquipped(playerid, hand_index, item_id);
public Player_OnItemEquipped(playerid, hand_index, item_id) {
	new slot = Player_GetFreeAttachmentSlot(playerid, .use = true);
	printf("Player_OnItemEquipped(%d, %d, %d), slot %d", playerid, hand_index, item_id, slot);
	if(slot != -1) {
		Player_AttachItemToHand(playerid, hand_index, slot, item_id);
	}
	return 1;
}

forward Player_OnItemUnequipped(playerid, hand_index, item_id);
public Player_OnItemUnequipped(playerid, hand_index, item_id) {
	if(playerHandAttachSlot[playerid][hand_index] != -1) {
		RemovePlayerAttachedObject(playerid, playerHandAttachSlot[playerid][hand_index]);
		playerHandAttachSlot[playerid][hand_index] = -1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerConnect(playerid) {
	playerHandAttachSlot[playerid][0] = playerHandAttachSlot[playerid][1] = -1;
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_hand_attachments (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	player_id INT UNSIGNED,\
	hand_index BOOLEAN,\
	item_id INT UNSIGNED,\
	offset_x FLOAT(11, 4) DEFAULT 0.0000,\
	offset_y FLOAT(11, 4) DEFAULT 0.0000,\
	offset_z FLOAT(11, 4) DEFAULT 0.0000,\
	rot_x FLOAT(11, 4) DEFAULT 0.0000,\
	rot_y FLOAT(11, 4) DEFAULT 0.0000,\
	rot_z FLOAT(11, 4) DEFAULT 0.0000,\
	scale_x FLOAT(11, 4) DEFAULT 1.0000,\
	scale_y FLOAT(11, 4) DEFAULT 1.0000,\
	scale_z FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (player_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (item_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}