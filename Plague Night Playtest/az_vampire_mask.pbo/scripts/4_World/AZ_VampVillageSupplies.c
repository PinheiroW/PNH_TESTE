


class AZ_VampVillageSupplies
{
	static const int PANTRY_COUNT = 3;
	static const int ITEMS_PER_PANTRY = 4;
	static const float BUILDING_RADIUS = 2.0;
	static const float RECOVER_RADIUS = 1.5;

	static bool s_StartupHandled;
	static ref array<EntityAI> s_Pantries = new array<EntityAI>;
	static ref array<bool> s_FillAttempted = new array<bool>;

	static void EnsureMemory()
	{
		while (s_Pantries.Count() < PANTRY_COUNT)
			s_Pantries.Insert(null);
		while (s_FillAttempted.Count() < PANTRY_COUNT)
			s_FillAttempted.Insert(false);
	}

	static vector BuildingAnchor(int slot)
	{
		if (slot == 0)
			return Vector(4485.588379, 17.736679, 10999.889648);
		if (slot == 1)
			return Vector(9040.535156, 11.685209, 10143.749023);
		if (slot == 2)
			return Vector(8123.648926, 17.617668, 10822.009766);
		return vector.Zero;
	}

	static string ExpectedBuildingClass(int slot)
	{
		if (slot == 2)
			return "Land_House_1W02";
		return "Land_House_1W04";
	}

	static vector LocalLootPoint(int slot)
	{
		if (slot == 2)
			return Vector(0.304931, -2.350821, -4.574829);
		return Vector(4.441040, -2.648395, -0.633362);
	}

	static Building FindBuilding(int slot)
	{
		array<Object> nearby = new array<Object>;
		GetGame().GetObjectsAtPosition(BuildingAnchor(slot), BUILDING_RADIUS, nearby, null);
		foreach (Object objectAtAnchor : nearby)
		{
			Building building = Building.Cast(objectAtAnchor);
			if (building && building.IsKindOf(ExpectedBuildingClass(slot)))
				return building;
		}
		return null;
	}

	static void ConsolidateDuplicate(EntityAI keep, EntityAI duplicate, vector at, string where)
	{
		if (!keep || !duplicate || keep == duplicate)
			return;
		GameInventory duplicateInventory = duplicate.GetInventory();
		CargoBase cargo;
		if (duplicateInventory)
			cargo = duplicateInventory.GetCargo();
		int moved = 0;
		int overflow = 0;
		while (cargo && cargo.GetItemCount() > 0)
		{
			EntityAI item = cargo.GetItem(0);
			if (!item)
				break;
			if (keep.ServerTakeEntityToCargo(item))
			{
				moved++;
				continue;
			}
			if (!duplicateInventory.DropEntity(InventoryMode.SERVER, duplicate, item))
			{
				Print("[AZ_Vamp] village pantry: " + where + " duplicate cargo could not move or drop; duplicate preserved");
				break;
			}
			item.SetPosition(at + Vector(0.7 + overflow * 0.25, 0, 0.7));
			AZ_VampLootPlace.Settle(item, where + " duplicate overflow");
			overflow++;
		}
		if (duplicateInventory && duplicateInventory.CountInventory() <= 1)
		{
			GetGame().ObjectDelete(duplicate);
			Print("[AZ_Vamp] village pantry: " + where + " consolidated duplicate (" + moved + " moved, " + overflow + " overflow)");
		}
	}

	static EntityAI Recover(int slot, vector at)
	{
		array<Object> nearby = new array<Object>;
		GetGame().GetObjectsAtPosition(at, RECOVER_RADIUS, nearby, null);
		EntityAI keep;
		float keepDistance = RECOVER_RADIUS + 1;
		foreach (Object objectAtSlot : nearby)
		{
			EntityAI candidate = EntityAI.Cast(objectAtSlot);
			if (!candidate || !candidate.IsKindOf("AZ_VampVillagePantry"))
				continue;
			float distance = vector.Distance(candidate.GetPosition(), at);
			if (distance < keepDistance)
			{
				keep = candidate;
				keepDistance = distance;
			}
		}
		if (!keep)
			return null;
		string where = "village pantry " + (slot + 1);
		foreach (Object duplicateObject : nearby)
		{
			EntityAI duplicate = EntityAI.Cast(duplicateObject);
			if (duplicate && duplicate != keep && duplicate.IsKindOf("AZ_VampVillagePantry"))
				ConsolidateDuplicate(keep, duplicate, at, where);
		}
		return keep;
	}

	static bool Empty(EntityAI pantry)
	{
		return pantry && pantry.GetInventory() && pantry.GetInventory().CountInventory() <= 1;
	}

	static string ItemFor(int slot, int item)
	{
		if (slot == 0)
		{
			if (item == 0) return "BakedBeansCan";
			if (item == 1) return "TunaCan";
			if (item == 2) return "SodaCan_Kvass";
			return "SodaCan_Fronta";
		}
		if (slot == 1)
		{
			if (item == 0) return "PeachesCan";
			if (item == 1) return "SardinesCan";
			if (item == 2) return "SodaCan_Pipsi";
			return "SodaCan_Spite";
		}
		if (item == 0) return "SpaghettiCan";
		if (item == 1) return "UnknownFoodCan";
		if (item == 2) return "SodaCan_Cola";
		return "SodaCan_Fronta";
	}


	static bool Fill(EntityAI pantry, int slot)
	{
		if (!pantry || !pantry.GetInventory())
			return false;
		array<EntityAI> made = new array<EntityAI>;
		for (int item = 0; item < ITEMS_PER_PANTRY; item++)
		{
			EntityAI created = pantry.GetInventory().CreateInInventory(ItemFor(slot, item));
			if (!created)
			{
				foreach (EntityAI rollback : made)
					GetGame().ObjectDelete(rollback);
				return false;
			}
			made.Insert(created);
		}
		return true;
	}

	static void Startup()
	{
		if (s_StartupHandled || !GetGame().IsServer())
			return;
		s_StartupHandled = true;
		EnsureMemory();
		for (int slot = 0; slot < PANTRY_COUNT; slot++)
		{
			Building building = FindBuilding(slot);
			if (!building)
			{
				Print("[AZ_Vamp] village pantry: building " + ExpectedBuildingClass(slot) + " missing at " + BuildingAnchor(slot));
				continue;
			}
			vector at = building.ModelToWorld(LocalLootPoint(slot));
			string where = "village pantry " + (slot + 1);
			EntityAI pantry = Recover(slot, at);
			if (!pantry)
			{
				pantry = EntityAI.Cast(GetGame().CreateObjectEx("AZ_VampVillagePantry", at, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_DYNAMIC_PERSISTENCY));
				if (!pantry)
				{
					Print("[AZ_Vamp] village pantry: " + where + " failed to spawn at " + at);
					continue;
				}
			}
			vector buildingOrientation = building.GetOrientation();
			pantry.SetOrientation(Vector(buildingOrientation[0], 0, 0));
			pantry.SetPosition(at);
			AZ_VampLootPlace.Settle(pantry, where);
			s_Pantries[slot] = pantry;
			if (!s_FillAttempted[slot] && Empty(pantry))
			{
				s_FillAttempted[slot] = true;
				Print("[AZ_Vamp] village pantry: " + where + " empty startup fill " + Fill(pantry, slot) + " at " + pantry.GetPosition());
			}
			else if (!Empty(pantry))
				Print("[AZ_Vamp] village pantry: " + where + " preserves " + (pantry.GetInventory().CountInventory() - 1) + " cargo entities");
		}
	}

	static void ServerReset()
	{
		s_StartupHandled = false;
		s_Pantries.Clear();
		s_FillAttempted.Clear();
	}
}
