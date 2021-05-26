#if defined faction_members_included
	#endinput
#endif

#define faction_members_included

static Map:m_factionMembers;

stock Faction_AddMember(id, member_id) {
	if(!Faction_Valid(id)) {
		return 0;
	}

	if(Faction_HasMember(id, member_id)) {
		return 0;
	}

	new LinkedList:ll;
	if(!map_has_key(m_factionMembers, id)) {
		ll = linked_list_new();
		map_set(m_factionMembers, id, ll);
	} else {
		ll = LinkedList:map_get(m_factionMembers, id);
	}
	linked_list_add(ll, member_id);
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO faction_members(id, member_id) VALUES (%d, %d)", id, member_id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

stock Faction_RemoveMember(id, member_id) {
	if(!map_has_key(m_factionMembers, id)) {
		return 0;
	}
	new query[128],
		LinkedList:ll = LinkedList:map_get(m_factionMembers, id);
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM faction_members WHERE id = %d AND member_id = %d", id, member_id);
	mysql_pquery(MySQL_Connection(), query);
	for(new Iter:it = linked_list_iter(ll); iter_inside(ll); iter_move_next(ll)) {
		if(iter_get(it) == member_id) {
			iter_erase(it);
			break;
		}
	}
	return 1;
}

stock Faction_HasMember(id, member_id) {
	if(!map_has_key(m_factionMembers, id)) {
		return 0;
	}

	new LinkedList:ll = LinkedList:map_get(m_factionMembers, id);
	for(new Iter:it = linked_list_iter(ll); iter_inside(ll); iter_move_next(ll)) {
		if(iter_get(it) == member_id) {
			return 1;
		}
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Faction_OnDeleted(id) {
	if(map_has_key(m_factionMembers, id)) {
		new LinkedList:ll = LinkedList:map_get(m_factionMembers, id);
		map_remove_deep(m_factionMembers, id);
	}
	return 1;
}

hook OnGameModeInit() {
	m_factionMembers = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS faction_members (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED,\
	member_id INT UNSIGNED,\
	rank TINYINT UNSIGNED NULL,\
	division TINYINT UNSIGNED NULL,\
	FOREIGN KEY (id) REFERENCES faction_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (member_id) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}