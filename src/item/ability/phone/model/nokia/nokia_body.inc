#if defined nokia_body_included
	#endinput
#endif

#define nokia_body_included

static const
	ROW = 4,
	COLUMN = 3,
	Float:BUTTON_START_OFFSET_X = 70.0000, // BUTTON_X - ANCHOR_X
	Float:BUTTON_START_OFFSET_Y = 130.0000, // BUTTON_Y - ANCHOR_Y
	Float:BUTTON_MARGIN_X = 25.0000, // BUTTON_X[next] - BUTTON_X[current]
	Float:BUTTON_MARGIN_Y = 12.0000, // BUTTON_Y[below] - BUTTON_Y[current]
	Float:SCREEN_OFFSET_MIN_X = 67.0000, // SCREEN_MIN_X - ANCHOR_X
	Float:SCREEN_OFFSET_MIN_Y = 30.0000, // SCREEN_MIN_Y - ANCHOR_Y
	Float:SCREEN_OFFSET_MAX_X = 137.0000, // SCREEN_MAX_X - ANCHOR_X
	Float:SCREEN_OFFSET_MAX_Y = 123.0000, // SCREEN_MAX_Y - ANCHOR_Y
	Float:FINGER_SIZE = 10.0000;

// anchor: 470, 230. 1: 540, 360. 2: 565, 360. 3: 540, 372

stock Nokia_HasIndex(index) {
	return (index > - 1 && index < (ROW * COLUMN));
}

stock Nokia_GetButtonOffset(index, &Float:x, &Float:y) {
	if(!Nokia_HasIndex(index)) {
		return 0;
	}

	x = BUTTON_START_OFFSET_X + BUTTON_MARGIN_X * (index % COLUMN);
	y = BUTTON_START_OFFSET_Y + BUTTON_MARGIN_Y * (floatround(float(index) / float(COLUMN), floatround_floor));
	return 1;
}

stock Nokia_GetFingerSize(index, &Float:size) {
	if(!Nokia_HasIndex(index)) {
		return 0;
	}

	size = FINGER_SIZE;
	return 1;
}

stock Nokia_GetScreenOffset(&Float:min_x, &Float:min_y, &Float:max_x, &Float:max_y) {
	min_x = SCREEN_OFFSET_MIN_X;
	min_y = SCREEN_OFFSET_MIN_Y;
	max_x = SCREEN_OFFSET_MAX_X;
	max_y = SCREEN_OFFSET_MAX_Y;
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPM_ValidateIndex(id, &index) {
	if(IPhone_GetModel(id) == Nokia) {
		if(!Nokia_HasIndex(index)) {
			index = -1;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPM_CalculateUp(id, &index) {
	if(IPhone_GetModel(id) == Nokia) {
		if(index == 0) {
			index = -1;
		} else {
			index -= COLUMN;
			if(index < 0) {
				index = 0;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPM_CalculateDown(id, &index) {
	if(IPhone_GetModel(id) == Nokia) {
		if(index == (ROW * COLUMN - 1)) {
			index = -1;
		} else {
			index += COLUMN;
			if(index >= ROW * COLUMN) {
				index = ROW * COLUMN - 1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPM_OnBodyOffsetGet(id, index, &Float:x, &Float:y, &Float:size, &bool:result) {
	if(IPhone_GetModel(id) == Nokia) {
		if(!Nokia_HasIndex(index)) {
			result = false;
		} else {
			Nokia_GetButtonOffset(index, x, y);
			Nokia_GetFingerSize(index, size);
			result = true;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPM_OnScreenOffsetGet(id, &Float:min_x, &Float:min_y, &Float:max_x, &Float:max_y, &result) {
	if(IPhone_GetModel(id) == Nokia) {
		min_x = SCREEN_OFFSET_MIN_X;
		min_y = SCREEN_OFFSET_MIN_Y;
		max_x = SCREEN_OFFSET_MAX_X;
		max_y = SCREEN_OFFSET_MAX_Y;
		result = true;
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}