#if defined app_store_included
	#endinput
#endif

#define app_store_included

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppStore, "App Store", "app_sprites:setting", false);
	return 1;
}