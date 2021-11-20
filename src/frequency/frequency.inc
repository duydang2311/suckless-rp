#if defined frequency_included
	#endinput
#endif

#define frequency_included

#include <a_samp>
#include <PawnPlus>

#include "./frequency_stats.inc"

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@81() {
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS frequency_stats(\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    frequency_v INT UNSIGNED DEFAULT 0)", false);
    return 1;
}
