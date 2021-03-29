#if defined diary_cow_included
	#endinput
#endif

#define diary_cow_included

static Map:m_milk;

forward DiaryCow_OnCreated(id);
public DiaryCow_OnCreated(id) {
	if(!map_valid(m_milk)) {
		m_milk = map_new();
	}
	map_set(m_milk, id, 0.0);
	return 1;
}

forward DiaryCow_OnRemoved(id);
public DiaryCow_OnRemoved(id) {
	if(map_valid(m_milk) && map_has_key(m_milk, id)) {
		map_remove(m_milk, id);
		if(!map_size(m_milk)) {
			map_delete(m_milk);
			m_milk = Map:-1;
		}
	}
	return 1;
}

stock Float:DiaryCow_GetMilk(id) {
	if(!map_valid(m_milk) || !map_has_key(m_milk, id)) {
		return 0.0;
	}
	return Float:map_get(m_milk, id);
}

stock DiaryCow_UpdateMilk(id, Float:value) {
	if(!map_valid(m_milk) || !map_has_key(m_milk, id)) {
		return 0;
	}
	new Float:milk = Float:map_get(m_milk, id) + value;
	if(milk > 1.0) {
		milk = 1.0;
	} else if(milk < 0.0) {
		milk = 0.0;
	}
	map_set(m_milk, id, milk);
	return 1;
}