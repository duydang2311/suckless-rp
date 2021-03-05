#if defined player_hands_included
	#endinput
#endif

#define player_hands_included

enum _:PlayerHandItemStats {
	hand_itemId,
	bool:hand_itemHand,
	Float:hand_itemOffset[3],
	Float:hand_itemRot[3],
	Float:hand_itemScale[3]
}

static Map:m_hands;

CMD:handsinfo(playerid) {
	new string[128],
		hands = 0;
	if(map_has_key(m_hands, playerid)) {
		hands = linked_list_size(LinkedList:map_get(m_hands, playerid));
	}
	format(string, sizeof(string), "m_hands ll size: %d", hands);
	Player_Msg(playerid, -1, string);
	return 1;
}

stock Player_EquipItem(playerid, bool:hand_leftright, item_id) {
	if(Player_GetHandItemId(playerid, hand_leftright) != -1) {
		return Player_UnequipItem(playerid, hand_leftright);
	}

	new Iter:inv_it = Inventory_GetItemIter(playerid, item_id);
	if(iter_valid(inv_it)) {
		new
			stats[PlayerHandItemStats],
			inv_stats[InventoryItemStats];
		iter_get_arr(inv_it, inv_stats);
		stats[hand_itemId] = inv_stats[inv_itemId];
		stats[hand_itemOffset] = inv_stats[inv_itemOffset];
		stats[hand_itemRot] = inv_stats[inv_itemRot];
		stats[hand_itemScale] = inv_stats[inv_itemScale];
		stats[hand_itemHand] = hand_leftright;

		new LinkedList:ll;
		if(!map_has_key(m_hands, playerid)) {
			ll = linked_list_new();
			map_set(m_hands, playerid, ll);
		} else {
			ll = LinkedList:map_get(m_hands, playerid);
		}
		linked_list_add_arr(ll, stats);

		new query[128];
		mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_hands (player_id, item_id, leftright) VALUES (%d, %d, %d)", Player_GetId(playerid), item_id, hand_leftright);
		mysql_pquery(MySQL_Connection(), query);

		CallLocalFunction("Item_OnPlayerEquipped", "iii", playerid, _:hand_leftright, item_id);
		Inventory_RemoveItemByIter(inv_it);
		iter_release(inv_it);
	}
	return 0;
}

stock Player_UnequipItem(playerid, bool:hand_leftright) {
	if(map_has_key(m_hands, playerid)) {
		new stats[PlayerHandItemStats];
		for(new Iter:it = linked_list_iter(LinkedList:map_get(m_hands, playerid)); iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, stats);
			if(stats[hand_itemHand] == hand_leftright) {
				Inventory_AddItem(playerid, stats[hand_itemId]);

				new query[128];
				mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM player_hands WHERE player_id = %d AND leftright = %d", Player_GetId(playerid), hand_leftright);
				mysql_pquery(MySQL_Connection(), query);

				iter_erase(it);
				CallLocalFunction("Item_OnPlayerUnequipped", "iii", playerid, _:hand_leftright, stats[hand_itemId]);
				return 1;
			}
		}
	}
	return 0;
}

stock Player_GetHandItemId(playerid, bool:hand_leftright) {
	if(map_has_key(m_hands, playerid)) {
		new stats[PlayerHandItemStats];
		for(new Iter:it = linked_list_iter(LinkedList:map_get(m_hands, playerid)); iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, stats);
			if(stats[hand_itemHand] == hand_leftright) {
				return stats[hand_itemId];
			}
		}
	}
	return -1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_hands = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_hands (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	player_id INT UNSIGNED,\
	item_id INT UNSIGNED PRIMARY KEY,\
	leftright BOOLEAN DEFAULT FALSE,\
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
	return 1;
}