#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

static Map:m_frequencyStats;

stock Frequency_Create(frequency) {
    if(!map_valid(m_frequencyStats)) {
        m_frequencyStats = map_new();
    }



    return map_size(m_frequencyStats);
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