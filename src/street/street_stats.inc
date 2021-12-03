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

static Map:m_streetStats;

stock Street_Create(id, number, StreetStatsType:type, const name[], size = sizeof(name)) {
    if(!map_valid(m_streetStats)) {
        m_streetStats = map_new();
    }

    if(map_has_key(m_streetStats, id)) {
        return 0;
    }

    new arr_street_stats[StreetStats];
    arr_street_stats[streetID] = id;
    arr_street_stats[streetNumber] = number;
    arr_street_stats[streetType] = type;
    strcat(arr_street_stats[streetName], name, size);

    new query[75 + 11 + 11 + 11 + 22 - 14 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO street_stats (id, number, type, name) VALUES (%d, %d, %d, '%e')", id, number, _:type, name);
    mysql_pquery(MySQL_Connection(), query);
    return map_set_arr(m_streetStats, id, arr_street_stats);
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
    m_streetStats = map_new();
    return 1;
}