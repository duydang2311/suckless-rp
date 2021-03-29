#if defined brown_cow_included
	#endinput
#endif

#define brown_cow_included

static Map:m_quality;

forward BrownCow_OnCreated(id);
public BrownCow_OnCreated(id) {
	if(!map_valid(m_quality)) {
		m_quality = map_new();
	}
	map_set(m_quality, id, 1.0);
	return 1;
}

forward BrownCow_OnRemoved(id);
public BrownCow_OnRemoved(id) {
	if(map_valid(m_quality) && map_has_key(m_quality, id)) {
		map_remove(m_quality, id);
		if(!map_size(m_quality)) {
			map_delete(m_quality);
			m_quality = Map:-1;
		}
	}
	return 1;
}

stock Float:BrownCow_GetQuality(id) {
	if(!map_valid(m_quality) || !map_has_key(m_quality, id)) {
		return 0.0;
	}
	return Float:map_get(m_quality, id);
}

stock BrownCow_UpdateQuality(id, Float:value) {
	if(!map_valid(m_quality) || !map_has_key(m_quality, id)) {
		return 0;
	}
	new Float:quality = Float:map_get(m_quality, id) + value;
	if(quality > 1.0) {
		quality = 1.0;
	} else if(quality < 0.0) {
		quality = 0.0;
	}
	map_set(m_quality, id, quality);
	return 1;
}