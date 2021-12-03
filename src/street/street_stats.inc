#if defined street_stats_included
	#endinput
#endif

#define street_stats_included

#define                 MAX_STREET_NAME_LENGTH                      22

enum StreetStatsType {
    strTypeAve,
    strTypeST
}

enum _:StreetStats {
    streetID,
    streetNumber,
    StreetStatsType:streetType,
    streetName[MAX_STREET_NAME_LENGTH]
}

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    return 1;
}