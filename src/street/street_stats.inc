#if defined street_stats_included
	#endinput
#endif

#define street_stats_included

const STREET_NAME_LENGTH = 22;
const STREET_TYPE_NAME_LENGTH = 7;
const MAX_LINE_STREET_LIST = 30;

enum StreetType {  
    Avenue,
    Street
};

stock Task:Street_Has(id) {
    new Task:t = task_new();

    new query[51 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(id) FROM street_stats WHERE `id` = %d LIMIT 1", id);
    mysql_tquery(MySQL_Connection(), query, "Street_OnHasCheck", "d", _:t);
    task_set_result_ms(t, 0, 1000);
    return t;
}

stock Task:Street_Create(number, StreetType:type, const name[]) {
    new Task:t = task_new();
        
    new query[71 + 11 + 11 + 22 - 8 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO street_stats (number, type, name) VALUES (%d, %d, '%e')", number, _:type, name);
    mysql_pquery(MySQL_Connection(), query, "Street_OnCreated", "d", _:t);
    return t;
}

stock Task:Street_Delete(id) {
    new Task:t = task_new();

    new query[48 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM street_stats WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnDeleted", "d", _:t);
    return t;
}

stock Task:Street_SetName(id, const name[]) {
    new Task:t = task_new();

    new query[61 + 22 + 11 - 4 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_stats SET `name` = '%e' WHERE `id` = %d LIMIT 1", name, id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnNameSet", "d", _:t);   
    return t;
}

stock Task:Street_SetNumber(id, number) {
    new Task:t= task_new();   

    new query[61 + 11 + 11 - 4 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_stats SET `number` = %d WHERE `id` = %d LIMIT 1", number, id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnNumberSet", "d", _:t);
    return t;
}

stock Task:Street_SetType(id, StreetType:type) {
    new Task:t = task_new();

    new query[59 + 11 + 11 - 4 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_stats SET `type` = %d WHERE `id` = %d LIMIT 1", _:type, id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnTypeSet", "d", _:t);
    return t;
}

stock Task:Street_GetTypeName(id) {
    new Task:t = task_new();
    
    new query[53 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT type FROM street_stats WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnTypeNameGet", "d", _:t);  
    return t;
}

stock Task:Street_GetNumber(id) {
    new Task:t = task_new();

    new query[55 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT number FROM street_stats WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnNumberGet", "d", _:t);
    return t;
}

stock Task:Street_GetName(id) {
    new Task:t = task_new();

    new query[55 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT name FROM street_stats WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnNameGet", "d", _:t);
    return t;   
}

forward Street_OnHasCheck(Task:t);
public Street_OnHasCheck(Task:t) {
    new count;
    cache_get_value_name_int(0, "COUNT(id)", count);

    if(count > 0) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward Street_OnCreated(Task:t);
public Street_OnCreated(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, cache_insert_id());
        return 1;
    }

    task_set_result(t, -1);
    return 1;
}

forward Street_OnDeleted(Task:t);
public Street_OnDeleted(Task:t) {
    task_set_result(t, cache_affected_rows());
    return 1;
}

forward Street_OnNameSet(Task:t);
public Street_OnNameSet(Task:t) {
    task_set_result(t, cache_affected_rows());
    return 1;
}

forward Street_OnNumberSet(Task:t);
public Street_OnNumberSet(Task:t) {
    task_set_result(t, cache_affected_rows());
    return 1;
}

forward Street_OnTypeSet(Task:t);
public Street_OnTypeSet(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward Street_OnTypeNameGet(Task:t);
public Street_OnTypeNameGet(Task:t) {   
    if(cache_num_rows()) {
        new type;
        cache_get_value_name_int(0, "type", type);

        if(type == _:Avenue) {
            task_set_result_str(t, "Avenue");
            return 1;
        }

        task_set_result_str(t, "Street");
        return 1;
    }

    task_set_result_str(t, "");
    return 1;
}

forward Street_OnNumberGet(Task:t);
public Street_OnNumberGet(Task:t) { 
    if(cache_num_rows() > 0) {
        new number;
        cache_get_value_name_int(0, "number", number);
        task_set_result(t, number);
        return 1;
    }

    task_set_result(t, -1);
    return 1;
}

forward Street_OnNameGet(Task:t);
public Street_OnNameGet(Task:t) {
    if(cache_num_rows()) {
        new name[STREET_NAME_LENGTH];
        cache_get_value_name(0, "name", name);
        task_set_result_str(t, name);
        return 1;
    }

    task_set_result_str(t, "");
    return 1;
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
    return 1;
}
