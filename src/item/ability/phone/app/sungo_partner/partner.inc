#if defined sungo_partner_included
	#endinput
#endif

#define sungo_partner_included

const SGO_PARTNER_NAME_SIZE = 32;

enum SGoPartner {
	PartnerSim,
	PartnerName[SGO_PARTNER_NAME_SIZE],
	LinkedList:PartnerProducts // LinkedList<SGoProduct>
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS sungo_partner (\
		sim INT UNSIGNED UNIQUE,\
		name VARCHAR(32) DEFAULT '',\
		hash VARCHAR(61) DEFAULT '',\
		FOREIGN KEY (sim) REFERENCES item_sim(id) ON UPDATE CASCADE\
	)", false);
	return 1;
}