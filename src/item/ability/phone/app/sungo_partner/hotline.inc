#if defined sungo_partner_hotline_included
	#endinput
#endif

#define sungo_partner_hotline_included

static const SGO_PARTNER_HOTLINE_NUMBER[SIM_NUMBER_SIZE] = "1729";
static hotline_sim = -1;

stock SGoPartner_GetHotlineSim() {
	return hotline_sim;
}

stock SGoPartner_GetHotlineNumber(number[], size = sizeof(number)) {
	number[0] = EOS;
	strcat(number, SGO_PARTNER_HOTLINE_NUMBER, size);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit@() {
	new query[128];
	mysql_format(MySQL_Connection(), query, sizeof(query), "SELECT id FROM item_sim WHERE number = '%e' LIMIT 1", SGO_PARTNER_HOTLINE_NUMBER);
	new Cache:result = mysql_query(MySQL_Connection(), query, true);
	if(cache_num_rows()) {
		cache_get_value_name_int(0, "id", hotline_sim);
		cache_delete(result);
		return 1;
	}

	cache_delete(result);
	mysql_format(MySQL_Connection(), query, sizeof(query), "INSERT INTO item_sim (number) VALUES ('%e')", SGO_PARTNER_HOTLINE_NUMBER);
	result = mysql_query(MySQL_Connection(), query, true);
	if(cache_affected_rows()) {
		hotline_sim = cache_insert_id();
	}
	cache_delete(result);

	if(hotline_sim == -1) {
		printf("WARNING: SunGo Partner failed to register hotline numbered '%s'", SGO_PARTNER_HOTLINE_NUMBER);
	} else {
		printf("INFO: SunGo Partner successfully registered hotline numbered '%s' (id: %d)", SGO_PARTNER_HOTLINE_NUMBER, hotline_sim);
	}
	return 1;
}