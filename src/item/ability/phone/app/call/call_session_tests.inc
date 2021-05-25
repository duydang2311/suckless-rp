#if defined app_call_session_test_included
	#endinput
#endif

#define app_call_session_test_included

#if defined CALL_SESSION_TEST

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	new Ref<LinkedList>:ss_ref = AppCall_CreateSession();
	AppCall_JoinSession(0, ss_ref);
	AppCall_JoinSession(1, ss_ref);

	new LinkedList:ll = pawn_unref<LinkedList>(ss_ref);
	for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
		printf("LinkedList:%d, ref %d, iter_get() = %d", _:ll, _:ss_ref, iter_get(it));
	}

	AppCall_QuitSession(0);
	for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
		printf("LinkedList:%d, ref %d, iter_get() = %d", _:ll, _:ss_ref, iter_get(it));
	}
	return 1;
}

#endif