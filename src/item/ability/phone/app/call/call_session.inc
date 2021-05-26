#if defined app_call_session_included
	#endinput
#endif

#define app_call_session_included

#define CALL_SESSION_TEST

enum _:CallSessionErrors {
	ssErrorAlreadyInASession
}

enum _:CallSessionStats {
	ssOffset,
	ssCreatedTime
}

static
	LinkedList:ll_callSessions, // LinkedList< LinkedList<phone_id> >
	Map:m_sessionStats, // { Ref<LinkedList>: int }
	Map:m_phoneCSessionRef; // { phone_id: Ref<LinkedList> }

stock AppCall_HasSessionRef(Ref<LinkedList>:ssref) {
	return (map_valid(m_sessionStats) && map_has_key(m_sessionStats, _:ssref));
}

stock Ref<LinkedList>:AppCall_GetPhoneSessionRef(id) {
	if(!map_valid(m_phoneCSessionRef) || !map_has_key(m_phoneCSessionRef, id)) {
		return Ref<LinkedList>:-1;
	}
	return Ref<LinkedList>:map_get(m_phoneCSessionRef, id);
}

stock AppCall_GetSessionCreatedTime(Ref<LinkedList>:ssref) {
	if(!AppCall_HasSessionRef(ssref)) {
		return 0;
	}

	new stats[CallSessionStats];
	map_get_arr(m_sessionStats, _:ssref, stats);
	return stats[ssCreatedTime];
}

stock Ref<LinkedList>:AppCall_CreateSession() {
	new
		LinkedList:session = linked_list_new(),
		Ref<LinkedList>:ssref = pawn_ref<LinkedList>(session),
		stats[CallSessionStats];
	stats[ssCreatedTime] = gettime();
	stats[ssOffset] = 0;
	if(!linked_list_valid(ll_callSessions)) {
		ll_callSessions = linked_list_new();
	}
	if(!map_valid(m_sessionStats)) {
		m_sessionStats = map_new();
	}
	linked_list_add(ll_callSessions, _:session);
	map_set_arr(m_sessionStats, _:ssref, stats);
	return ssref;
}

stock AppCall_JoinSession(id, Ref<LinkedList>:session_ref) {
	if(AppCall_GetPhoneSessionRef(id) != Ref<LinkedList>:-1) {
		return 0;
	}
	printf("AppCall_JoinSession(%d, %d), passed condition 1", id, _:session_ref);
	new LinkedList:ll = pawn_unref<LinkedList>(session_ref);
	if(!linked_list_valid(ll)) {
		return 0;
	}
	printf("AppCall_JoinSession(%d, %d), passed condition 2", id, _:session_ref);
	linked_list_add(ll, id);
	if(!map_valid(m_phoneCSessionRef)) {
		m_phoneCSessionRef = map_new();
	}
	map_set(m_phoneCSessionRef, id, _:session_ref);
	printf("AppCall_JoinSession(%d, %d)", id, _:session_ref);
	return 1;
}

stock AppCall_QuitSession(id) {
	new Ref<LinkedList>:ssref = AppCall_GetPhoneSessionRef(id);
	if(ssref == Ref<LinkedList>:-1) {
		return 0;
	}

	if(map_valid(m_phoneCSessionRef) && map_has_key(m_phoneCSessionRef, id)) {
		map_remove(m_phoneCSessionRef, id);
	}
	new LinkedList:ll = pawn_unref<LinkedList>(ssref);
	if(linked_list_valid(ll)) {
		for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
			if(iter_get(it) == id) {
				iter_erase(it);
				CallLocalFunction("AppCall_OnSessionQuit", "ii", _:ssref, id);
				return 1;
			}
		}
	}
	return 0;
}

stock AppCall_DeleteSession(Ref<LinkedList>:ssref) {
	if(!AppCall_HasSessionRef(ssref)) {
		return 0;
	}

	CallLocalFunction("AppCall_OnSessDeleted", "i", _:ssref);

	// remove from m_sessionStats
	map_remove(m_sessionStats, _:ssref);
	if(!map_size(m_sessionStats)) {
		map_delete(m_sessionStats);
		m_sessionStats = Map:-1;
	}

	// getting phone ids in session and remove phone ids from m_phoneCSessionRef
	new LinkedList:ll = pawn_unref<LinkedList>(ssref);
	if(linked_list_valid(ll)) {
		for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
			if(map_valid(m_phoneCSessionRef) && map_has_key(m_phoneCSessionRef, iter_get(it))) {
				printf("AppCall_DeleteSession(%d), remove phone %d from phone csessionref", _:ssref, iter_get(it));
				map_remove(m_phoneCSessionRef, iter_get(it));
			}
		}
	}

	// in ll_callSessions, find the session which has the Ref equals to ssref, then erase from ll_callSessions and delete that session LinkedList
	if(linked_list_valid(ll_callSessions)) {
		for(new Iter:it = linked_list_iter(ll_callSessions); iter_inside(it); iter_move_next(it)) {
			if(pawn_ref<LinkedList>(LinkedList:iter_get(it)) == ssref) {
				new LinkedList:sessionll = pawn_unref<LinkedList>(ssref);
				iter_erase_deep(it);
				if(linked_list_valid(sessionll)) {
					linked_list_delete(sessionll);
				}
				break;
			}
		}
	}
	return 1;
}

stock AppCall_GetSessionOffset(Ref<LinkedList>:ssref) {
	if(!AppCall_HasSessionRef(ssref)) {
		return 0;
	}

	new stats[CallSessionStats];
	map_get_arr(m_sessionStats, _:ssref, stats);
	return stats[ssOffset];
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnInCallAccept(id) {
	new
		number[SIM_NUMBER_SIZE],
		phone;
	AppCall_GetIncomingNumber(id, number);
	phone = ISim_GetPhoneWithNumber(number);
	if(phone != -1) {
		new
			Ref<LinkedList>:ref1 = AppCall_GetPhoneSessionRef(id),
			Ref<LinkedList>:ref2 = AppCall_GetPhoneSessionRef(phone);
		printf("AppCall_OnInCallAccept(%d), ref1 %d, ref 2 %d", id, _:ref1, _:ref2);
		if(ref1 == ref2) {
			if(ref1 == Ref<LinkedList>:-1) {
				ref1 = AppCall_CreateSession();
			} else {
				CallLocalFunction("AppCall_OnSsJoinError", "ii", id, ssErrorAlreadyInASession);
				CallLocalFunction("AppCall_OnSsJoinError", "ii", phone, ssErrorAlreadyInASession);
				return 1;
			}
		} else {
			if(ref1 != Ref<LinkedList>:-1 && ref2 != Ref<LinkedList>:-1) {
				CallLocalFunction("AppCall_OnSsJoinError", "ii", id, ssErrorAlreadyInASession);
				CallLocalFunction("AppCall_OnSsJoinError", "ii", phone, ssErrorAlreadyInASession);
				return 1;
			}
			ref1 = (ref1 == Ref<LinkedList>:-1) ? ref2 : ref1;
		}
		AppCall_JoinSession(id, ref1);
		AppCall_JoinSession(phone, ref1);
		CallLocalFunction("AppCall_OnSessionJoined", "i", id);
		CallLocalFunction("AppCall_OnSessionJoined", "i", phone);
	}
	return 1;
}
