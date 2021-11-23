#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

stock Task:Frequency_Has(frequency) {
    new Task:t = task_new();
    task_detach();

    new query[50 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT `id` FROM `frequency_stats` WHERE `id` = %d", frequency);
    mysql_pquery(MySQL_Connection(), query, "Frequency_OnCheckingEmpty", "d", _:t); 
    task_set_result_ms(t, -1, 1000);
    return t;
}   

stock Frequency_Create(frequency) {
    new query[76 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO frequency_stats (id) VALUES (%d) ON DUPLICATE KEY UPDATE id = %d", frequency, frequency);
    mysql_pquery(MySQL_Connection(), query);
    return 1;
}

forward Frequency_OnCheck(Task:t);
public Frequency_OnCheck(Task:t) {
    if(cache_num_rows()) {
        task_set_result_ms(t, true, 1000);
        return 1;
    }

    task_set_result_ms(t, false, 1000);
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