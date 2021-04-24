#if defined app_now_included
	#endinput
#endif

#define app_now_included

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppNow, "App Now", "app_sprites:setting", true);
	return 1;
}