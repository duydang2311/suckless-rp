#if defined app_store_stats_included
	#endinput
#endif

#define app_store_stats_included

static Map:m_storeStats;

stock AppStore_HasStats(id) {
	return (map_valid(m_storeStats) && map_has_key(m_storeStats, id));
}

stock Iter:AppStore_GetStatsIter(id, index = 0) {
	new Iter:it = Iter:-1;
	if(AppStore_HasStats(id)) {
		new LinkedList:ll = LinkedList:map_get(m_storeStats, id);
		it = linked_list_iter(ll, index);
		iter_acquire(it);
	}
	return it;
}

#include <YSI_Coding\y_hooks>

hook IPA_OnEnsureRequest() {
	IPApp_Ensure(AppStore, "App Store", "app_sprites:setting", false);
	return 1;
}

hook IPA_OnStatsInitAttempt(id, app, Task:result) {
	if(app == AppStore) {
		// To save time, AppStore initiation will rely on AppMenu stats instead of retrieving phone external apps again, so no asynchronous process should be made.
		// Note: AppStore will always rely on AppMenu stats, so never terminate AppMenu while AppStore is being used
		new
			Iter:it = AppMenu_GetStatsIter(id),
			store;
		if(iter_valid(it) && iter_release(it)) {

		}
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
