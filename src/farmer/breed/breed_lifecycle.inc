#if defined breed_lifecycle_included
	#endinput
#endif

#define breed_lifecycle_included

static Float:breedAreaPoints[] = {
	8.8366, 37.1001,
	18.7342, 66.6058,
	35.2461, 60.5792,
	81.0216, 26.0616,
	77.2413, -15.5266,
	71.1856, -48.4400,
	40.2060, -7.5538,
	22.7715, 11.1557
};

static Float:random_float(Float:min,Float:max,accuracy = 4){
	if(min >= max) return 0.0;
	if(min < 0.0 || max < 0.0) return 0.0;
	if(accuracy < 1 || accuracy > 6) accuracy = 4;
	new divValue = floatround(floatpower(10.0,accuracy));
	return random(floatround(max)-floatround(min))+min+(random(divValue)/divValue);
}

static IsPointInPolygon(Float:x,Float:y,const Float:points[],maxpoints = sizeof(points)){
	new polygon_Sides = ((maxpoints - 2) / 2);
	if((maxpoints - 2) & 0b1 || polygon_Sides < 3) return 0;
	new Float:polygon_Data[2][256], cross_Total;
	for(new i = 0, j; i < maxpoints; i += 2, j++){
		polygon_Data[0][j] = points[i];
		polygon_Data[1][j] = points[i+1];
	}
	for(new i, j = polygon_Sides - 1; i < polygon_Sides; j = i, i++){
		if(polygon_Data[1][i] < y && polygon_Data[1][j] >= y || polygon_Data[1][j] < y && polygon_Data[1][i] >= y){
			if(polygon_Data[0][i] + (y - polygon_Data[1][i]) / (polygon_Data[1][j] - polygon_Data[1][i]) * (polygon_Data[0][j] - polygon_Data[0][i]) < x){
				cross_Total++;
			}
		}
	}
	return (cross_Total & 0x1);
}

static RandomPointInRectangle(Float:minx,Float:miny,Float:maxx,Float:maxy,&Float:tx,&Float:ty){
	tx = random_float(0.0,floatsqroot(floatpower(minx-maxx,2)),6)+minx;
	ty = random_float(0.0,floatsqroot(floatpower(miny-maxy,2)),6)+miny;
}

static RandomPointInPolygon(const Float:points[],&Float:tx,&Float:ty,maxpoints = sizeof(points)){
	new Float:minx, Float:miny, Float:maxx, Float:maxy, recheck = 1;
	minx = maxx = points[0];
	miny = maxy = points[1];
	for(new i = 2; i < maxpoints; i += 2){
		if(points[i] < minx)	minx = points[i];
		if(points[i] > maxx)	maxx = points[i];
		if(points[i+1] < miny) 	miny = points[i+1];
		if(points[i+1] > maxy)	maxy = points[i+1];
	}
	RandomPointInRectangle(minx,miny,maxx,maxy,tx,ty);
	while(!IsPointInPolygon(tx,ty,points,maxpoints)){
		RandomPointInRectangle(minx,miny,maxx,maxy,tx,ty);
		recheck++;
	}
	return recheck;
}

forward Breed_Move(id);
public Breed_Move(id) {
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}
	new
		Float:x,
		Float:y,
		Float:z;
	RandomPointInPolygon(breedAreaPoints, x, y);
	CA_FindZ_For2DCoord(x, y, z);
	FCNPC_GoTo(npcid, x, y, z, .speed = FCNPC_MOVE_SPEED_WALK);
	return 1;
}

stock Breed_LifeCycle(id) {
	new npcid = Breed_GetNPCId(id);
	if(npcid == -1) {
		return 0;
	}
	Breed_Move(id);
	return 1;
}

public FCNPC_OnReachDestination(npcid) {
	new id = Breed_GetIdByNPCId(npcid);
	if(id != -1) {
		SetPreciseTimer("Breed_Move", 1000, false, "i", id);
	}
	return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	CA_Init();
	return 1;
}