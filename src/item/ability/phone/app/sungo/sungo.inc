#if defined app_sungo_included
	#endinput
#endif

#define app_sungo_included

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppSunGo, "SunGo", "app_sprites:setting", true);
	return 1;
}