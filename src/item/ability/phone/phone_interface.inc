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

stock IPhone_InitInterface(id, Float:x, Float:y) {
	if(map_valid(m_interfaceStats) && map_has_key(m_interfaceStats, id)) {
		IPhone_TerminateInterface(id);
	}
	if(!map_valid(m_interfaceStats)) {
		m_interfaceStats = map_new();
	}

	new stats[m_interfaceStats];
	stats[ifaceOn] = false;
	stats[ifaceX] = x;
	stats[ifaceY] = y;
	CallLocalFunction("IPhone_OnInterfaceInit", "i", id);
	map_set_arr(m_interfaceStats, id, stats);
	return 1;
}

stock IPhone_ShowInterface(id) {

}

stock IPhone_TerminateInterface() {

}

stock IPhone_GetInterfacePos(id, &Float:x, &Float:y) {
	if(!map_valid(m_interfaceStats) || !map_has_key(m_interfaceStats, id)) {
		return 0;
	}

	new stats[InterfaceStats];
	map_get_arr(m_interfaceStats, id, stats);
	x = stats[ifaceX];
	y = stats[ifaceY];
	return 1;
}