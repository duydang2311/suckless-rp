#if defined inventory_stats_included
	#endinput
#endif

#define inventory_stats_included

static Map:m_inventory;

forward Inventory_OnItemAdded(playerid, item_id);
public Inventory_OnItemAdded(playerid, item_id) {
	return 1;
}

forward Inventory_OnItemRemoved(playerid, item_id);
public Inventory_OnItemRemoved(playerid, item_id) {
	return 1;
}

forward Inventory_OnStatsRetrieved(playerid);
public Inventory_OnStatsRetrieved(playerid) {
	new rows = cache_num_rows();
	if(rows) {
		new
			id,
			LinkedList:ll;
		if(!map_has_key(m_inventory, playerid)) {
			ll = linked_list_new();
			map_set(m_inventory, playerid, ll);
		} else {
			ll = LinkedList:map_get(m_inventory, playerid);
		}
		for(new i = 0; i != rows; i++) {
			cache_get_value_name_int(i, "item_id", id);
			linked_list_add(ll, id);
			CallLocalFunction("Inventory_OnItemAdded", "ii", playerid, id);
		}
	}
	return 1;
}

stock Inventory_AddItem(playerid, item_id) {
	new LinkedList:ll;
	if(map_valid(m_inventory) && map_has_key(m_inventory, playerid)) {
		ll = LinkedList:map_get(m_inventory, playerid);
	} else {
		ll = linked_list_new();
		map_set(m_inventory, playerid, ll);
	}

	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO inventory (player_id, item_id) VALUES (%d, %d)", Player_GetId(playerid), item_id);
	mysql_pquery(MySQL_Connection(), query);
	linked_list_add(ll, item_id);
	CallLocalFunction("Inventory_OnItemAdded", "ii", playerid, item_id);
	return 1;
}

stock Inventory_RemoveItem(playerid, item_id) {
	if(map_valid(m_inventory) && map_has_key(m_inventory, playerid)) {
		for(new Iter:it = linked_list_iter(LinkedList:map_get(m_inventory, playerid)); iter_inside(it); iter_move_next(it)) {
			if(iter_get(it) == item_id) {
				new query[128];
				mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM inventory WHERE item_id = %d LIMIT 1", item_id);
				mysql_pquery(MySQL_Connection(), query);
				iter_erase(it);
				printf("Inventory_RemoveItem: player %d, item_id %d, size %d", playerid, item_id, linked_list_size(LinkedList:map_get(m_inventory, playerid)));
				CallLocalFunction("Inventory_OnItemRemoved", "ii", playerid, item_id);
				return 1;
			}
		}
	}
	return 0;
}

stock Iter:Inventory_GetIterByItem(playerid, item_id) {
	if(map_valid(m_inventory) && map_has_key(m_inventory, playerid)) {
		for(new Iter:it = linked_list_iter(LinkedList:map_get(m_inventory, playerid)); iter_inside(it); iter_move_next(it)) {
			if(iter_get(it) == item_id) {
				iter_acquire(it);
				return it;
			}
		}
	}
	return Iter:-1;
}

stock Iter:Inventory_GetIter(playerid, index = 0) {
	if(map_valid(m_inventory) && map_has_key(m_inventory, playerid)) {
		new LinkedList:ll = LinkedList:map_get(m_inventory, playerid);
		if(index < linked_list_size(ll)) {
			new
				Iter:it = linked_list_iter(ll, index);
			iter_acquire(it);
			return it;
		}
	}
	return Iter:-1;
}

stock Inventory_RemoveItemByIter(playerid, Iter:it) {
	if(iter_valid(it)) {
		new
			query[128],
			id = iter_get(it);
		mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM inventory WHERE item_id = %d LIMIT 1", id);
		mysql_pquery(MySQL_Connection(), query);
		iter_erase(it);
		printf("Inventory_RemoveItem: player %d, item_id %d, size %d", playerid, id, linked_list_size(LinkedList:map_get(m_inventory, playerid)));
		CallLocalFunction("Inventory_OnItemRemoved", "ii", playerid, id);
	}
	return 1;
}

stock Inventory_GetSize(playerid) {
	if(map_valid(m_inventory) && map_has_key(m_inventory, playerid)) {
		return linked_list_size(LinkedList:map_get(m_inventory, playerid));
	}
	return 0;
}

#include <YSI_Coding\y_hooks>

hook Player_OnStatsLoaded(playerid) {
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT * FROM inventory WHERE player_id = %d", Player_GetId(playerid));
	mysql_pquery(MySQL_Connection(), query, "Inventory_OnStatsRetrieved", "i", playerid);
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	if(map_valid(m_inventory) && map_has_key(m_inventory, playerid)) {
		map_remove_deep(m_inventory, playerid);
	}
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
	FOREIGN KEY (item_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (player_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}