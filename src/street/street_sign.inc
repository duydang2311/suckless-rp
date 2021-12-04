#if defined street_signs_included
	#endinput
#endif

#define street_signs_included

const STREET_NAME_LENGTH = 22;

enum StreetType {  
    Avenue,
    Street
};

stock Task:Street_Has(id) {
    new Task:t = task_new();
    task_detach();

    new query[51 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(id) FROM street_signs WHERE `id` = %d LIMIT 1", id);
    mysql_tquery(MySQL_Connection(), query, "Street_OnHasCheck", "d", _:t);
    task_set_result_ms(t, -1, 1000);
    return t;
}

stock Task:Street_Create(number, StreetType:type, const name[]) {
    new Task:t = task_new();
    task_detach();
        
    new query[71 + 11 + 11 + 22 - 8 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO street_signs (number, type, name) VALUES (%d, %d, '%e')", number, _:type, name);
    mysql_pquery(MySQL_Connection(), query, "Street_OnCreated", "d", _:t);
    return t;
}

stock Task:Street_Delete(id) {
    new Task:t = task_new();
    task_delete(t);

    new query[48 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM street_signs WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "OnStreetDelete", "d", _:t);
    return t;
}

stock Task:Street_SetName(id, const name[]) {
    new Task:t = task_new();
    task_detach();

    new query[61 + 22 + 11 - 4 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_signs SET `name` = '%e' WHERE `id` = %d LIMIT 1", name, id);
    mysql_pquery(MySQL_Connection(), query, "Street_OnNameSet", "d", _:t);   
    return t;
}

stock Task:Street_SetNumber(id, number) {
    new check = await Street_Has(id);
    if(!check) {
        return Task:0;
    }

    new Task:t= task_new();
    task_detach();

    new query[61 + 11 + 11 - 4 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_signs SET `number` = %d WHERE `id` = %d LIMIT 1", number, id);
    mysql_pquery(MySQL_Connection(), query, "OnStreetSetNumber", "d", _:t);
    return 1;
}

stock Task:Street_SetType(id, StreetType:type) {
    new check = await Street_Has(id);
    if(!check) {
        return Task:0;
    }

    new Task:t= task_new();
    task_detach();

    new query[59 + 11 + 11 - 4 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE street_signs SET `type` = %d WHERE `id` = %d LIMIT 1", _:type, id);
    mysql_pquery(MySQL_Connection(), query, "OnStreetSetType", "d", _:t);
    return 1;
}

stock Task:Street_GetTypeName(id) {
    new check = await Street_Has(id);
    if(!check) {
        return Task:0;
    }

    new Task:t = task_new();
    task_detach();

    new query[53 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT type FROM street_signs WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "OnStreetGetTypeName", "d", _:t);
    return t;
}

stock Task:Street_GetNumber(id) {
    new check = await Street_Has(id);
    if(!check) {
        return Task:0;
    }
    
    new Task:t = task_new();
    task_detach();

    new query[55 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT number FROM street_signs WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "OnStreetGetNumber", "d", _:t);
    return t;
}

stock Task:Street_GetName(id) {
    new check = await Street_Has(id);
    if(!check) {
        return Task:0;
    }

    new Task:t = task_new();
    task_detach();

    new query[55 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT name FROM street_signs WHERE `id` = %d LIMIT 1", id);
    mysql_pquery(MySQL_Connection(), query, "OnStreetGetName", "d", _:t);
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
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward OnStreetDelete(Task:t);
public OnStreetDelete(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward Street_OnNameSet(Task:t);
public Street_OnNameSet(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward OnStreetSetNumber(Task:t);
public OnStreetSetNumber(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward OnStreetSetType(Task:t);
public OnStreetSetType(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward OnStreetGetTypeName(Task:t);
public OnStreetGetTypeName(Task:t) {
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

forward OnStreetGetNumber(Task:t);
public OnStreetGetNumber(Task:t) {
    if(cache_num_rows() > 0) {
        new number;
        cache_get_value_name_int(0, "number", number);
        task_set_result(t, number);
    }

    task_set_result(t, -1);
    return 1;
}

forward OnStreetGetName(Task:t);
public OnStreetGetName(Task:t) {
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
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS street_signs(\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    number SMALLINT,\
    type TINYINT UNSIGNED,\
    name VARCHAR(22) DEFAULT '')", false);
    return 1;
}