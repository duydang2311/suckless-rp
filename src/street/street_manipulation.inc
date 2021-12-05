#if defined street_manipulation_included
    #endinput
#endif

#define street_manipulation_included

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS street_manipulation (\
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP NULL ON UPDATE CURRENT_TIMESTAMP,\
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,\
    x1 FLOAT(11, 4) DEFAULT 0.0000,\
    y1 FLOAT(11, 4) DEFAULT 0.0000,\
    x2 FLOAT(11, 4) DEFAULT 0.0000,\
    y2 FLOAT(11, 4) DEFAULT 0.0000,\
    interior INT UNSIGNED DEFAULT 0,\
	vw INT UNSIGNED DEFAULT 0,\
	FOREIGN KEY (id) REFERENCES street_stats(id) ON UPDATE CASCADE ON DELETE CASCADE)", false);
    return 1;
}