#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

#include <YSI_Coding\y_hooks>
hook OnGameModeInit() {
    mysql_query(MySQlConnection(), "CREATE TABLE IF NOT EXISTS frequency_stats(\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    frequency_v INT UNSIGNED DEFAULT 0", false);
    return 1;
}