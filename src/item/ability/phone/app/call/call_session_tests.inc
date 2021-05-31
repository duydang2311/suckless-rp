#if defined app_call_session_test_included
	#endinput
#endif

#define app_call_session_test_included

#if defined CALL_SESSION_TEST

#define RUN_TESTS

#include <YSI_Core\y_testing>
#include <YSI_Coding\y_hooks>

Test:SessionTests() {
	new Ref<LinkedList>:ss_ref = AppCall_CreateSession();
	ASSERT(ss_ref != Ref<LinkedList>:-1);
	AppCall_JoinSession(0, ss_ref);
	AppCall_JoinSession(1, ss_ref);

	AppCall_QuitSession(0);
	ASSERT(!AppCall_HasSessionRef(ss_ref));
	return 1;
}

#endif