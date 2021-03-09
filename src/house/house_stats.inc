#if defined house_stats_included
	#endinput
#endif

#define house_stats_included

static Map:m_house;

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS house_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY,\
	FOREIGN KEY (id) REFERENCES house(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}