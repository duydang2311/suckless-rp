#if defined app_call_incoming_included
	#endinput
#endif

#define app_call_incoming_included

stock AppCall_HasIncoming(id) {
	return (map_valid(m_incoming) && map_has_key(m_incoming, id));
}

stock AppCall_CancelIncoming(id) {
	if(!AppCall_HasIncoming(id)) {
		return 0;
	}

	CallLocalFunction("AppCall_OnInCallCancel", "i", id); // process first, delete then
	map_remove(m_incoming, id);
	if(!map_size(m_incoming)) {
		map_delete(m_incoming);
		m_incoming = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnDialCancel(id) {
	new
		dialPhone,
		stats[CallDialStats];
	map_get_arr(m_dialStats, id, stats);
	dialPhone = ISim_GetPhoneWithNumber(stats[dialSimNumber]);
	if(AppCall_HasIncoming(dialPhone)) {
		if(map_get(m_incoming, dialPhone) == id) {
			AppCall_CancelIncoming(dialPhone);
		}
	}
	return 1;
}