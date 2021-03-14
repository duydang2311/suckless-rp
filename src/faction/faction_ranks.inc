#if defined faction_divisions_included
	#endinput
#endif

#define faction_divisions_included

enum _:FactionRank {
	rankOrder,
	rankName[32]
}

static Map:m_factionRanks;

stock Faction_AddRank(id, order, const name[]) {
	if(!Faction_Valid(id)) {
		return 0;
	}

	new LinkedList:ll;
	if(!map_has_key(m_factionRanks, id)) {
		ll = linked_list_new();
		map_set(m_factionRanks, id, ll);
	} else {
		ll = LinkedList:map_get(m_factionRanks, id);
	}
	linked_list_add_str(ll, name, order);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_factionRanks = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS faction_ranks (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_datet TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED,\
	rank_order TINYINT UNSIGNED,\
	rank_name VARCHAR(32) DEFAULT '',\
	FOREIGN KEY (id) REFERENCES faction_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}