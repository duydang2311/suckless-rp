#if defined item_stats_included
	#endinput
#endif

#define item_stats_included

const Float:MAX_ITEM_RELIABILITY = 1.0;

enum ItemAbility {
	ITEM_ABILITY_GUN,
	ITEM_ABILITY_MELEE,
	ITEM_ABILITY_PHONE,
	ITEM_ABILITY_FOOD
}

enum ItemStats {
	itemId,
	itemAbility
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS item_stats (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	ability TINYINT UNSIGNED DEFAULT 0\
	)", false);
	return 1;
}