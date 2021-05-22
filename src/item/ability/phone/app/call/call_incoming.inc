#if defined app_call_incoming_included
	#endinput
#endif

#define app_call_incoming_included

static Map:m_incoming; // { phone_id: sim_number }

stock AppCall_HasIncoming(id) {
	printf("AppCall_HasIncoming(%d) ? %d", id, (map_valid(m_incoming) && map_has_key(m_incoming, id)));
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

stock AppCall_GetIncomingNumber(id, number[SIM_NUMBER_SIZE]) {
	if(!AppCall_HasIncoming(id)) {
		return 0;
	}
	map_get_str(m_incoming, id, number);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnDialCancel(id) {
	new
		dialPhone,
		number[SIM_NUMBER_SIZE];
	AppCall_GetDialSimNumber(id, number);
	dialPhone = ISim_GetPhoneWithNumber(number);
	if(AppCall_HasIncoming(dialPhone)) {
		if(map_get(m_incoming, dialPhone) == id) {
			AppCall_CancelIncoming(dialPhone);
		}
	}
	return 1;
}

hook AppCall_OnDialed(from_phone, const to_number[]) {
	if(!IPhone_Has(from_phone) || !IGroup_Has(from_phone)) {
		return 1;
	}

	new to_phone = ISim_GetPhoneWithNumber(to_number);
	if(to_phone == -1) {
		return 1;
	}

	new Iter:phone_it = IGroup_GetIter(from_phone);
	if(iter_valid(phone_it) && iter_release(phone_it)) {
		for(new sim; iter_inside(phone_it); iter_move_next(phone_it)) {
			sim = iter_get(phone_it);
			if(ISim_Has(sim)) {
				new number[SIM_NUMBER_SIZE];
				ISim_GetNumber(sim, number);
				if(!map_valid(m_incoming)) {
					m_incoming = map_new();
				}
				map_set_str(m_incoming, to_phone, number);
			}
		}
	}
	return 1;
}