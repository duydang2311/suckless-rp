#if defined breed_area_included
	#endinput
#endif

#define breed_area_included

static const Float:breedAreaPoints[] = {
	8.8366, 37.1001,
	18.7342, 66.6058,
	35.2461, 60.5792,
	81.0216, 26.0616,
	77.2413, -15.5266,
	71.1856, -48.4400,
	40.2060, -7.5538,
	22.7715, 11.1557
};

static breedArea;

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

stock Breed_GetRandomPointInArea(&Float:x, &Float:y) {
	return RandomPointInPolygon(breedAreaPoints, x, y);
}

stock Breed_IsPointInArea(Float:x, Float:y, Float:z) {
	return IsPointInDynamicArea(breedArea, x, y, z);
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
	breedArea = CreateDynamicPolygon(breedAreaPoints, 0.0, 20.0, .worldid = 0, .interiorid = 0);
	return 1;
}