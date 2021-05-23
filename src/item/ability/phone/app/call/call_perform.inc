#if defined app_call_perform_included
	#endinput
#endif

#define app_call_perform_included

static enum _:CallPerformStats {
	LinkedList:callRemotePhoneList, // LinkedList<phone_id>, useful for calling many people at once
	bool:callSpeaker
}

static Map:m_performStats; // { phone_id: {CallPerformStats} }

stock AppCall_HasPerformStats(id) {
	return (map_valid(m_performStats) && map_has_key(m_performStats, id));
}

stock AppCall_Perform(from_phone, to_phone) {
	if(!map_valid(m_performStats)) {
		m_performStats = map_new();
	}

	new stats[CallPerformStats];
	if(!map_has_key(m_performStats, from_phone)) {
		stats[callRemotePhoneList] = linked_list_new();
		stats[callSpeaker] = false;
		map_set_arr(m_performStats, from_phone, stats);
	}
	if(!map_has_key(m_performStats, to_phone)) {
		stats[callRemotePhoneList] = linked_list_new();
		stats[callSpeaker] = false;
		map_set_arr(m_performStats, to_phone, stats);
	}

	map_get_arr(m_performStats, from_phone, stats);
	linked_list_add_arr(stats[callRemotePhoneList], stats);
	map_get_arr(m_performStats, to_phone, stats);
	linked_list_add_arr(stats[callRemotePhoneList], stats);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnInCallAccept(id) {
	new
		number[SIM_NUMBER_SIZE],
		phone;
	AppCall_GetIncomingNumber(id, number);
	phone = ISim_GetPhoneWithNumber(number);
	if(phone != -1) {
		AppCall_Perform(id, phone);
	}
	return 1;
}
