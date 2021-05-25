#if defined app_call_session_included
	#endinput
#endif

#define app_call_session_included

enum CallSessionStats {
	ssCreatedTime
}

static
	LinkedList:ll_callSessions, // LinkedList< LinkedList<phone_id> >
	Map:m_sessionStats, // { Ref<LinkedList>: int }
	Map:m_phoneCSessionRef; // { phone_id: Ref<LinkedList> }

stock Ref<LinkedList>:AppCall_GetPhoneSessionRef(id) {
	if(!map_valid(m_phoneCSessionRef) || !map_has_key(m_phoneCSessionRef, id)) {
		return Ref<LinkedList>:-1;
	}
	return Ref<LinkedList>:map_get(m_phoneCSessionRef, id);
}

stock Ref<LinkedList>:AppCall_CreateSession() {
	new
		LinkedList:session = linked_list_new(),
		ss_ref = pawn_ref<LinkedList>(session),
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
	if(AppCall_GetPhoneSessionRef(id) != -1) {
		return 0;
	}
	new LinkedList:ll = pawn_unref<LinkedList>(session_ref);
	if(linked_list_valid(ll)) {
		linked_list_add(ll, id);
	}
	return 1;
}
