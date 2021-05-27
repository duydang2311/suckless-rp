#if defined util_included
	#endinput
#endif

#define util_included

#include <a_samp>
#include <YSI_Data\y_iterate>

stock Util_GetPointInFront2D(Float:x,Float:y,Float:rz,Float:radius,&Float:tx,&Float:ty) {
	tx = x - (radius * floatsin(rz,degrees));
	ty = y + (radius * floatcos(rz,degrees));
	return 1;
}

stock Util_GetClosestVehicle(Float:x, Float:y, Float:z, Float:range) {
    new
		Float:max_dist = range + 0.001,
        Float:dist,
        vehicle_id = -1;
    foreach(new i : Vehicle) {
        if((dist = GetVehicleDistanceFromPoint(i, x, y, z)) < max_dist) {
            max_dist = dist;
            vehicle_id = i;
        }
    }
    return vehicle_id;
}

stock Util_Clamp(&value, min, max) {
    if(value < min) {
        value = min;
    } else if(value > max) {
        value = max;
    }
    return 1;
}

stock Util_FloatClamp(&Float:value, Float:min, Float:max) {
    if(value < min) {
        value = min;
    } else if(value > max) {
        value = max;
    }
    return 1;
}

stock Util_RGBA(color, &rr, &gg, &bb, &aa) {
	rr = color >>> 24;
	gg = (color >>> 16) & 0xFF;
	bb = (color >>> 8) & 0xFF;
	aa = color & 0xFF;
}

stock Util_Hex(&color, rr, gg, bb, aa) {
	color = (aa & 0xFF) | ((bb & 0xFF) << 8) | ((gg & 0xFF) << 16) | (rr << 24);
}
