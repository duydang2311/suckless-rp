#if defined street_stats_included
	#endinput
#endif

#define street_stats_included

static const MAX_STREET_NAME_LENGTH = 22;

enum _:StreetStats {
    streetName[MAX_STREET_NAME_LENGTH]    
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    return 1;
}