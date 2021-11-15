#if defined vehicle_name_included
	#endinput
#endif

#define vehicle_name_included

const MAX_VEHICLE_MODEL_NAME = 22;

enum VehicleType {
	SportsCars,
	SUVS,
	Lowriders,
	Tuners,
	Vans,
	Industrial,
	Coupes,
	Sedans,
	Trains,
	PublicService,
	Novelty,
	Government,
	Aircraft,
	Boats,
	Motorcycles,
	RemoteControl,
	Miscellaneous,
	Trailers
};

static enum _:E_VEHICLE_DATA {
	vehicleModelID,
	VehicleType:vehicleType,
	vehicleName[MAX_VEHICLE_MODEL_NAME],
	vehicleSeat
};

static const VehicleModel[][E_VEHICLE_DATA] = {
	{400, SUVS, "Landstalker", 4}, 
	{401, Coupes, "Bravura", 2}, 
	{402, SportsCars, "Buffalo", 2}, 
	{403, Industrial, "Linerunner", 2}, 
	{404, Sedans, "Perennial", 4}, 
	{405, Sedans, "Sentinel", 4}, 
	{406, Industrial, "Dumper", 2}, 
	{407, Government, "Fire Truck", 4}, 
	{408, PublicService, "Trashmaster", 2}, 
	{409, Sedans, "Stretch", 4}, 
	{410, Coupes,"Manana", 2},
	{411, SportsCars, "Infernus", 2}, 
	{412, Lowriders, "Voodoo", 2}, 
	{413, Vans, "Pony", 4}
};

stock Vehicle_GetName(model, name[], size = sizeof(name)) {
	name[0] = EOS;
	if(model >= 400 && model <= 611) {
		strcat(name, VehicleModel[model - 400][vehicleName], size);
		return 1;
	}
	return 0;
}