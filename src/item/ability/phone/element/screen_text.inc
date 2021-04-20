#if defined screen_text_included
	#endinput
#endif

#define screen_text_included

enum _:ScreenTextStats {
	Float:scTextX,
	Float:scTextY
}

enum _:ScreenBoxStats {
	Float:scBoxX,
	Float:scBoxY
}

static Map:m_phBtnStats;

hook iPhone_OnInterfaceInit(id) {
	switch(iPhone_GetModel(id)) {
		case Classic_Nokia: {
			new
				Float:x,
				Float:y;
			if(iPhone_GetInterfacePos(id, x, y)) {
				new
					Float:base_x = x + 50.0,
					Float:base_y = y + 50.0,
					Float:off_x = 10.0,
					Float:off_y = 10.0,
					LinkedList:ll;
				if(!map_valid(m_phBtnStats)) {
					ll = linked_list_new();
					m_phBtnStats = map_new();
					map_set(m_phBtnStats, id, _:ll);
				} else {
					ll = LinkedList:map_get(m_phBtnStats, id);
				}
				// TODO: init all button stats and add every of it to ll.
			}
		}
	}
	return 1;
}