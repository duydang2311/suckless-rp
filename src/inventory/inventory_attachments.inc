#if defined inventory_attachments_included
	#endinput
#endif

#define inventory_attachments_included

enum AttachmentStats {
	attBone,
	Float:attOffset[3],
	Float:attRot[3],
	Float:attScale[3]
}

enum _:InventoryAttachmentStats {
	attItemId,
	attSlot
}

static Map:m_inventoryAttachment;

stock Task:Inventory_AttachItem(playerid, slot, item_id) {
	new Task:t = task_new();
	task_detach();
	inline const QueryFinished() {
		if(cache_num_rows()) {
			new
				att[AttachmentStats],
				stats[InventoryAttachmentStats],
				LinkedList:ll,
				query[128];
			if(!map_has_key(m_inventoryAttachment, playerid)) {
				ll = linked_list_new();
				map_set(m_inventoryAttachment, playerid, ll);
			} else {
				ll = LinkedList:map_get(m_inventoryAttachment, playerid);
			}
			cache_get_value_name_int(0, "bone", att[attBone]);
			cache_get_value_name_float(0, "offset_x", att[attOffset][0]);
			cache_get_value_name_float(0, "offset_y", att[attOffset][1]);
			cache_get_value_name_float(0, "offset_z", att[attOffset][2]);
			cache_get_value_name_float(0, "rot_x", att[attRot][0]);
			cache_get_value_name_float(0, "rot_y", att[attRot][1]);
			cache_get_value_name_float(0, "rot_z", att[attRot][2]);
			cache_get_value_name_float(0, "scale_x", att[attScale][0]);
			cache_get_value_name_float(0, "scale_y", att[attScale][1]);
			cache_get_value_name_float(0, "scale_z", att[attScale][2]);
			stats[attSlot] = slot;
			stats[attItemId] = item_id;
			linked_list_add_arr(ll, stats);
			SetPlayerAttachedObject(playerid, slot, Item_GetModel(item_id), att[attBone], att[attOffset][0], att[attOffset][1], att[attOffset][2], att[attRot][0], att[attRot][1], att[attRot][2], att[attScale][0], att[attScale][1], att[attScale][2]);
			mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE inventory_attachments SET attached = TRUE WHERE item_id = %d LIMIT 1", item_id);
			mysql_pquery(MySQL_Connection(), query);
			task_set_result(t, 1);
		} else {
			task_set_result(t, 0);
		}
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM inventory_attachments WHERE item_id = %d LIMIT 1", item_id);
	return t;
}

stock Inventory_DetachItem(playerid, item_id) {
	if(!map_has_key(m_inventoryAttachment, playerid)) {
		return 0;
	}

	new LinkedList:ll = LinkedList:map_get(m_inventoryAttachment, playerid);
	for(new Iter:it = linked_list_iter(ll), stats[InventoryAttachmentStats]; iter_inside(it); iter_move_next(it)) {
		iter_get_arr(it, stats);
		if(stats[attItemId] == item_id) {
			new query[128];
			mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE inventory_attachments SET attached = FALSE WHERE item_id = %d LIMIT 1", item_id);
			mysql_pquery(MySQL_Connection(), query);
			RemovePlayerAttachedObject(playerid, stats[attSlot]);
			iter_erase(it);
			break;
		}
	}
	return 1;
}

stock Inventory_IsAttached(playerid, item_id) {
	if(!map_has_key(m_inventoryAttachment, playerid)) {
		return 0;
	}
	new LinkedList:ll = LinkedList:map_get(m_inventoryAttachment, playerid);
	for(new Iter:it = linked_list_iter(ll), stats[InventoryAttachmentStats]; iter_inside(it); iter_move_next(it)) {
		iter_get_arr(it, stats);
		if(stats[attItemId] == item_id) {
			return 1;
		}
	}
	return 0;
}

stock Task:Inventory_InsertAttachment(item_id, bone) {
	new Task:t = task_new();
	task_detach();
	inline const QueryFinished() {
		task_set_result(t, 1);
	}
	MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "INSERT INTO inventory_attachments(item_id, bone) VALUES (%d, %d) ON DUPLICATE KEY UPDATE bone = VALUES(bone)", item_id, bone);
	return t;
}

stock Inventory_GetAttachSlot(playerid, item_id) {
	if(map_has_key(m_inventoryAttachment, playerid)) {
		new LinkedList:ll = LinkedList:map_get(m_inventoryAttachment, playerid);
		for(new Iter:it = linked_list_iter(ll), stats[InventoryAttachmentStats]; iter_inside(it); iter_move_next(it)) {
			iter_get_arr(it, stats);
			if(stats[attItemId] == item_id) {
				return stats[attSlot];
			}
		}
	}
	return -1;
}

stock Inventory_SetAttachment(item_id, Float:offx, Float:offy, Float:offz, Float:rotx, Float:roty, Float:rotz, Float:scalex, Float:scaley, Float:scalez) {
	new query[384];
	mysql_format(MySQL_Connection(), query, sizeof(query), "UPDATE inventory_attachments SET offset_x = %.4f, offset_y = %.4f, offset_z = %.4f, rot_x = %.4f, rot_y = %.4f, rot_z = %.4f, scale_x = %.4f, scale_y = %.4f, scale_z = %.4f WHERE item_id = %d LIMIT 1", offx, offy, offz, rotx, roty, rotz, scalex, scaley, scalez, item_id);
	mysql_pquery(MySQL_Connection(), query);
	return 1;
}

#include <YSI_Coding\y_hooks>

hook Inventory_OnItemAdded(playerid, item_id) {
	new slot = Player_GetFreeAttachmentSlot(playerid);
	printf("on item added free slot %d", slot);
	if(slot != -1) {
		inline const QueryFinished() {
			if(cache_num_rows()) {
				new
					bool:attached;
				cache_get_value_name_int(0, "attached", attached);
				if(!attached) {
					return 1;
				}
				new
					att[AttachmentStats],
					stats[InventoryAttachmentStats],
					LinkedList:ll;
				if(!map_has_key(m_inventoryAttachment, playerid)) {
					ll = linked_list_new();
					map_set(m_inventoryAttachment, playerid, ll);
				} else {
					ll = LinkedList:map_get(m_inventoryAttachment, playerid);
				}
				cache_get_value_name_int(0, "bone", att[attBone]);
				cache_get_value_name_float(0, "offset_x", att[attOffset][0]);
				cache_get_value_name_float(0, "offset_y", att[attOffset][1]);
				cache_get_value_name_float(0, "offset_z", att[attOffset][2]);
				cache_get_value_name_float(0, "rot_x", att[attRot][0]);
				cache_get_value_name_float(0, "rot_y", att[attRot][1]);
				cache_get_value_name_float(0, "rot_z", att[attRot][2]);
				cache_get_value_name_float(0, "scale_x", att[attScale][0]);
				cache_get_value_name_float(0, "scale_y", att[attScale][1]);
				cache_get_value_name_float(0, "scale_z", att[attScale][2]);
				stats[attSlot] = slot;
				stats[attItemId] = item_id;
				linked_list_add_arr(ll, stats);
				SetPlayerAttachedObject(playerid, slot, Item_GetModel(item_id), att[attBone], att[attOffset][0], att[attOffset][1], att[attOffset][2], att[attRot][0], att[attRot][1], att[attRot][2], att[attScale][0], att[attScale][1], att[attScale][2]);
			}
		}
		MySQL_PQueryInline(MySQL_Connection(), using inline QueryFinished, "SELECT * FROM inventory_attachments WHERE item_id = %d LIMIT 1", item_id);
	}
	return 1;
}

hook Inventory_OnItemRemoved(playerid, item_id) {
	new slot = Inventory_GetAttachSlot(playerid, item_id);
	printf("on item removed slot %d", slot);
	if(slot != -1) {
		RemovePlayerAttachedObject(playerid, slot);
	}
	return 1;
}

hook OnGameModeInit() {
	m_inventoryAttachment = map_new();
	mysql_query(MySQL_Connection(),
	"CREATE TABLE IF NOT EXISTS inventory_attachments (\
	created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
	modified_date TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\
	item_id INT UNSIGNED PRIMARY KEY,\
	attached BOOLEAN DEFAULT FALSE,\
	bone TINYINT UNSIGNED,\
	offset_x FLOAT(11, 4) DEFAULT 0.0000,\
	offset_y FLOAT(11, 4) DEFAULT 0.0000,\
	offset_z FLOAT(11, 4) DEFAULT 0.0000,\
	rot_x FLOAT(11, 4) DEFAULT 0.0000,\
	rot_y FLOAT(11, 4) DEFAULT 0.0000,\
	rot_z FLOAT(11, 4) DEFAULT 0.0000,\
	scale_x FLOAT(11, 4) DEFAULT 1.0000,\
	scale_y FLOAT(11, 4) DEFAULT 1.0000,\
	scale_z FLOAT(11, 4) DEFAULT 1.0000,\
	FOREIGN KEY (item_id) REFERENCES item_stats(id) ON UPDATE CASCADE ON DELETE CASCADE\
	)", false);
	return 1;
}