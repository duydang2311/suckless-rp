#if defined street_manipulation_included
    #endinput
#endif

#define street_manipulation_included

#include <YSI_Coding\y_unique>

const StreetExtraID = UNIQUE_SYMBOL;

static Map:m_streetAreaID;

stock Task:Street_CreateArea(streetid, Float:minX, Float:minY, Float:maxX, Float:maxY, worldid, interior) {
    if(!map_valid(m_streetAreaID)) {
        m_streetAreaID = map_new();
    }

    new 
        Task:t = task_new(),
        query[297 + 500];

    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO street_manipulation (id, min_x, min_y, max_x, max_y, interior, vw) VALUES (%d, %.4f, %.4f, %.4f, %.4f, %d, %d) ON DUPLICATE KEY UPDATE id = VALUES(id), min_x = VALUES(min_x), min_y = VALUES(min_y), max_x = VALUES(max_x), max_y = VALUES(max_y), interior = VALUES(interior), vw = VALUES (vw)", streetid, minX, minY, maxX, maxY, interior, worldid);
    mysql_pquery(MySQL_Connection(), query, "Street_OnAreaCreated", "ddffffdd", _:t, streetid, minX, minY, maxX, maxY, worldid, interior);
    return t;
}

stock Task:Street_SetCoordinate(streetid, Float:minX, Float:minY, Float:maxX, Float:maxY) {
    new Task:t = task_new();
    if(!map_has_key(m_streetAreaID, streetid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new areaid = map_get(m_streetAreaID, streetid);
    if(!IsValidDynamicArea(areaid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new query[113 + 44 - 16 + 1];
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MIN_X, minX);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MIN_Y, minY);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MAX_X, maxX);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MAX_Y, maxY);
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_manipulation SET `min_x` = %.4f, `min_y` = %.4f, `max_x` = %.4f, `max_y` = %.4f WHERE `id` = %d LIMIT 1", minX, minY, maxX, maxY, id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnPositionSet", "d", _:t);
    return t;
}

stock Task:Street_SetInt(streetid, int) {
    new Task:t = task_new();
    if(!map_has_key(m_streetAreaID, streetid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new areaid = map_get(m_streetAreaID, streetid);
    if(!IsValidDynamicArea(areaid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    } 

    new query[70 + 11 + 11 - 4 + 1];
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_INTERIOR_ID, int);
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_manipulation SET `interior` = %d WHERE `id` = %d LIMIT 1", int, streetid);
    mysql_pquery(MySQL_Connection(), query, "Street_OnInteriorSet", "d", _:t);
    return t;
}

stock Task:Street_SetWorldId(streetid, worldid) {
    new Task:t = task_new();
    if(!map_has_key(m_streetAreaID, streetid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new areaid = map_get(m_streetAreaID, streetid);
    if(!IsValidDynamicArea(areaid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    } 

    new query[64 + 11 + 11 - 4 + 1];
    Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_WORLD_ID, int);
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_manipulation SET `vw` = %d WHERE `id` = %d LIMIT 1", worldid, streetid);
    mysql_pquery(MySQL_Connection(), query, "Street_OnWorldIdSet", "d", _:t);
    return t;
}

stock Street_GetIDByArea(areaid) {
    for(new Iter:it = map_iter(m_streetAreaID); iter_inside(it); iter_move_next(it)) {
        if(iter_get_value(it) == areaid) {
            return iter_get_key(it);
        }
    }

    return (-1);
}

forward Street_OnAreaCreated(Task:task, id, Float:minX, Float:minY, Float:maxX, Float:maxY, interior, worldid);
public Street_OnAreaCreated(Task:task, id, Float:minX, Float:minY, Float:maxX, Float:maxY, interior, worldid) {
    if(cache_affected_rows()) {   
        new areaid = CreateDynamicRectangle(minX, minY, maxX, maxY, worldid, interior);
        map_set(m_streetAreaID, id, areaid);
        Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, StreetExtraID);
        task_set_result(task, cache_insert_id());
        return 1;
    }

    task_set_result(task, -1);
    return 1;
}

forward Street_OnPositionSet(Task:task);
public Street_OnPositionSet(Task:task) {
    task_set_result(task, cache_affected_rows());
    return 1;
}

forward Street_OnInteriorSet(Task:task);
public Street_OnInteriorSet(Task:task) {
    task_set_result(task, cache_affected_rows());
    return 1;
}

forward Street_OnWorldIdSet(Task:task);
public Street_OnWorldIdSet(Task:task) {
    task_set_result(task, cache_affected_rows());
    return 1;
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_streetAreaID = map_new();
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS street_manipulation (\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    min_x FLOAT(11, 4) DEFAULT 0.0000,\
    min_y FLOAT(11, 4) DEFAULT 0.0000,\
    max_x FLOAT(11, 4) DEFAULT 0.0000,\
    max_y FLOAT(11, 4) DEFAULT 0.0000,\
    interior INT UNSIGNED DEFAULT 0,\
	vw INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES street_stats(id) ON UPDATE CASCADE ON DELETE CASCADE)", false);

    new Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM street_manipulation", true);
    new rows = cache_num_rows();
    if(rows) {
        for(new i = 0, streetid, Float:minX, Float:minY, Float:maxX, Float:maxY, interior, worldid; i < rows; ++i) {
            cache_get_value_name_int(i, "id", streetid);
            cache_get_value_name_float(i, "min_x", minX);
            cache_get_value_name_float(i, "min_y", minY);
            cache_get_value_name_float(i, "max_x", maxX);
            cache_get_value_name_float(i, "max_y", maxY);
            cache_get_value_name_int(i, "interior", interior);
            cache_get_value_name_int(i, "vw", worldid);

            new areaid = CreateDynamicRectangle(minX, minY, maxX, maxY, worldid, interior);
            map_set(m_streetAreaID, streetid, areaid);
            Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, StreetExtraID);
        }
    }

    cache_delete(result);
    return 1;
}