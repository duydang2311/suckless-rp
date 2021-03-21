#if defined player_hands_included
	#endinput
#endif

#define player_hands_included

static playerHands[MAX_PLAYERS][2] = {{-1, -1}, ...};

stock Player_EquipItem(playerid, hand_index, item_id) {
	if(playerHands[playerid][hand_index] != -1) {
		return 0;
	}

	new Iter:inv_it = Inventory_GetIterByItem(playerid, item_id);
	if(iter_valid(inv_it)) {
		playerHands[playerid][hand_index] = item_id;
		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_hands (player_id, item_id, hand_index) VALUES (%d, %d, %d)", Player_GetId(playerid), item_id, hand_index);
		mysql_pquery(MySQL_Connection(), query);

		Inventory_RemoveItemByIter(playerid, inv_it);
		iter_release(inv_it);
		CallLocalFunction("Item_OnPlayerEquipped", "iii", playerid, hand_index, item_id);
		CallLocalFunction("Player_OnItemEquipped", "iii", playerid, hand_index, item_id);
	}
	return 0;
}

stock Player_UnequipItem(playerid, hand_index) {
	if(playerHands[playerid][hand_index] == -1) {
		return 0;
	}

	new query[128];
	Inventory_AddItem(playerid, playerHands[playerid][hand_index]);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM player_hands WHERE player_id = %d AND hand_index = %d", Player_GetId(playerid), hand_index);
	mysql_pquery(MySQL_Connection(), query);
	CallLocalFunction("Item_OnPlayerUnequipped", "iii", playerid, hand_index, playerHands[playerid][hand_index]);
	CallLocalFunction("Player_OnItemUnequipped", "iii", playerid, hand_index, playerHands[playerid][hand_index]);
	playerHands[playerid][hand_index] = -1;
	return 1;
}

stock Player_GetHandItemId(playerid, hand_index) {
	return playerHands[playerid][hand_index];
}

stock Player_GetFreeHand(playerid) {
	return (playerHands[playerid][1] == -1 ? 1 : playerHands[playerid][0] == -1 ? 0 : -1);
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {
	inline const QueryFinished() {
		new index;
		for(new i = 0, rows = cache_num_rows(); i != rows; i++) {
			cache_get_value_name_int(i, "hand_index", index);
			cache_get_value_name_int(i, "item_id", playerHands[playerid][index]);
			CallLocalFunction("Item_OnPlayerEquipped", "iii", playerid, index, playerHands[playerid][index]);
			CallLocalFunction("Player_OnItemEquipped", "iii", playerid, index, playerHands[playerid][index]);
			printf("called Player_OnItemEquipped %d %d %d", playerid, index, playerHands[playerid][index]);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_hands WHERE player_id = %d", Player_GetId(playerid));
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	playerHands[playerid][0] = playerHands[playerid][1] = -1;
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
	FOREIGN KEY (item_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (player_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	for(new i = 0; i != MAX_PLAYERS; i++) {
		playerHands[i][0] = playerHands[i][1] = -1;
	}
	return 1;
}