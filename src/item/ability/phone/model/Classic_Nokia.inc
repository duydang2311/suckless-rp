#if defined Classic_Nokia_included
	#endinput
#endif

#define Classic_Nokia_included

static const PhysicalButtonStats[][3] = {
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