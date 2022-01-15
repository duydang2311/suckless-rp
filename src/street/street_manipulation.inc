#if defined street_manipulation_included
    #endinput
#endif

#define street_manipulation_included

static streetManipulateExtraId = 'S' + 'T' + 'R' + 'E' + 'E' + 'T';

static Map:m_streetID;  

stock Task:StreetManipulate_CreateArea(streetid, Float:xMin, Float:yMin, Float:xMax, Float:yMax, worldid, interior) {
    if(!map_valid(m_streetID)) {
        m_streetID = map_new();
    }

    new Task:t = task_new();
    if(map_has_key(m_streetID, streetid)) {
        task_set_result_ms(t, -1, 1);
        return t;
    }

    new query[500];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO street_manipulation (id, xmin, ymin, xmax, ymax, interior, vw) VALUES (%d, %.4f, %.4f, %.4f, %.4f, %d, %d)", streetid, xMin, yMin, xMax, yMax, interior, worldid);
    mysql_pquery(MySQL_Connection(), query, "StreetManipulate_OnAreaCreated", "ddffffdd", _:t, streetid, xMin, yMin, xMax, yMax, worldid, interior);
    return t;
}

stock Task:StreetManipulate_SetCoordinate(streetid, Float:xMin, Float:yMin, Float:xMax, Float:yMax) {
    new Task:t = task_new();
    if(!map_has_key(m_streetID, streetid)) {
        task_set_result_ms(t, -1, 1);
        return t;
    }

    new areaid = map_get(m_streetID, streetid);
    if(!IsValidDynamicArea(areaid)) {
        task_set_result_ms(t, -1, 1);
        return t;
    }

    new query[113 + 44 - 16 + 1];
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MIN_X, minX);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MIN_Y, minY);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MAX_X, maxX);
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_MAX_Y, maxY);
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_manipulation SET `xmin` = %.4f, `ymin` = %.4f, `xmax` = %.4f, `ymax` = %.4f WHERE `id` = %d LIMIT 1", xMin, yMin, xMax, yMax, id);
    mysql_pquery(MySQL_Connection(), query, "StreetManipulate_OnPositionSet", "d", _:t);
    return t;
}

stock Task:StreetManipulate_SetInt(streetid, int) {
    new Task:t = task_new();
    if(!map_has_key(m_streetID, streetid)) {
        task_set_result_ms(t, -1, 1);
        return t;
    }

    new areaid = map_get(m_streetID, streetid);
    if(!IsValidDynamicArea(areaid)) {
        task_set_result_ms(t, -1, 1);
        return t;
    } 

    new query[70 + 11 + 11 - 4 + 1];
    Streamer_SetFloatData(STREAMER_TYPE_AREA, areaid, E_STREAMER_INTERIOR_ID, int);
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_manipulation SET `interior` = %d WHERE `id` = %d LIMIT 1", int, streetid);
    mysql_pquery(MySQL_Connection(), query, "StreetManipulate_OnInteriorSet", "d", _:t);
    return t;
}

stock Task:StreetManipulate_SetWorldId(streetid, worldid) {
    new Task:t = task_new();
    if(!map_has_key(m_streetID, streetid)) {
        task_set_result_ms(t, -1, 1);
        return t;
    }

    new areaid = map_get(m_streetID, streetid);
    if(!IsValidDynamicArea(areaid)) {
        task_set_result_ms(t, -1, 1);
        return t;
    } 

    new query[64 + 11 + 11 - 4 + 1];
    Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_WORLD_ID, int);
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_manipulation SET `vw` = %d WHERE `id` = %d LIMIT 1", worldid, streetid);
    mysql_pquery(MySQL_Connection(), query, "StreetManipulate_OnWorldIdSet", "d", _:t);
    return t;
}

forward StreetManipulate_OnAreaCreated(Task:task, id, Float:xMin, Float:yMin, Float:xMax, Float:yMax, interior, worldid);
public StreetManipulate_OnAreaCreated(Task:task, id, Float:xMin, Float:yMin, Float:xMax, Float:yMax, interior, worldid) {
    if(cache_affected_rows()) {   
        new areaid = CreateDynamicRectangle(xMin, yMin, xMax, yMax, worldid, interior);
        map_set(m_streetID, id, areaid);
        Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, streetManipulateExtraId);
        task_set_result(task, 1);
        return 1;
    }

    task_set_result(task, 0);
    return 1;
}

forward StreetManipulate_OnPositionSet(Task:task);
public StreetManipulate_OnPositionSet(Task:task) {
    if(cache_affected_rows()) {
        task_set_result(task, 1);
    }

    task_set_result(task, 0);
    return 1;
}

forward StreetManipulate_OnInteriorSet(Task:task);
public StreetManipulate_OnInteriorSet(Task:task) {
    if(cache_affected_rows()) {
        task_set_result(task, 1);
    }
    task_set_result(task, 0);
    return 1;
}

forward StreetManipulate_OnWorldIdSet(Task:task);
public StreetManipulate_OnWorldIdSet(Task:task) {
    if(cache_affected_rows()) {
        task_set_result(task, 1);
    }

    task_set_result(task, 0);
    return 1;
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS street_manipulation (\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    xmin FLOAT(11, 4) DEFAULT 0.0000,\
    ymin FLOAT(11, 4) DEFAULT 0.0000,\
    xmax FLOAT(11, 4) DEFAULT 0.0000,\
    ymax FLOAT(11, 4) DEFAULT 0.0000,\
    interior INT UNSIGNED DEFAULT 0,\
	vw INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES street_stats(id) ON UPDATE CASCADE ON DELETE CASCADE)", false);
    return 1;
}