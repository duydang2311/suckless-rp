#if defined app_call_session_included
	#endinput
#endif

#define app_call_session_included

#define CALL_SESSION_TEST

enum _:CallSessionStats {
	ssCreatedTime
}

static
	LinkedList:ll_callSessions, // LinkedList< LinkedList<phone_id> >
	Map:m_sessionStats, // { Ref<LinkedList>: int }
	Map:m_phoneCSessionRef; // { phone_id: Ref<LinkedList> }

stock AppCall_HasSessionRef(Ref<LinkedList>:ss_ref) {
	return (map_valid(m_sessionStats) && map_has_key(m_sessionStats, _:ss_ref));
}

stock Ref<LinkedList>:AppCall_GetPhoneSessionRef(id) {
	if(!map_valid(m_phoneCSessionRef) || !map_has_key(m_phoneCSessionRef, id)) {
		return Ref<LinkedList>:-1;
	}
	return Ref<LinkedList>:map_get(m_phoneCSessionRef, id);
}

stock AppCall_GetSessionCreatedTime(Ref<LinkedList>:ss_ref) {
	if(!AppCall_HasSessionRef(ss_ref)) {
		return 0;
	}

	new stats[CallSessionStats];
	map_get_arr(m_sessionStats, _:ss_ref, stats);
	return stats[ssCreatedTime];
}

stock Ref<LinkedList>:AppCall_CreateSession() {
	new
		LinkedList:session = linked_list_new(),
		Ref<LinkedList>:ss_ref = pawn_ref<LinkedList>(session),
		stats[CallSessionStats];
	stats[ssCreatedTime] = gettime();
	if(!linked_list_valid(ll_callSessions)) {
		ll_callSessions = linked_list_new();
	}
	if(!map_valid(m_sessionStats)) {
		m_sessionStats = map_new();
	}
	linked_list_add(ll_callSessions, _:session);
	map_set_arr(m_sessionStats, _:ss_ref, stats);
	return ss_ref;
}

stock AppCall_JoinSession(id, Ref<LinkedList>:session_ref) {
	if(AppCall_GetPhoneSessionRef(id) != Ref<LinkedList>:-1) {
		return 0;
	}
	new LinkedList:ll = pawn_unref<LinkedList>(session_ref);
	if(!linked_list_valid(ll)) {
		return 0;
	}
	linked_list_add(ll, id);
	if(!map_valid(m_phoneCSessionRef)) {
		m_phoneCSessionRef = map_new();
	}
	map_set(m_phoneCSessionRef, id, _:session_ref);
	return 1;
}

stock AppCall_QuitSession(id) {
	new Ref<LinkedList>:ssref = AppCall_GetPhoneSessionRef(id);
	if(ssref == Ref<LinkedList>:-1) {
		return 0;
	}

	new LinkedList:ll = pawn_unref<LinkedList>(ssref);
	if(linked_list_valid(ll)) {
		for(new Iter:it = linked_list_iter(ll); iter_inside(it); iter_move_next(it)) {
			if(iter_get(it) == id) {
				iter_erase(it);
				return 1;
			}
		}
	}
	return 0;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnInCallAccept(id) {
	new
		number[SIM_NUMBER_SIZE],
		Ref<LinkedList>:ss_ref,
		phone;
	AppCall_GetIncomingNumber(id, number);
	phone = ISim_GetPhoneWithNumber(number);
	if(phone != -1) {
		ss_ref = AppCall_GetPhoneSessionRef(id);
		if(ss_ref == Ref<LinkedList>:-1) {
			ss_ref = AppCall_GetPhoneSessionRef(phone);
			if(ss_ref == Ref<LinkedList>:-1) {
				ss_ref = AppCall_CreateSession();
			}
		}
		AppCall_JoinSession(id, ss_ref);
		AppCall_JoinSession(phone, ss_ref);
	}
	return 1;
}
