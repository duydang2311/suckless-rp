#if defined app_sungo_included
	#endinput
#endif

#define app_sungo_included

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppSunGo, "SunGo", "app:sungo", true);
	return 1;
}

hook AppStore_OnStatsInit() {
	AppStore_Register(AppSunGo, "Dat hang, nhan hang nhanh chong voi SunGo!!", "Sunday Company", "0.1.0");
	return 1;
}