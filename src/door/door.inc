#if defined door_included
	#endinput
#endif

#define door_included

#include "./door_stats.inc"
#include "./type/house.inc"

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS house (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
	type TINYINT UNSIGNED DEFAULT 0\
	)", false);
	return 1;
}