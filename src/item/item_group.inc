#if defined item_group_included
	#endinput
#endif

#define item_group_included

static Map:m_groupRoot;

stock Item_AddToGroupItem(id, group_id) {
	new
		query[128],
		LinkedList:ll;
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_group(group_id, member_id) VALUES (%d, %d) ON DUPLICATE KEY UPDATE group_id = VALUES(group_id)", group_id, id);
	mysql_pquery(MySQL_Connection(), query);
	if(!map_valid(m_groupRoot)) {
		m_groupRoot = map_new();
	}
	if(!map_has_key(m_groupRoot, group_id)) {
		ll = linked_list_new();
		map_set(m_groupRoot, group_id, ll);
	} else {
		ll = LinkedList:map_get(m_groupRoot, group_id);
	}
	linked_list_add(ll, id);
	CallLocalFunction("Item_OnAddedToGroup", "ii", id, group_id);
	return 1;
}

stock Item_RemoveFromGroupItem(member_id, group_id) {
	if(!map_valid(m_groupRoot) || !map_has_key(m_groupRoot, group_id)) {
		return 0;
	}

	new
		query[128],
		LinkedList:ll = LinkedList:map_get(m_groupRoot, group_id);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM item_group WHERE member_id = %d LIMIT 1", member_id);
	mysql_pquery(MySQL_Connection(), query);
	for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
		if(iter_get(it) == member_id) {
			iter_erase(it);
			break;
		}
	}
	if(!linked_list_size(ll)) {
		map_remove_deep(m_groupRoot, group_id);
	}
	return 1;
}

stock Iter:Item_GetGroupIter(group_id, index = 0) {
	if(!map_valid(m_groupRoot) || !map_has_key(m_groupRoot, group_id)) {
		return Iter:-1;
	}
	new Iter:it = linked_list_iter(LinkedList:map_get(m_groupRoot, group_id), index);
	iter_acquire(it);
	return it;
}

stock Item_HasGroup(group_id) {
	return (map_valid(m_groupRoot) && map_has_key(m_groupRoot, group_id));
}

hook OnGameModeInit@1() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_group (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	group_id INT UNSIGNED,\
	member_id INT UNSIGNED PRIMARY KEY,\
	FOREIGN KEY (group_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (member_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);

	new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM item_group", true);
	for(new i = 0, rows = cache_num_rows(), group_id, member_id, LinkedList:ll; i != rows; i++) {
		cache_get_value_name_int(i, "group_id", group_id);
		cache_get_value_name_int(i, "member_id", member_id);
		if(!map_valid(m_groupRoot)) {
			m_groupRoot = map_new();
		}
		if(!map_has_key(m_groupRoot, group_id)) {
			ll = linked_list_new();
			map_set(m_groupRoot, group_id, ll);
		} else {
			ll = LinkedList:map_get(m_groupRoot, group_id);
		}
		linked_list_add(ll, member_id);
	}
	cache_delete(result);
	return 1;
}

hook OnGameModeExit@1() {
	if(map_valid(m_groupRoot)) {
		map_delete_deep(m_groupRoot);
		m_groupRoot = Map:-1;
	}
	return 1;
}