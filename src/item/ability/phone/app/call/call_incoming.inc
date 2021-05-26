#if defined app_call_incoming_included
	#endinput
#endif

#define app_call_incoming_included

static Map:m_incoming; // { phone_id: sim_number }

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

stock AppCall_AcceptIncoming(id) {
	if(!AppCall_HasIncoming(id)) {
		return 0;
	}

	CallLocalFunction("AppCall_OnInCallAccept", "i", id); // process first, delete then
	map_remove(m_incoming, id);
	if(!map_size(m_incoming)) {
		map_delete(m_incoming);
		m_incoming = Map:-1;
	}
	return 1;
}

stock AppCall_GetIncomingNumber(id, number[SIM_NUMBER_SIZE]) {
	if(!AppCall_HasIncoming(id)) {
		return 0;
	}
	map_get_str(m_incoming, id, number);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnDialCancel@(id) {
	new
		dialPhone,
		number[SIM_NUMBER_SIZE];
	AppCall_GetDialSimNumber(id, number);
	dialPhone = ISim_GetPhoneWithNumber(number);
	if(AppCall_HasIncoming(dialPhone)) {
		map_get_str(m_incoming, dialPhone, number);
		if(ISim_GetPhoneWithNumber(number) == id) {
			map_remove(m_incoming, dialPhone);
			if(!map_size(m_incoming)) {
				map_delete(m_incoming);
				m_incoming = Map:-1;
			}
		}
	}
	return 1;
}

hook AppCall_OnDialed(from_phone, const to_number[]) {
	if(!IPhone_Has(from_phone) || !IGroup_Has(from_phone)) {
		return 1;
	}

	new to_phone = ISim_GetPhoneWithNumber(to_number);
	printf("AppCall_OnDialed(%d, %s), to_phone %d", from_phone, to_number, to_phone);
	if(to_phone == -1) {
		return 1;
	}

	new Iter:phone_it = IGroup_GetIter(from_phone);
	if(iter_valid(phone_it) && iter_release(phone_it)) {
		for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
			printf("AppCall_OnDialed(%d, %s), to_phone %d, iterating sim %d", from_phone, to_number, to_phone, sim);
			sim = iter_get(phone_it);
			if(ISim_Has(sim)) {
				new number[SIM_NUMBER_SIZE];
				ISim_GetNumber(sim, number);
				printf("AppCall_OnDialed(%d, %s), to_phone %d, iterating sim %d, number %s", from_phone, to_number, to_phone, sim, number);
				if(!map_valid(m_incoming)) {
					m_incoming = map_new();
				}
				map_set_str(m_incoming, to_phone, number);
			}
		}
	}
	return 1;
}