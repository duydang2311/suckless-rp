#if defined sungo_partner_included
	#endinput
#endif

#define sungo_partner_included

const SGO_PARTNER_NAME_SIZE = 32;

enum SGoPartner {
	PartnerSim,
	PartnerName[SGO_PARTNER_NAME_SIZE],
	Float:PartnerPos[3],
	PartnerInt,
	PartnerVW,
	LinkedList:PartnerProducts // LinkedList<SGoProduct>
}

stock SGoPartner_AddPartner(sim, const name[], Float:x, Float:y, Float:z, int = 0, vw = 0) {

	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS sungo_partner (\
		sim INT UNSIGNED UNIQUE,\
		name VARCHAR(32) DEFAULT '',\
		x FLOAT(11, 4) DEFAULT 0.0000,\
		y FLOAT(11, 4) DEFAULT 0.0000,\
		z FLOAT(11, 4) DEFAULT 0.0000,\
		int MEDIUMINT UNSIGNED DEFAULT 0,\
		vw MEDIUMINT UNSIGNED DEFAULT 0,\
		hash VARCHAR(61) DEFAULT '',\
		FOREIGN KEY (sim) REFERENCES item_sim(id) ON UPDATE CASCADE\
	)", false);
	return 1;
}