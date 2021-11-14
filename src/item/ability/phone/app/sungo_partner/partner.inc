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

forward SGoPartner_OnPasswordHashed(sim, const name[], Float:x, Float:y, Float:z, int, vw, Task:t);
public SGoPartner_OnPasswordHashed(sim, const name[], Float:x, Float:y, Float:z, int, vw, Task:t) {
	new
		hash[BCRYPT_HASH_LENGTH],
		query[300];
	bcrypt_get_hash(hash);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO sungo_partner (sim, hash, name, x, y, z, int, vw) VALUES (%d, '%e', '%e', %.4f, %.4f, %.4f, %d, %d)", sim, hash, name, x, y, z, int, vw);
	mysql_pquery(MySQL_Connection(), query, "_prv_SGoPartner_OnPartnerAdded", "i", _:t);
	return 1;
}

stock Task:SGoPartner_AddPartner(sim, const raw_password[], const name[], Float:x, Float:y, Float:z, int = 0, vw = 0) {
	new Task:t = task_new();
	bcrypt_hash(raw_password, 12, "SGoPartner_OnPasswordHashed", "isfffii", sim, name, x, y, z, int, vw, _:t);
	return t;
}

stock Task:SGoPartner_RemovePartner(sim) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "DELETE FROM sungo_partner WHERE sim = %d LIMIT 1", sim);
	mysql_pquery(MySQL_Connection(), query, "_prv_SGoPartner_OnPartnerRmvd", "i", _:t);
	return t;
}

stock Task:SGoPartner_HasPartnerAsync(sim) {
	new
		Task:t = task_new(),
		query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT COUNT(*) FROM sungo_partner WHERE sim = %d LIMIT 1", sim);
	mysql_pquery(MySQL_Connection(), query, "_prv_SGoPartner_OnPartnerHas", "i", _:t);
}

forward _prv_SGoPartner_OnPartnerAdded(Task:t);
public _prv_SGoPartner_OnPartnerAdded(Task:t) {
	if(task_valid(t)) {
		task_set_result(t, cache_affected_rows());
	}
	return 1;
}

forward _prv_SGoPartner_OnPartnerRmvd(Task:t);
public _prv_SGoPartner_OnPartnerRmvd(Task:t) {
	if(task_valid(t)) {
		task_set_result(t, cache_affected_rows());
	}
	return 1;
}

forward _prv_SGoPartner_OnPartnerHas(Task:t);
public _prv_SGoPartner_OnPartnerHas(Task:t) {
	new cnt = 0;
	cache_get_value_name_int(0, "COUNT(*)", cnt);
	if(task_valid(t)) {
		task_set_result(t, cnt);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(), "CREATE TABLE IF NOT EXISTS sungo_partner (\
		sim INT UNSIGNED PRIMARY KEY,\
		name VARCHAR(32) DEFAULT '',\
		x FLOAT(11, 4) DEFAULT 0.0000,\
		y FLOAT(11, 4) DEFAULT 0.0000,\
		z FLOAT(11, 4) DEFAULT 0.0000,\
		interior MEDIUMINT UNSIGNED DEFAULT 0,\
		vw MEDIUMINT UNSIGNED DEFAULT 0,\
		hash VARCHAR(61) DEFAULT '',\
		FOREIGN KEY (sim) REFERENCES item_sim(id) ON UPDATE CASCADE\
	)", false);
	return 1;
}