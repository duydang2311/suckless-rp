#if defined app_call_session_included
	#endinput
#endif

#define app_call_session_included

static
	LinkedList:ll_callSessions, // LinkedList< LinkedList<phone_id> >
	Map:m_phoneCSessionRef; // { phone_id: Ref<LinkedList> }