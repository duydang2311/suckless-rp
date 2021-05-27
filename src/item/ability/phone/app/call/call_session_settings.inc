#if defined call_session_settings_included
	#endinput
#endif

#define call_session_settings_included

static enum _:CallSessionSettings {
	bool:Speaker
}

static Map:m_settings; // { phone_id: {CallSessionSettings} }

stock AppCall_HasSettings(id) {
	return (map_valid(m_settings) && map_has_key(m_settings, id));
}

stock bool:AppCall_GetSpeakerSetting(id) {
	if(!AppCall_HasSettings(id)) {
		return false;
	}
	new settings[CallSessionSettings];
	map_get_arr(m_settings, id, settings);
	return settings[Speaker];
}

stock AppCall_SetSpeakerSetting(id, bool:value) {
	if(!map_valid(m_settings)) {
		m_settings = map_new();
	}
	if(!map_has_key(m_settings, id)) {
		new settings[CallSessionSettings];
		settings[Speaker] = false;
		map_set_arr(m_settings, id, settings);
	}
	map_set_cell(m_settings, id, Speaker, value);
	return 1;
}

stock AppCall_DeleteSettings(id) {
	if(!AppCall_HasSettings(id)) {
		return 0;
	}
	map_remove(m_settings, id);
	if(!map_size(m_settings)) {
		map_delete(m_settings);
		m_settings = Map:-1;
	}
	return 1;
}