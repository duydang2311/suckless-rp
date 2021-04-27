#if defined app_store_stats_included
	#endinput
#endif

#define app_store_stats_included

static enum _:AppStoreStats {
	appIndexOffset
}

static Map:m_storeStats;

stock AppStore_HasStats(id) {
	return (map_valid(m_storeStats) && map_has_key(m_storeStats, id));
}

stock AppStore_GetIndexOffset(id) {
	if(!AppStore_HasStats(id)) {
		return 0;
	}

	new stats[AppStoreStats];
	map_get_arr(m_storeStats, id, stats);
	return stats[appIndexOffset];
}

stock Iter:AppStore_GetAppListIter(id, index = 0) {
	if(!AppStore_HasStats(id)) {
		return Iter:-1;
	}
	return AppMenu_GetAppListIter(id, index);
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppStore, "App Store", "app_sprites:setting", false);
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppStore) {
		// To save time, AppStore initiation will rely on AppStore stats instead of retrieving phone external apps again, so no asynchronous process should be made.
		// Note: AppStore will always rely on AppStore stats, so never terminate AppStore while AppStore is being used
		if(!map_valid(m_storeStats)) {
			m_storeStats = map_new();
		}
		new stats[AppStoreStats];
		stats[appIndexOffset] = 0;
		map_set_arr(m_storeStats, id, stats);
		if(task_valid(result)) { // if AppStore initiation is called by IPApp_InitStatsAsync, just set the task result anyways to avoid undead task & memory leak
			task_unblock(Y_HOOKS_BREAK_RETURN_1);
			await task_ms(1);
			task_set_result(result, 1);
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPA_OnStatsTermnAttempt(id, app) {
	if(app == AppStore) {

	}
	return 1;
}

hook IPA_OnAppsGetAttempt(id, bool:arr[], size) {
	if(AppStore < size && AppStore_HasStats(id)) {
		arr[AppStore] = true;
	}
	return 1;
}

hook OnGameModeInit() {
	return 1;
}
