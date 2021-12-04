#if defined street_stats_included
	#endinput
#endif

#define street_stats_included

const STREET_NAME_LENGTH = 22;

enum StreetType {  
    Avenue,
    Street
};

enum _:StreetStats {
    streetID,
    streetNumber,
    StreetStatsType:streetType,
    streetName[STREET_NAME_LENGTH]
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

stock Street_SetName(id, const name[], size = sizeof(name)) {
    if(!map_has_key(m_streetStats, id)) {
        return 0;
    }

    new arr_street_stats[StreetStats];
    if(map_get_arr_safe(m_streetStats, id, arr_street_stats)) {
        arr_street_stats[streetName][0] = EOS;
        strcat(arr_street_stats[streetName], name, size);
        map_set_arr(m_streetStats, id, arr_street_stats);

        new query[61 + 22 + 11 - 4 - 2 + 1];
        mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_stats SET `name` = '%e' WHERE `id` = %d LIMIT 1", name, id);
        mysql_tquery(MySQL_Connection(), query);
    }   
    return 1;
}

stock Street_SetNumber(id, number) {
    if(!map_has_key(m_streetStats, id)) {
        return 0;
    }

    new arr_street_stats[StreetStats];
    if(map_get_arr_safe(m_streetStats, id, arr_street_stats)) {
        arr_street_stats[streetNumber] = number;
        map_set_arr(m_streetStats, id, arr_street_stats);

        new query[61 + 11 + 11 - 4 + 1];
        mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_stats SET `number` = %d WHERE `id` = %d LIMIT 1", number, id);
        mysql_tquery(MySQL_Connection(), query);
    }
    return 1;
}

stock Street_SetType(id, StreetStatsType:type) {
    if(!map_has_key(m_streetStats, id)) {
        return 0;
    }

    new arr_street_stats[StreetStats];
    if(map_get_arr_safe(m_streetStats, id, arr_street_stats)) {
        arr_street_stats[streetNumber] = type;
        map_set_arr(m_streetStats, id, arr_street_stats);

        new query[59 + 11 + 11 - 4 + 1];
        mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_stats SET `type` = %d WHERE `id` = %d LIMIT 1", _:type, id);
        mysql_tquery(MySQL_Connection(), query);
    }
    return 1;
}

stock Street_GetTypeName(id, name[], size = sizeof(name)) {
    if(!map_has_key(m_streetStats, id)) {
        return 0;
    }

    new arr_street_stats[StreetStats];
    if(map_get_arr_safe(m_streetStats, id, arr_street_stats)) {
        name[0] = EOS;
        strcat(name, size, "Street");
        if(arr_street_stats[streetType] == Avenue) {
            strcat(name, size, "Avenue");
        } 
    }
    return 1;
}

stock Street_GetNumber(id) {
    if(!map_has_key(m_streetStats, id)) {
        return (-1);
    }
    
    new arr_street_stats[StreetStats];
    map_get_arr_safe(m_streetStats, id, arr_street_stats);
    return arr_street_stats[streetNumber];
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS street_stats(\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    number SMALLINT,\
    type TINYINT UNSIGNED,\
    name VARCHAR(22) DEFAULT '')", false);
    m_streetStats = map_new();
    return 1;
}