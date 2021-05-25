#if defined phone_system_error_included
	#endinput
#endif

#define phone_system_error_included

const
	ERROR_CALLBACK_NAME_SIZE = 32,
	ERROR_TEXT_SIZE = 128;

enum _:PhoneErrorStats {
	phErrorCallback[ERROR_CALLBACK_NAME_SIZE],
	phErrorText[ERROR_TEXT_SIZE],
	phErrorExtra
}

stock IPError_Has(id) {
	return (map_valid(m_errorStats) && map_has_key(m_errorStats, id));
}

stock IPError_New(id, const callback[], const text[]) {
	if(!map_valid(m_errorStats)) {
		m_errorStats = map_new();
	}

	new stats[PhoneErrorStats];
	strcat(stats[phErrorCallback], callback);
	strcat(stats[phErrorText], text);
	map_set_arr(m_errorStats, id, stats);
	return 1;
}

stock IPError_GetText(id, text[], size = sizeof(text)) {
	if(!IPError_Has(id)) {
		return 0;
	}

	new stast[PhoneErrorStats];
	map_get_arr(m_errorStats, id, stats);
	text[0] = EOS;
	strcat(text, stats[phErrorText]);
	return 1;
}