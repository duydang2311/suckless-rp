#if defined frequency_stats_included
    #endinput
#endif

#define frequency_stats_included

#define                     MAX_LENGTH_FREQUENCY_PASSWORD                   22

enum _:FrequencyStats {
    frequencyOwnerID,
    frequencyPassword[MAX_LENGTH_FREQUENCY_PASSWORD]
};

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