#if defined player_hands_included
	#endinput
#endif

#define player_hands_included

enum PlayerHandStats {
	hand_itemId,
	Float:hand_itemOffset[3],
	Float:hand_itemRot[3],
	Float:hand_itemScale[3]
}

static playerHands[MAX_PLAYERS][2][PlayerHandStats];

CMD:handsinfo(playerid) {
	new string[128];
	format(string, sizeof(string), "hands: %d, %d", playerHands[playerid][0][hand_itemId], playerHands[playerid][1][hand_itemId]);
	Player_Msg(playerid, -1, string);
	return 1;
}

stock Player_EquipItem(playerid, hand_index, item_id) {
	if(playerHands[playerid][hand_index][hand_itemId] != -1) {
		return Player_UnequipItem(playerid, hand_index);
	}

	new Iter:inv_it = Inventory_GetItemIter(playerid, item_id);
	if(iter_valid(inv_it)) {
		new
			stats[PlayerHandStats],
			inv_stats[InventoryItemStats];
		iter_get_arr(inv_it, inv_stats);
		stats[hand_itemId] = inv_stats[inv_itemId];
		stats[hand_itemOffset] = inv_stats[inv_itemOffset];
		stats[hand_itemRot] = inv_stats[inv_itemRot];
		stats[hand_itemScale] = inv_stats[inv_itemScale];

		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_hands (player_id, item_id, hand_index) VALUES (%d, %d, %d)", Player_GetId(playerid), item_id, hand_index);
		mysql_pquery(MySQL_Connection(), query);

		CallLocalFunction("Item_OnPlayerEquipped", "iii", playerid, _:hand_index, item_id);
		Inventory_RemoveItemByIter(inv_it);
		iter_release(inv_it);
	}
	return 0;
}

stock Player_UnequipItem(playerid, hand_index) {
	if(playerHands[playerid][hand_index][hand_itemId] == -1) {
		return 0;
	}

	new query[128];
	Inventory_AddItem(playerid, playerHands[playerid][hand_index][hand_itemId]);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM player_hands WHERE player_id = %d AND hand_index = %d", Player_GetId(playerid), hand_index);
	mysql_pquery(MySQL_Connection(), query);
	playerHands[playerid][hand_index][hand_itemId] = -1;
	playerHands[playerid][hand_index][hand_itemOffset][hand_index] = playerHands[playerid][hand_index][hand_itemOffset][1] = playerHands[playerid][hand_index][hand_itemOffset][2] =
	playerHands[playerid][hand_index][hand_itemRot][hand_index] = playerHands[playerid][hand_index][hand_itemRot][1] = playerHands[playerid][hand_index][hand_itemRot][2] =
	playerHands[playerid][hand_index][hand_itemScale][hand_index] = playerHands[playerid][hand_index][hand_itemScale][1] = playerHands[playerid][hand_index][hand_itemScale][2] = 0.0000;
	CallLocalFunction("Item_OnPlayerUnequipped", "iii", playerid, _:hand_index, playerHands[playerid][hand_index][hand_itemId]);
	return 1;
}

stock Player_GetHandItemId(playerid, hand_index) {
	return playerHands[playerid][hand_index][hand_itemId];
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {
	print("player_hands.inc");

	inline const QueryFinished() {
		new rows = cache_num_rows();
		if(rows) {
			new index;
			for(new i = 0; i != rows; i++) {
				cache_get_value_name_int(i, "hand_index", index);
				cache_get_value_name_int(i, "item_id", playerHands[playerid][index][hand_itemId]);
				cache_get_value_name_float(i, "offset_x", playerHands[playerid][index][hand_itemOffset][0]);
				cache_get_value_name_float(i, "offset_y", playerHands[playerid][index][hand_itemOffset][1]);
				cache_get_value_name_float(i, "offset_z", playerHands[playerid][index][hand_itemOffset][2]);
				cache_get_value_name_float(i, "rot_x", playerHands[playerid][index][hand_itemRot][0]);
				cache_get_value_name_float(i, "rot_y", playerHands[playerid][index][hand_itemRot][1]);
				cache_get_value_name_float(i, "rot_z", playerHands[playerid][index][hand_itemRot][2]);
				cache_get_value_name_float(i, "scale_x", playerHands[playerid][index][hand_itemScale][0]);
				cache_get_value_name_float(i, "scale_y", playerHands[playerid][index][hand_itemScale][1]);
				cache_get_value_name_float(i, "scale_z", playerHands[playerid][index][hand_itemScale][2]);
				CallLocalFunction("Item_OnPlayerEquipped", "iii", playerid, index, playerHands[playerid][index][hand_itemId]);
			}
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_hands WHERE player_id = %d", Player_GetId(playerid));
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	playerHands[playerid][0][hand_itemId] = -1;
	playerHands[playerid][0][hand_itemOffset][0] = playerHands[playerid][0][hand_itemOffset][1] = playerHands[playerid][0][hand_itemOffset][2] =
	playerHands[playerid][0][hand_itemRot][0] = playerHands[playerid][0][hand_itemRot][1] = playerHands[playerid][0][hand_itemRot][2] =
	playerHands[playerid][0][hand_itemScale][0] = playerHands[playerid][0][hand_itemScale][1] = playerHands[playerid][0][hand_itemScale][2] = 0.0000;

	playerHands[playerid][1][hand_itemId] = -1;
	playerHands[playerid][1][hand_itemOffset][1] = playerHands[playerid][1][hand_itemOffset][1] = playerHands[playerid][1][hand_itemOffset][2] =
	playerHands[playerid][1][hand_itemRot][1] = playerHands[playerid][1][hand_itemRot][1] = playerHands[playerid][1][hand_itemRot][2] =
	playerHands[playerid][1][hand_itemScale][1] = playerHands[playerid][1][hand_itemScale][1] = playerHands[playerid][1][hand_itemScale][2] = 0.0000;
	return 1;
}

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_hands (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	player_id INT UNSIGNED,\
	item_id INT UNSIGNED PRIMARY KEY,\
	hand_index BOOLEAN DEFAULT FALSE,\
	offset_x FLOAT(11, 4) DEFAULT 0.0000,\
	offset_y FLOAT(11, 4) DEFAULT 0.0000,\
	offset_z FLOAT(11, 4) DEFAULT 0.0000,\
	rot_x FLOAT(11, 4) DEFAULT 0.0000,\
	rot_y FLOAT(11, 4) DEFAULT 0.0000,\
	rot_z FLOAT(11, 4) DEFAULT 0.0000,\
	scale_x FLOAT(11, 4) DEFAULT 0.0000,\
	scale_y FLOAT(11, 4) DEFAULT 0.0000,\
	scale_z FLOAT(11, 4) DEFAULT 0.0000,\
	FOREIGN KEY (item_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (player_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	for(new i = 0; i != MAX_PLAYERS; i++) {
		playerHands[i][0][hand_itemId] = -1;
		playerHands[i][0][hand_itemOffset][0] = playerHands[i][0][hand_itemOffset][1] = playerHands[i][0][hand_itemOffset][2] =
		playerHands[i][0][hand_itemRot][0] = playerHands[i][0][hand_itemRot][1] = playerHands[i][0][hand_itemRot][2] =
		playerHands[i][0][hand_itemScale][0] = playerHands[i][0][hand_itemScale][1] = playerHands[i][0][hand_itemScale][2] = 0.0000;

		playerHands[i][1][hand_itemId] = -1;
		playerHands[i][1][hand_itemOffset][1] = playerHands[i][1][hand_itemOffset][1] = playerHands[i][1][hand_itemOffset][2] =
		playerHands[i][1][hand_itemRot][1] = playerHands[i][1][hand_itemRot][1] = playerHands[i][1][hand_itemRot][2] =
		playerHands[i][1][hand_itemScale][1] = playerHands[i][1][hand_itemScale][1] = playerHands[i][1][hand_itemScale][2] = 0.0000;
	}
	return 1;
}