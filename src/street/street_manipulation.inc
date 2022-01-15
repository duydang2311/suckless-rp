#if defined street_manipulation_included
    #endinput
#endif

#define street_manipulation_included

static 
    Map:m_streetID,
    StreetExtraId;

stock Task:Street_CreateArea(streetid, Float:min_x, Float:min_y, Float:max_x, Float:max_y, worldid, interior) {
    if(!map_valid(m_streetID)) {
        m_streetID = map_new();
    }

    new 
        Task:t = task_new(),
        query[297 + 500];

    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO street_manipulation (id, min_x, min_y, max_x, max_y, interior, vw) VALUES (%d, %.4f, %.4f, %.4f, %.4f, %d, %d) ON UPDATE KEY UPDATE id = VALUES(streetid), min_x = VALUES(min_x), min_y = VALUES(min_y), max_x = VALUES(max_x), max_y = VALUES(max_y), interior = VALUES(interior), vw = VALUES (worldid)", streetid, min_x, min_y, max_x, max_y, interior, worldid);
    mysql_pquery(MySQL_Connection(), query, "Street_OnAreaCreated", "ddffffdd", _:t, streetid, min_x, min_y, max_x, max_y, worldid, interior);
    return t;
}

stock Task:Street_SetCoordinate(streetid, Float:min_x, Float:min_y, Float:max_x, Float:max_y) {
    new Task:t = task_new();
    if(!map_has_key(m_streetID, streetid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new areaid = map_get(m_streetID, streetid);
    if(!IsValidDynamicArea(areaid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new query[113 + 44 - 16 + 1];
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MIN_X, minX);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MIN_Y, minY);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MAX_X, maxX);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MAX_Y, maxY);
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_manipulation SET `min_x` = %.4f, `min_y` = %.4f, `max_x` = %.4f, `max_y` = %.4f WHERE `id` = %d LIMIT 1", min_x, min_y, max_x, max_y, id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnPositionSet", "d", _:t);
    return t;
}

stock Task:Street_SetInt(streetid, int) {
    new Task:t = task_new();
    if(!map_has_key(m_streetID, streetid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new areaid = map_get(m_streetID, streetid);
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
    if(!map_has_key(m_streetID, streetid)) {
        task_set_result_ms(t, 0, 0);
        return t;
    }

    new areaid = map_get(m_streetID, streetid);
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

forward Street_OnAreaCreated(Task:task, id, Float:min_x, Float:min_y, Float:max_x, Float:max_y, interior, worldid);
public Street_OnAreaCreated(Task:task, id, Float:min_x, Float:min_y, Float:max_x, Float:max_y, interior, worldid) {
    if(cache_affected_rows()) {   
        new areaid = CreateDynamicRectangle(min_x, min_y, max_x, max_y, worldid, interior);
        map_set(m_streetID, id, areaid);
        Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, StreetExtraId);
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
    StreetExtraId = Session_GenerateId();
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
    return 1;
}