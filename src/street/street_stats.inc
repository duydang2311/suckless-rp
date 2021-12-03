#if defined street_stats_included
	#endinput
#endif

#define street_stats_included

#define                 MAX_STREET_NAME_LENGTH                      22

enum StreetStatsType {
    strTypeAve,
    strTypeST
}

enum _:StreetStats {
    streetID,
    streetNumber,
    StreetStatsType:streetType,
    streetName[MAX_STREET_NAME_LENGTH]
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS street_stats(\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    id INT UNSIGNED PRIMARY KEY,\
    number SMALLINT,\
    type TINYINT UNSIGNED,\
    name VARCHAR(22) DEFAULT '')", false);
    return 1;
}