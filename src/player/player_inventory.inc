#if defined player_inventory_included
	#endinput
#endif

#define player_inventory_included

static Map:m_inventory;

CMD:inv(playerid) {
	if(map_has_key(m_inventory, playerid)) {
		for(new Iter:it = linked_list_iter(LinkedList:map_get(m_inventory, playerid)); iter_inside(it); iter_move_next(it)) {
			printf("player %d inv: %d", playerid, iter_get(it));
		}
	}
	return 1;
}

stock Inventory_GiveItem(playerid, item_id) {
	new LinkedList:ll;
	if(map_has_key(m_inventory, playerid)) {
		ll = LinkedList:map_get(m_inventory, playerid);
	} else {
		ll = linked_list_new();
	}

	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO player_inventory (id, item_id) VALUES (%d, %d)", Player_GetId(playerid), item_id);
	mysql_pquery(MySQL_Connection(), query);
	linked_list_add(ll, item_id);
	// map_set(m_inventory, playerid, ll);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Player_OnSpawn(playerid) {
	inline const QueryFinished() {
		new rows = cache_num_rows();
		if(rows) {
			new
				item_id,
				LinkedList:ll = linked_list_new();
			for(new i = 0; i != rows; i++) {
				cache_get_value_name_int(i, "item_id", item_id);
				linked_list_add(ll, item_id);
			}
			map_set(m_inventory, playerid, ll);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM player_inventory WHERE id = %d", Player_GetId(playerid));
	return 1;
}

hook OnGameModeInit() {
	m_inventory = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_inventory (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED,\
	item_id INT UNSIGNED PRIMARY KEY,\
	FOREIGN KEY (item_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}