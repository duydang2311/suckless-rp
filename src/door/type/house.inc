#if defined door_type_house_included
	#endinput
#endif

#define door_type_house_included

enum HouseDoorStats {
	doorId,
	doorHouseId
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS house_doors (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAM ON UPDATE CURRENT_TIMESTAMP,\
	door_id INT UNSIGNED PRIMARY KEY,\
	house_id INT UNSIGNED,\
	FOREIGN KEY (door_id) REFERENCES door_stats(id) ON UPDATE CASCADE ON DELETE CASCADE,\
	FOREIGN KEY (house_id) REFERENCES house(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}
