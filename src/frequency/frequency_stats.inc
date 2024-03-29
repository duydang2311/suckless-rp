#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

#define                         MAX_FREQUENCY_PASSWORD_LENGTH              22

stock Task:Frequency_Create(frequency) {
    new Task:t = task_new();
    task_detach();

    new query[76 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO frequency_stats (id) VALUES (%d) ON DUPLICATE KEY UPDATE id = %d", frequency, frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnCreated", "dd", _:t, frequency);
    return t;
}

stock Task:Frequency_Delete(frequency) {
    new Task:t = task_new();
    task_detach();

    new query[53 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM `frequency_stats` WHERE `id` = %d LIMIT 1", frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnDeleted", "d", _:t);
    return t;
}

stock Task:Frequency_SetOwnerID(frequency, ownerid) {
    new Task:t = task_new();
    task_detach();

    new query[65 + 11 + 11 - 4 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE `frequency_stats` SET `owner` = %d WHERE `id` = %d LIMIT 1", ownerid, frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnSetOwner", "d", _:t);
    return t;
}

stock Task:Frequency_SetPassword(frequency, const password[]) {
    new Task:t = task_new();
    task_detach();

    new query[70 + 22 - 4 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE `frequency_stats` SET `password` = '%e' WHERE `id` = %d LIMIT 1", password, frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnSetPassword", "d", _:t);
    return t;
}

stock Task:Frequency_Has(frequency) {
    new Task:t = task_new();
    task_detach();

    new query[50 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(id) FROM frequency_stats WHERE id = %d LIMIT 1", frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnCheck", "d", _:t); 
    return t;
}   

forward Frequency_OnCheck(Task:t);
public Frequency_OnCheck(Task:t) {
    new count;
    cache_get_value_name_int(0, "COUNT(id)", count);

    if(count > 0) {
        task_set_result(t, true);
        return 1;
    }

    task_set_result(t, false);
    return 1;
}

stock Task:Frequency_GetOwnerID(frequency) {
    new Task:t = task_new();
    task_detach();

    new query[53 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT `owner` FROM `frequency_stats` WHERE `id` = %d LIMIT 1", frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnGetOwner", "d", _:t);
    return t;
}

stock Task:Frequency_GetPassword(frequency) {
    new Task:t = task_new();
    task_detach();

    new query[56 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT `password` FROM `frequency_stats` WHERE `id` = %d LIMIT 1", frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnGetPassword", "d", _:t);
    return t;
}

forward Frequency_OnCreated(Task:t, frequency);
public Frequency_OnCreated(Task:t, frequency) {
    if(cache_affected_rows()) {
        task_set_result(t, frequency);
        return 1;
    }

    task_set_result(t, -1);
    return 1;
}

forward Frequency_OnDeleted(Task:t);
public Frequency_OnDeleted(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward Frequency_OnSetOwner(Task:t);
public Frequency_OnSetOwner(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward Frequency_OnSetPassword(Task:t);
public Frequency_OnSetPassword(Task:t) {
    if(cache_affected_rows()) {
        task_set_result(t, 1);
        return 1;
    }

    task_set_result(t, 0);
    return 1;
}

forward Frequency_OnGetOwner(Task:t);
public Frequency_OnGetOwner(Task:t) {
    if(cache_num_rows()) {
        new value;
        cache_get_value_name_int(0, "owner", value);
        task_set_result(t, value);
        return 1;
    }

    task_set_result(t, -1);
    return 1;   
}

forward Frequency_OnGetPassword(Task:t);
public Frequency_OnGetPassword(Task:t) {
    if(cache_num_rows()) {
        new value[MAX_FREQUENCY_PASSWORD_LENGTH + 1];
        cache_get_value_name(0, "password", value);
        task_set_result_str(t, value);
        return 1;
    }

    task_set_result_str(t, "");
    return 1;   
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS frequency_stats(\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    id INT UNSIGNED PRIMARY KEY,\
    owner INT UNSIGNED,\
    password VARCHAR(22) DEFAULT '',\
    FOREIGN KEY (owner) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE)", false);
    return 1;   
}