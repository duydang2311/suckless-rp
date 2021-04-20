#if defined phone_interface_included
	#endinput
#endif

#define phone_interface_included

enum _:InterfaceStats {
	bool:ifaceOn,
	Float:ifaceX,
	Float:ifaceY
}

static Map:m_interfaceStats;

stock iPhone_InitInterface(id, Float:x, Float:y) {
	if(map_valid(m_interfaceStats) && map_has_key(m_interfaceStats, id)) {
		iPhone_TerminateInterface(id);
	}
	if(!map_valid(m_interfaceStats)) {
		m_interfaceStats = map_new();
	}

	new stats[m_interfaceStats];
	stats[ifaceOn] = false;
	stats[ifaceX] = x;
	stats[ifaceY] = y;
	CallLocalFunction("iPhone_OnInterfaceInit", "i", id);
	map_set_arr(m_interfaceStats, id, stats);
	return 1;
}

stock iPhone_ShowInterface(id) {

}

stock iPhone_TerminateInterface() {

}

stock iPhone_GetInterfacePos(id, &Float:x, &Float:y) {
	if(!map_valid(m_interfaceStats) || !map_has_key(m_interfaceStats, id)) {
		return 0;
	}

	new stats[InterfaceStats];
	map_get_arr(m_interfaceStats, id, stats);
	x = stats[ifaceX];
	y = stats[ifaceY];
	return 1;
}