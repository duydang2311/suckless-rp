#if defined Classic_Nokia_included
	#endinput
#endif

#define Classic_Nokia_included

static const
	COLUMN_PER_ROW = 3;

static const Float:PhysicalButtonStats[][3] = {
	// {x, y, radius}
	// x, y are the offsets from textdraw sprite position
	// radius is the radius of finger-pointing textdraw
	// value of each button is referred to its index in this array
	// this const array provides stats for physical button of Classic Nokia's model
	{0.0, 0.0, 2.0}
};

stock ClassicNokia_HasIndex(index) {
	return (index > - 1 && index < sizeof(PhysicalButtonStats));
}

stock ClassicNokia_GetOffset(index, &Float:x, &Float:y) {
	if(!ClassicNokia_HasIndex(index)) {
		return 0;
	}

	x = PhysicalButtonStats[index][0];
	y = PhysicalButtonStats[index][1];
	return 1;
}

stock ClassicNokia_GetRadius(index, &Float:radius) {
	if(!ClassicNokia_HasIndex(index)) {
		return 0;
	}

	radius = PhysicalButtonStats[index][2];
	return 1;
}

#include <YSI_Coding\y_hooks>

hook IPB_ValidateIndex(model, &index) {
	if(model == ClassicNokia) {
		if(!ClassicNokia_HasIndex(index)) {
			index = -1;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IBP_CalculateUp(model, &index) {
	if(model == ClassicNokia) {
		if(index == 0) {
			index = -1;
		} else {
			index -= COLUMN_PER_ROW;
			if(index < 0) {
				index = 0;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IBP_CalculateDown(model, &index) {
	if(model == ClassicNokia) {
		if(index == (sizeof(PhysicalButtonStats) - 1)) {
			index = -1;
		} else {
			index += COLUMN_PER_ROW;
			if(index >= sizeof(PhysicalButtonStats)) {
				index = sizeof(PhysicalButtonStats) - 1;
			}
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}

hook IPB_GetButtonStats(model, index, &Float:x, &Float:y, &Float:radius, &bool:result) {
	if(model == ClassicNokia) {
		if(!ClassicNokia_HasIndex(index)) {
			result = false;
		} else {
			ClassicNokia_GetOffset(index, x, y);
			ClassicNokia_GetRadius(index, radius);
			result = true;
		}
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return 1;
}