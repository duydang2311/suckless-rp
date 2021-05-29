#if defined app_sungo_partner_included
	#endinput
#endif

#define app_sungo_partner_included

#include ".\item\ability\phone\app\sungo_partner\product.inc"
#include ".\item\ability\phone\app\sungo_partner\partner.inc"
#include ".\item\ability\phone\app\sungo_partner\entry_ui.inc"

static Map:m_has; // { phone_id: int }

stock AppSunGoPartner_Has(id) {
	return (map_valid(m_has) && map_has_key(m_has, id));
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppSunGoPartner, "SunGo Partner", "app:sungo", true);
	return 1;
}

hook AppStore_OnStatsInit() {
	AppStore_Register(AppSunGoPartner, "Dang ky cua hang doi tac cua cong ty SunGo va quan ly cua hang tren ung dung de dang!!", "Sunday Company", "0.1.0");
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppSunGoPartner) {
		if(task_valid(result)) {
			task_unblock(Y_HOOKS_BREAK_RETURN_1);
			await task_ms(1);
			task_set_result(result, 1);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppSunGoPartner) {

		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppSunGoPartner < size && AppSunGoPartner_Has(id)) {
		arr[AppSunGoPartner] = true;
	}
	return 1;
}

hook OnGameModeInit@() {
	cache_delete(mysql_query(MySQL_Connection(), "INSERT INTO item_sim (number) VALUES ('1729') ON DUPLICATE KEY UPDATE id = VALUES(id)", true));
	printf("INFO: SunGo Partner registered sim numbered '1729'");
	return 1;
}