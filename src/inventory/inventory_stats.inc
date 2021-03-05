#if defined inventory_stats_included
	#endinput
#endif

#define inventory_stats_included

enum _:InventoryItemStats {
	inv_itemId,
	bool:inv_itemVisible,
	Float:inv_itemOffset[3],
	Float:inv_itemRot[3],
	Float:inv_itemScale[3]
}

static Map:m_inventory;

stock Inventory_AddItem(playerid, item_id) {
	new LinkedList:ll;
	if(map_has_key(m_inventory, playerid)) {
		ll = LinkedList:map_get(m_inventory, playerid);
	} else {
		ll = linked_list_new();
		map_set(m_inventory, playerid, ll);
	}

	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO inventory (player_id, item_id) VALUES (%d, %d)", Player_GetId(playerid), item_id);
	mysql_pquery(MySQL_Connection(), query);
	linked_list_add(ll, item_id);
	return 1;
}

stock Inventory_RemoveItem(playerid, item_id) {
	if(map_has_key(m_inventory, playerid)) {
		new stats[InventoryItemStats];
		for(new Iter:it = linked_list_iter(LinkedList:map_get(m_inventory, playerid)); iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, stats);
			if(stats[inv_itemId] == item_id) {
				new query[128];
				mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM inventory WHERE item_id = %d LIMIT 1", item_id);
				mysql_pquery(MySQL_Connection(), query);
				iter_erase(it);
				return 1;
			}
		}
	}
	return 0;
}

stock Iter:Inventory_GetItemIter(playerid, item_id) {
	if(map_has_key(m_inventory, playerid)) {
		new
			Iter:it = linked_list_iter(LinkedList:map_get(m_inventory, playerid)),
			stats[InventoryItemStats];
		iter_acquire(it);
		for(; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, stats);
			if(stats[inv_itemId] == item_id) {
				return it;
			}
		}
		iter_release(it);
	}
	return Iter:0;
}

CMD:invinfo(playerid) {
	new string[128],
		inventory = 0;
	if(map_has_key(m_inventory, playerid)) {
		inventory = linked_list_size(LinkedList:map_get(m_inventory, playerid));
	}
	format(string, sizeof(string), "m_inventory ll size: %d", inventory);
	Player_Msg(playerid, -1, string);
	return 1;
}

stock Iter:Inventory_GetFirstItemIter(playerid) {
	if(map_has_key(m_inventory, playerid)) {
		new
			Iter:it = linked_list_iter(LinkedList:map_get(m_inventory, playerid));
		iter_acquire(it);
		return it;
	}
	return Iter:0;
}

stock Inventory_RemoveItemByIter(Iter:it) {
	if(iter_valid(it)) {
		new
			stats[InventoryItemStats],
			query[128];
		iter_get_arr(it, stats);
		mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM inventory WHERE item_id = %d LIMIT 1", stats[inv_itemId]);
		mysql_pquery(MySQL_Connection(), query);
		iter_erase(it);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {
	print("inventory_stats.inc");
	inline const QueryFinished() {
		new rows = cache_num_rows();
		if(rows) {
			new
				stats[InventoryItemStats],
				LinkedList:ll = linked_list_new();
			for(new i = 0; i != rows; i++) {
				cache_get_value_name_int(i, "item_id", stats[inv_itemId]);
				cache_get_value_name_int(i, "visible", stats[inv_itemVisible]);

				cache_get_value_name_float(i, "offset_x", stats[inv_itemOffset][0]);
				cache_get_value_name_float(i, "offset_y", stats[inv_itemOffset][1]);
				cache_get_value_name_float(i, "offset_z", stats[inv_itemOffset][2]);
				cache_get_value_name_float(i, "rot_x", stats[inv_itemRot][0]);
				cache_get_value_name_float(i, "rot_y", stats[inv_itemRot][1]);
				cache_get_value_name_float(i, "rot_z", stats[inv_itemRot][2]);
				cache_get_value_name_float(i, "scale_x", stats[inv_itemScale][0]);
				cache_get_value_name_float(i, "scale_y", stats[inv_itemScale][1]);
				cache_get_value_name_float(i, "scale_z", stats[inv_itemScale][2]);
				linked_list_add_arr(ll, stats[inv_itemId]);
			}
			map_set(m_inventory, playerid, ll);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM inventory WHERE player_id = %d", Player_GetId(playerid));
	return 1;
}

hook OnGameModeInit() {
	m_inventory = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS inventory (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	player_id INT UNSIGNED,\
	item_id INT UNSIGNED PRIMARY KEY,\
	visible BOOLEAN DEFAULT TRUE,\
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