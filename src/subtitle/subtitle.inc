#if defined subtitle_included
	#endinput
#endif

#define subtitle_included

enum _:SubtitleType {
	SubtitleText,
	SubtitleSelect
}

#include "./subtitle_text.inc"
#include "./subtitle_select.inc"

static Map:m_subType;

forward Subtitle_OnPreShown(playerid, type);
public Subtitle_OnPreShown(playerid, type) {
	if(!map_valid(m_subType)) {
		m_subType = map_new();
	} else if(map_has_key(m_subType, playerid)) {
		switch(map_get(m_subType, playerid)) {
			case SubtitleText: {
				SubtitleText_Hide(playerid);
			}
			case SubtitleSelect: {
				SubtitleSelect_Hide(playerid);
			}
		}
	}
	map_set(m_subType, playerid, type);
	return 1;
}

forward Subtitle_OnHided(playerid);
public Subtitle_OnHided(playerid) {
	if(map_valid(m_subType) && map_has_key(m_subType, playerid)) {
		map_remove(m_subType, playerid);
		if(!map_size(m_subType)) {
			map_delete(m_subType);
			m_subType = Map:-1;
		}
		return 1;
	}
	return 0;
}

#include <YSI_Coding\y_hooks>

hook OnPlayerDisconnect(playerid, reason) {
	if(map_valid(m_subType) && map_has_key(m_subType, playerid)) {
		switch(map_get(m_subType, playerid)) {
			case SubtitleText: {
				SubtitleText_Hide(playerid);
			}
			case SubtitleSelect: {
				SubtitleSelect_Hide(playerid);
			}
		}
	}
	return 1;
}