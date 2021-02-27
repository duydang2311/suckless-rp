#include <a_samp>
#include <PawnPlus>

enum PlayerAuth {
	id,
	name[MAX_PLAYER_NAME + 1]
}

static Map:m_playerAuth;



#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	m_playerAuth = map_new();

	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS player_auth (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	id INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,\
	name VARCHAR(33) DEFAULT ''\
	);", false);
	return 1;
}