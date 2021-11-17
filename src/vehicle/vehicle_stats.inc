#if defined vehicle_stats_included
	#endinput
#endif

#define vehicle_stats_included


const MAX_VEHICLES_DATA_LIST = 184;
const MAX_VEHICLE_MODEL_NAME = 22;

enum VehicleType {
	SportsCars = 1,
	SUV,
	Lowriders,
	Tuners,
	Vans,
	Coupes,
	Sedans,
	Trains,
	PublicService,
	Novelty,
	Government,
	Aircraft,
	Motorcycles,
	RemoteControl,
	Miscellaneous,
	Trailers,
	Industrial,
	Boats
};

static enum _:E_VEHICLE_DATA {
	vehicleModelID,
	VehicleType:vehicleType,
	vehicleName[MAX_VEHICLE_MODEL_NAME],
	vehicleSeat
};
/*
stock Vehicle_GetTypeName(VehicleType:type, name[], size = sizeof(name)) {
	new bool:check = false;
	name[0] = EOS;
	for(new i = 0; i < sizeof(VehicleModel); i++) {
		if(VehicleModel[i][vehicleType] == type) {
			check = true;
			break;
		}
	}

	if(check == true) {
		switch(type) {
			case SportsCars: strcat(name, "Sports Cars", size);
			case SUV: strcat(name, "SUVs & Pickup Trucks", size); 
			case Lowriders: strcat(name, "Lowriders", size);
			case Tuners: strcat(name, "Tuners", size);
			case Vans: strcat(name, "Vans", size);
			case Industrial: strcat(name, "Industrial", size);
			case Coupes: strcat(name, "Coupes & Hatchbacks", size);
			case Sedans: strcat(name, "Sedans & Station Wagons", size);
			case Trains: strcat(name, "Trains", size);
			case PublicService: strcat(name, "Public Services", size);
			case Novelty: strcat(name, "Novelty", size);
			case Government: strcat(name, "Government", size);
			case Aircraft: strcat(name, "Aircraft", size);
			case Boats: strcat(name, "Boats", size);
			case Motorcycles: strcat(name, "Motorcycles & Bikes", size);
			case RemoteControl: strcat(name, "Remote Control", size);
			case Miscellaneous: strcat(name, "Miscellaneous", size);
			case Trailers: strcat(name, "Trailers", size);
			default: strcat(name, "Error!", size);
		}
	}

	return 0;
}
*/

hook OnGameModeInit() {
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS vehicle_stats (\
	model SMALLINT UNSIGNED PRIMARY KEY,\
	type TINYINT UNSIGNED DEFAULT 0,\
	name VARCHAR(22) DEFAULT '',\
	seats TINYINT DEFAULT 2)", false);	

	new rows = 0, Cache:result = mysql_query(MySQL_Connection(), "SELECT * FROM `vehicle_stats`");
	cache_get_row_count(rows);
	if(rows == 0) {
		new query[4100];
		mysql_format(MySQL_Connection(), query, sizeof(query), 
			"INSERT INTO `vehicle_stats` VALUES\
			(402, 1, 'Buffalo', 2), (411, 1, 'Infernus', 2), (415, 1, 'Cheetah', 2), (429, 1, 'Banshee', 2), (434, 1, 'Hotknife', 2), (451, 1, 'Turismo', 2), (477, 1, 'ZR-350', 2), (480, 1, 'Comet', 2), (506, 1, 'Super GT', 2), (541, 1, 'Bullet', 2), (555, 1, 'Windsor', 2), (587, 1, 'Euros', 2), (602, 1, 'Alpha', 2), (603, 1, 'Phoenix', 2),\
			(400, 2, 'Landstalker', 4), (422, 2, 'Bobcat', 2), (444, 2, 'Monster', 2), (470, 2, 'Patriot', 4), (478, 2, 'Walton', 2), (489, 2, 'Rancher', 2), (495, 2, 'Sandking', 2), (500, 2, 'Mesa', 2), (543, 2, 'Salder', 2), (554, 2, 'Yosemite', 2), (579, 2, 'Huntley', 4), (600, 2, 'Picador', 2),\
			(536, 3, 'Blade', 2), (575, 3, 'Broadway', 2), (412, 3, 'Voodoo', 2), (534, 3, 'Remington', 2), (535, 3, 'Slamvan', 2), (566, 3, 'Tahoma', 4), (567, 3, 'Savanna', 4), (576, 3, 'Tornado', 2),\
			(558, 4, 'Uranus', 2), (559, 4, 'Jester', 2), (560, 4, 'Sultan', 4), (561, 4, 'Stratum', 4), (562, 4, 'Elegy', 2), (565, 4, 'Flash', 2),\
			(413, 5, 'Pony', 4), (418, 5, 'Moonbeam', 4), (423, 5, 'Mr. Whoopee', 4), (428, 5, 'Securicar', 4), (440, 5, 'Rumpo', 4), (459, 5, 'Berkleys RC', 4), (482, 5, 'Burrito', 4), (582, 5, 'Newsvan', 4), (588, 5, 'Hotdog', 4),\
			(401, 6, 'Bravura', 2), (410, 6, 'Manana', 2), (419, 6, 'Esperanto', 2), (436, 6, 'Previon', 2), (439, 6, 'Stallion', 2), (474, 6, 'Hermes', 2), (475, 6, 'Sabre', 2), (491, 6, 'Virgo', 2), (496, 6, 'Blista Compact', 2), (517, 6, 'Majestic', 2), (518, 6, 'Buccaneer', 2), (526, 6, 'Fortune', 2), (527, 6, 'Cadrona', 2), (533, 6, 'Feltzer', 2), (542, 6, 'Clover', 2), (545, 6, 'Hustler', 2), (549, 6, 'Tampa', 2), (589, 6, 'Club', 2),\
			(404, 7, 'Perennial', 4), (405, 7, 'Sentinel', 4), (409, 7, 'Stretch', 4), (421, 7, 'Washington', 4), (426, 7, 'Premier', 4), (442, 7, 'Romero', 4), (445, 7, 'Admiral', 4), (458, 7, 'Solair', 4), (466, 7, 'Glendale', 4), (467, 7, 'Oceanic', 4), (479, 7, 'Regina', 4), (492, 7, 'Greenwood', 4), (507, 7, 'Elegant', 4), (516, 7, 'Nebula', 4), (529, 7, 'Williard', 4), (540, 7, 'Vincent', 4), (546, 7, 'Intruder', 4), (547, 7, 'Primo', 4), (550, 7, 'Sunrise', 4), (551, 7, 'Merit', 4), (580, 7, 'Stafford', 4), (585, 7, 'Emperor', 4),\
			(449, 8, 'Tram', 2), (537, 8, 'Freight', 2), (538, 8, 'Brown Streak', 6),\
			(408, 9, 'Trashmaster', 2), (420, 9, 'Taxi', 4), (431, 9, 'Bus', 8), (437, 9, 'Coach', 8), (438, 9, 'Cabbie', 4), (485, 9, 'Baggage', 2), (525, 9, 'Tow Truck', 2), (574, 9, 'Sweeper', 2), (583, 9, 'Tug', 1),\
			(424, 10, 'BF Injection', 2), (457, 10, 'Caddy', 2), (471, 10, 'Quad', 1), (483, 10, 'Camper', 4), (508, 10, 'Journey', 4), (539, 10, 'Vortex', 1), (568, 10, 'Bandito', 1), (571, 10, 'Kart', 1), (572, 10, 'Mower', 1), (573, 10, 'Dune', 2),\
			(416, 11, 'Ambulance', 4), (433, 10, 'Barracks', 4), (427, 10, 'Enforcer', 4), (528, 10, 'FBI Truck', 4), (523, 10, 'HPV-1000', 1), (596, 10, 'Police Car (LSPD)', 4), (597, 10, 'Police Car (SFFD)', 4), (598, 10, 'Police Car (LVPD)', 4), (599, 10, 'Police Ranger', 4), (432, 10, 'Rhino', 4), (601, 10, 'SWAT', 4),\
			(417, 12, 'Leviathan', 4), (425, 12, 'Hunter', 1), (447, 12, 'Seasparrow', 1), (460, 12, 'Skimmer', 1), (476, 12, 'Rustler', 1), (487, 12, 'Maverick', 4), (497, 12, 'Police Maverick', 4), (511, 12, 'Beagle', 2), (512, 12, 'Cropduster', 1), (519, 12, 'Shamal', 2), (520, 12, 'Hydra', 1), (548, 12, 'Cargobob', 4), (553, 12, 'Nevada', 8), (563, 12, 'Raindance', 4), (592, 12, 'Andromada', 8), (593, 12, 'Dodo', 1),\
			(448, 13, 'Pizza Boy', 1), (461, 13, 'PCJ-600', 2), (462, 13, 'Faggio', 2), (463, 13, 'Freeway', 2), (468, 13, 'Sanchez', 2), (481, 13, 'BMX', 1), (509, 13, 'Bike', 1), (510, 13, 'Mountain Bike', 1), (521, 13, 'FCR-900', 2), (522, 13, 'NRG-500', 2), (581, 13, 'BF-400', 2), (586, 13, 'Wayfarer', 1),\
			(441, 14, 'RC Bandit', 1), (464, 14, 'RC Baron', 1), (465, 14, 'RC Raider', 1), (501, 14, 'RC Goblin', 1),\
			(504, 15, 'Bloodring Banger', 4), (494, 15, 'Hotring Racer', 4), (594, 15, 'RC Cam', 1),\
			(435, 16, 'Articalated Trailer', 0), (610, 16, 'Farm Trailer', 0), (584, 16, 'Petrol Trailer', 0)");
		mysql_tquery(MySQL_Connection(), query);
		
		// above, a lot of lines in format that can't read all of thems, i need to create another one.
		mysql_format(MySQL_Connection(), query, sizeof(query), 
			"INSERT INTO `vehicle_stats` VALUES (499, 17, 'Benson', 2), (609, 17, 'Boxville', 2), (524, 17, 'Cement Truck', 2), (532, 17, 'Harvester', 2), (578, 17, 'DFT-30', 2), (406, 17, 'Dumper', 2), (455, 17, 'Flatbed', 2),  (530, 17, 'Forklift', 1), (403, 17, 'Linerunner', 2), (414, 17, 'Mule', 2), (443, 17, 'Packer', 2), (515, 17, 'Roadtrain', 2), (514, 17, 'Tanker', 2), (456, 17, 'Yanke', 2),\
			(472, 18, 'Coastguard', 4), (473, 18, 'Dinghy', 4), (493, 18, 'Jetmax', 4), (595, 18, 'Launch', 4), (484, 18, 'Marquis', 4), (430, 18, 'Predator', 4), (453, 18, 'Reefer', 4), (452, 18, 'Speeder', 4), (446, 18, 'Squallo', 4), (454, 18, 'Tropic', 4)");
		mysql_tquery(MySQL_Connection(), query);
	}

	result = mysql_query(MySQL_Connection(), "SELECT * FROM `vehicle_stats`");
	cache_get_row_count(rows);
	printf("Loaded %d/%d rows of vehicle_stats", rows, MAX_VEHICLES_DATA_LIST);
	cache_delete(result);
	return 1;
}