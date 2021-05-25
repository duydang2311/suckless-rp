#if defined phone_system_dialog_included
	#endinput
#endif

#define phone_system_dialog_included

#define IPDialog_New(%0,%1,%2) _prv_IPDialog_New(%0,#%1,%2)
#define PhoneDialog:%0(%1) \
	forward phDlg_%0(%1); \
	public phDlg_%0(%1)

const
	DIALOG_CALLBACK_SIZE = 32,
	DIALOG_TEXT_SIZE = 128;

enum _:PhoneDialogStats {
	phDialogCallback[DIALOG_CALLBACK_SIZE],
	phDialogText[DIALOG_TEXT_SIZE],
	phDialogExtra
}

static Map:m_dialogStats; // { {Key}: {PhoneDialogStats} }

stock IPDialog_Has(id) {
	return (map_valid(m_dialogStats) && map_has_key(m_dialogStats, id));
}

stock _prv_IPDialog_New(id, const callback[], const text[]) {
	if(!map_valid(m_dialogStats)) {
		m_dialogStats = map_new();
	}

	new stats[PhoneDialogStats];
	strcat(stats[phDialogCallback], "phDlg_");
	strcat(stats[phDialogCallback], callback);
	strcat(stats[phDialogText], text);
	map_set_arr(m_dialogStats, id, stats);
	return 1;
}

stock IPDialog_GetText(id, text[], size = sizeof(text)) {
	if(!IPDialog_Has(id)) {
		return 0;
	}

	new stats[PhoneDialogStats];
	map_get_arr(m_dialogStats, id, stats);
	text[0] = EOS;
	strcat(text, stats[phDialogText], size);
	return 1;
}

stock IPDialog_GetCallback(id, callback[], size = sizeof(callback)) {
	if(!IPDialog_Has(id)) {
		return 0;
	}

	new stats[PhoneDialogStats];
	map_get_arr(m_dialogStats, id, stats);
	callback[0] = EOS;
	strcat(callback, stats[phDialogCallback], size);
	return 1;
}
