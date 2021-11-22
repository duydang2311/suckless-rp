#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

#define                     MAX_LENGTH_FREQUENCY_PASSWORD                   22

enum _:FrequencyStats {
    frequencyOwnerID,
    frequencyPassword[MAX_LENGTH_FREQUENCY_PASSWORD]
};

static Map:m_frequencyStats;

stock Task:Frequency_Create(frequency) {
    new Task:t = task_new();
    task_detach();

    new query[68 + 11 - 2 + 1];
    mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT `frequency_v` FROM `frequency_stats` WHERE `frequency_v` = %d", frequency);
    mysql_tquery(MySQL_Connection(), query, "OnFrequencyCreated", "i", frequency);
    task_set_result_ms(t, -1, 2000);    
    return t;
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_frequencyStats = map_new();
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS frequency_stats(\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    frequency_v INT UNSIGNED DEFAULT 0,\
    owner INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    password VARCHAR(22) DEFAULT '',\
    FOREIGN KEY (owner) REFERENCES player_auth(id) ON UPDATE CASCADE ON DELETE CASCADE)", false);
    return 1;   
}