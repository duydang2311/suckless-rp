#if defined app_sungo_included
	#endinput
#endif

#define app_sungo_included

const
	SG_PARTNER_NAME_SIZE = 32,
	SG_GOODS_NAME_SIZE = 32;

static enum _:Goods {
	GName[SG_GOODS_NAME_SIZE],
	GPrice
}

static enum _:Partner {
	PId,
	PName[SG_PARTNER_NAME_SIZE],
	Float:PPos[3],
	LinkedList:PGoods
}

static LinkedList:sungo; // LinkedList<Partner>

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppSunGo, "SunGo", "app:sungo", true);
	return 1;
}

hook AppStore_OnStatsInit() {
	AppStore_Register(AppSunGo, "Dat hang, nhan hang nhanh chong voi SunGo!!", "Sunday Company", "0.1.0");
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppStore) {
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
	if(app == AppSunGo) {

		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	// if(AppSunGo < size && AppSunGo_HasStats(id)) {
	// 	arr[AppSunGo] = true;
	// }
	return 1;
}

hook OnGameModeInit() {

	return 1;
}