#if defined app_call_dial_included
	#endinput
#endif

#define app_call_dial_included

const APP_CALL_DIAL_DELAY_MS = 4000;

enum _:CallDialStats {
	dialSimNumber[SIM_NUMBER_SIZE]
}

enum _:DialErrors {
	errRemoteBusy,
	errRemoteInvalid,
	errRemoteNotReachable
}

static Map:m_dialStats; // { phone_id: {CallDialStats} }

stock AppCall_HasDialStats(id) {
	return (map_valid(m_dialStats) && map_has_key(m_dialStats, id));
}

stock AppCall_GetDialStats(id, stats[CallDialStats]) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	map_get_arr(m_dialStats, id, stats);
	return 1;
}

stock AppCall_GetDialSimNumber(id, number[SIM_NUMBER_SIZE]) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	new stats[CallDialStats];
	map_get_arr(m_dialStats, id, stats);
	number[0] = EOS;
	strcat(number, stats[dialSimNumber]);
	return 1;
}

stock AppCall_Dial(id, const to_number[]) {
	if(!IPhone_Has(id) || !IGroup_Has(id)) {
		return 0;
	}

	new Iter:it = IGroup_GetIter(id);
	if(iter_valid(it) && iter_release(it)) {
		for(new sim; iter_inside(it); iter_move_next(it)) {
			sim = iter_get(it);
			if(ISim_Has(sim)) {
				new stats[CallDialStats];
				strcat(stats[dialSimNumber], to_number, SIM_NUMBER_SIZE);
				if(isnull(stats[dialSimNumber])) {
					strcat(stats[dialSimNumber], "Unknown");
				}
				if(!map_valid(m_dialStats)) {
					m_dialStats = map_new();
				}
				map_set_arr(m_dialStats, id, stats);
				CallLocalFunction("AppCall_OnPreDialed", "is", sim, stats[dialSimNumber]);
				SetPreciseTimer("AppCall_OnDialed", APP_CALL_DIAL_DELAY_MS, false, "is", id, stats[dialSimNumber]);
				return 1;
			}
		}
	}
	return 0;
}

stock AppCall_ThrowDialError(from_phone, error) {
	if(!AppCall_HasDialStats(from_phone)) {
		return 0;
	}

	CallLocalFunction("AppCall_OnDialError", "ii", from_phone, error);
	map_remove(m_dialStats, from_phone);
	if(!map_size(m_dialStats)) {
		map_delete(m_dialStats);
		m_dialStats = Map:-1;
	}
	return 1;
}

stock AppCall_CancelDial(id) {
	if(!AppCall_HasDialStats(id)) {
		return 0;
	}

	CallLocalFunction("AppCall_OnDialCancel", "i", id); // process first, delete then
	map_remove(m_dialStats, id);
	if(!map_size(m_dialStats)) {
		map_delete(m_dialStats);
		m_dialStats = Map:-1;
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook AppCall_OnTermnAttempt(id) {
	AppCall_CancelDial(id);
	return 1;
}

hook AppCall_OnDialed(from_phone, const to_number[]) {
	new phone = ISim_GetPhoneWithNumber(to_number);
	if(phone == -1) {
		// we are not using task_unblock here because we want other hooks to wait for this operation
		phone = await ISim_RetrieveByNumber(to_number);
		if(phone != -1) {
			AppCall_ThrowDialError(from_phone, errRemoteNotReachable);
			return Y_HOOKS_BREAK_RETURN_1;
		} else {
			AppCall_ThrowDialError(from_phone, errRemoteInvalid);
			return Y_HOOKS_BREAK_RETURN_1;
		}
	}
	if(AppCall_HasDialStats(phone) || AppCall_HasIncoming(phone)) {
		AppCall_ThrowDialError(from_phone, errRemoteBusy);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook AppCall_OnInCallCancel@(id) {
	new number[SIM_NUMBER_SIZE];
	AppCall_GetIncomingNumber(id, number);
	new phone = ISim_GetPhoneWithNumber(number);
	if(phone != -1 && AppCall_HasDialStats(phone)) {
		map_remove(m_dialStats, phone);
		if(!map_size(m_dialStats)) {
			map_delete(m_dialStats);
			m_dialStats = Map:-1;
		}
	}
	return 1;
}

hook AppCall_OnInCallAccept@(id) {
	new
		number[SIM_NUMBER_SIZE],
		phone;
	AppCall_GetIncomingNumber(id, number);
	phone = ISim_GetPhoneWithNumber(number);
	if(phone != -1 && AppCall_HasDialStats(phone)) {
		map_remove(m_dialStats, phone);
		if(!map_size(m_dialStats)) {
			map_delete(m_dialStats);
			m_dialStats = Map:-1;
		}
	}
	return 1;
}
