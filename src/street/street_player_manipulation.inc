#if defined street_player_manipulation_defined
    #endinput
#endif

#define street_player_manipulation_defined

new Map:m_streetPlayerManipulate;

#include <YSI_Coding\y_hooks>
hook OnGameModeInit@() {
    m_streetPlayerManipulate = map_new();
    return 1;
}