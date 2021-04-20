#if defined nokia_body_included
	#endinput
#endif

#define nokia_body_included

static const
	ROW = 4,
	COLUMN = 3,
	Float:ANCHOR_START_OFFSET_X = 70.0000,
	Float:ANCHOR_START_OFFSET_Y = 130.0000,
	Float:BUTTON_OFFSET_X = 15.0000,
	Float:BUTTON_OFFSET_Y = 12.0000,
	Float:FINGER_SIZE = 10.0000;

// anchor: 470, 230. 1: 540, 360. 2: 565, 360. 3: 540, 372

stock Nokia_HasIndex(index) {
	return (index > - 1 && index < (ROW * COLUMN));
}

stock Nokia_GetOffset(index, &Float:x, &Float:y) {
	if(!Nokia_HasIndex(index)) {
		return 0;
	}

	x = ANCHOR_START_OFFSET_X + BUTTON_OFFSET_X * (index % COLUMN);
	y = ANCHOR_START_OFFSET_Y + BUTTON_OFFSET_Y * (floatround(float(index) / float(COLUMN), floatround_ceil));
	return 1;
}

stock Nokia_GetSize(index, &Float:size) {
	if(!Nokia_HasIndex(index)) {
		return 0;
	}

	size = FINGER_SIZE;
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPB_ValidateIndex(model, &index) {
	if(model == Nokia) {
		if(!Nokia_HasIndex(index)) {
			index = -1;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPB_CalculateUp(model, &index) {
	if(model == Nokia) {
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

hook IPB_CalculateDown(model, &index) {
	if(model == Nokia) {
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

hook IPB_GetButtonStats(model, index, &Float:x, &Float:y, &Float:size, &bool:result) {
	if(model == Nokia) {
		if(!Nokia_HasIndex(index)) {
			result = false;
		} else {
			Nokia_GetOffset(index, x, y);
			Nokia_GetSize(index, size);
			result = true;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}