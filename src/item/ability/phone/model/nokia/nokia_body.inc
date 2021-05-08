#if defined nokia_body_included
	#endinput
#endif

#define nokia_body_included

static const
	ROW = 4,
	COLUMN = 3,
	Float:WIDTH_HEIGHT_RATIO = 0.5,
	Float:DEFAULT_SIZE_X = 110.0000,
	Float:DEFAULT_SIZE_Y = 220.0000,
	Float:BUTTON_START_OFFSET_X = 70.0000, // BUTTON_X - ANCHOR_X
	Float:BUTTON_START_OFFSET_Y = 130.0000, // BUTTON_Y - ANCHOR_Y
	Float:BUTTON_MARGIN_X = 25.0000, // BUTTON_X[next] - BUTTON_X[current]
	Float:BUTTON_MARGIN_Y = 12.0000, // BUTTON_Y[below] - BUTTON_Y[current]
	Float:SAFE_SCREEN_OFFSET_MIN_X = 16.0000, // SCREEN_MIN_X - ANCHOR_X
	Float:SAFE_SCREEN_OFFSET_MIN_Y = 30.0000, // SCREEN_MIN_Y - ANCHOR_Y
	Float:SAFE_SCREEN_OFFSET_MAX_X = 94.0000, // SCREEN_MAX_X - ANCHOR_X
	Float:SAFE_SCREEN_OFFSET_MAX_Y = 102.0000, // SCREEN_MAX_Y - ANCHOR_Y
	Float:SCREEN_OFFSET_MIN_X = 16.0000, // SCREEN_MIN_X - ANCHOR_X
	Float:SCREEN_OFFSET_MIN_Y = 25.0000, // SCREEN_MIN_Y - ANCHOR_Y
	Float:SCREEN_OFFSET_MAX_X = 94.0000, // SCREEN_MAX_X - ANCHOR_X
	Float:SCREEN_OFFSET_MAX_Y = 107.0000; // SCREEN_MAX_Y - ANCHOR_Y

// anchor: 470, 230. 1: 540, 360. 2: 565, 360. 3: 540, 372

stock Nokia_GetDefaultSize(&Float:x, &Float:y) {
	x = DEFAULT_SIZE_X;
	y = DEFAULT_SIZE_Y;
	return 1;
}

stock Nokia_GetSafeScreenOffset(&Float:min_x, &Float:min_y, &Float:max_x, &Float:max_y) {
	min_x = SAFE_SCREEN_OFFSET_MIN_X;
	min_y = SAFE_SCREEN_OFFSET_MIN_Y;
	max_x = SAFE_SCREEN_OFFSET_MAX_X;
	max_y = SAFE_SCREEN_OFFSET_MAX_Y;
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPM_OnSafeScreenGet(id, &Float:min_x, &Float:min_y, &Float:max_x, &Float:max_y, &result) {
	if(IPhone_GetModel(id) == Nokia) {
		Nokia_GetSafeScreenOffset(min_x, min_y, max_x, max_y);
		result = true;
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}