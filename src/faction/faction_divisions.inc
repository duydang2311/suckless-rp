#if defined faction_divisions_included
	#endinput
#endif

#define faction_divisions_included

enum _:FactionDivision {
	divOrder,
	divName[32]
}

static Map:m_factionDivisions;

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_factionDivisions = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS faction_divisions (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED,\
	div_order TINYINT UNSIGNED,\
	div_name VARCHAR(32) DEFAULT '',\
	FOREIGN KEY (id) REFERENCES faction_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}