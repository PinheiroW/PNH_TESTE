

class AZ_VampHunterSlot
{
 int slot;
 vector position;
 vector churchPosition;
 string churchClass;
 bool placed;
}
class AZ_VampHunterLayout
{
 int version = 1;
 string worldName;
 ref array<ref AZ_VampHunterSlot> slots;
}

class AZ_VampDrop
{
	static const int CACHE_COUNT = 6;
	static const float RECOVER_RADIUS = 1.5;
	// AZ-2605: the chest is in no mission types file, so without this the CE default cleanup (45 s) deletes it.
	static const float LIFETIME = 3888000;

	static const string LAYOUT_FILE = "$profile:AZ_Vampire/hunter_caches.json";
	static ref AZ_VampHunterLayout s_Layout;
	static bool s_LayoutWritable = true;
	static bool s_ChurchSweepDone;
	static int s_LastConfiguredCount = -1;
	static ref array<int> s_LastCheckedAt = new array<int>;
	static bool s_StartupHandled;
	static ref array<EntityAI> s_Caches = new array<EntityAI>;
	static ref array<bool> s_FillAttempted = new array<bool>;

	static void EnsureMemory()
	{
		while (s_Caches.Count() < CACHE_COUNT)
			s_Caches.Insert(null);
		while (s_FillAttempted.Count() < CACHE_COUNT)
			s_FillAttempted.Insert(false);
		while (s_LastCheckedAt.Count() < CACHE_COUNT)
			s_LastCheckedAt.Insert(GetGame().GetTime());
	}

	static vector CachePosition(int slot)
	{
		if (slot == 0)
			return Vector(6888.08, 15.5136, 10977.1);
		if (slot == 1)
			return Vector(5505.54, 116.874, 8143.21);
		if (slot >= 2 && slot < CACHE_COUNT && s_Layout && s_Layout.slots && slot < s_Layout.slots.Count() && s_Layout.slots[slot])
			return s_Layout.slots[slot].position;
		return Vector(0, 0, 0);
	}

	static EntityAI CacheAt(int slot)
	{
		EnsureMemory();
		if (slot < 0 || slot >= CACHE_COUNT)
			return null;
		return s_Caches[slot];
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
				Print("[AZ_Vamp] hunter cache: " + where + " duplicate cargo could not move or drop; duplicate preserved");
				break;
			}
			vector beside = at + Vector(0.7 + overflow * 0.25, 0, 0.7);
			item.SetPosition(beside);
			AZ_VampLootPlace.Settle(item, where + " duplicate overflow");
			overflow++;
		}
		if (duplicateInventory && duplicateInventory.CountInventory() <= 1)
		{
			GetGame().ObjectDelete(duplicate);
			Print("[AZ_Vamp] hunter cache: " + where + " consolidated duplicate (" + moved + " moved, " + overflow + " overflow)");
		}
	}

	static EntityAI RecoverSlot(int slot, vector at)
	{
		array<Object> nearby = new array<Object>;
		GetGame().GetObjectsAtPosition(at, RECOVER_RADIUS, nearby, null);
		EntityAI keep;
		float keepDistance = RECOVER_RADIUS + 1;
		foreach (Object objectAtSlot : nearby)
		{
			EntityAI candidate = EntityAI.Cast(objectAtSlot);
			if (!candidate || !candidate.IsKindOf("AZ_VampHunterChest"))
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
		string where = "church cache " + (slot + 1);
		foreach (Object duplicateObject : nearby)
		{
			EntityAI duplicate = EntityAI.Cast(duplicateObject);
			if (duplicate && duplicate != keep && duplicate.IsKindOf("AZ_VampHunterChest"))
				ConsolidateDuplicate(keep, duplicate, at, where);
		}
		AZ_VampLootPlace.Track(keep, where);
		return keep;
	}

	static void KeepAlive(EntityAI chest)
	{
		if (!chest) return;
		chest.SetLifetimeMax(LIFETIME);
		chest.SetLifetime(LIFETIME);
	}

	static bool Empty(EntityAI chest)
	{
		return chest && chest.GetInventory() && chest.GetInventory().CountInventory() <= 1;
	}

	static bool Put(EntityAI chest, string className)
	{
		if (!chest || !chest.GetInventory())
			return false;
		return chest.GetInventory().CreateInInventory(className) != null;
	}

	static bool Fill(EntityAI chest)
	{
		if (!Empty(chest)) return false;
		string weapon = "Repeater";
		if (Math.RandomInt(0, 2) == 1)
			weapon = "Magnum";
		string practical = "Binoculars";
		if (Math.RandomInt(0, 2) == 1)
			practical = "Canteen";
		bool ok = true;
		ok = Put(chest, weapon) && ok;
		ok = Put(chest, "AZ_VampAmmoBox_357Silver_10Rnd") && ok;
		ok = Put(chest, "AZ_VampAmmoBox_357Silver_10Rnd") && ok;
		ok = Put(chest, "AZ_VampStake") && ok;
		ok = Put(chest, "AZ_VampHolyWater") && ok;
		ok = Put(chest, "HuntingKnife") && ok;
		ok = Put(chest, "BandageDressing") && ok;
		ok = Put(chest, practical) && ok;
		return ok;
	}

	static int ValidCount(int value) { return Math.Clamp(value, 0, CACHE_COUNT); }
	static int ValidRefillMin(int value)
	{
		if (value == 0) return 0;
		return Math.Clamp(value, 10, 240);
	}
	static int ConfiguredCount() { return ValidCount(AZ_VampState.Cfg().hunterCacheCount); }
	static bool PeriodDue(int nowMs, int lastMs, int periodMs)
	{
		return periodMs > 0 && nowMs >= lastMs && nowMs - lastMs >= periodMs;
	}
	static bool ShouldRefill(bool empty, int nowMs, int lastMs, int periodMs)
	{
		return empty && PeriodDue(nowMs, lastMs, periodMs);
	}
	static void SaveLayout()
	{
		if (s_LayoutWritable && s_Layout) JsonFileLoader<AZ_VampHunterLayout>.JsonSaveFile(LAYOUT_FILE, s_Layout);
	}
	static void LoadLayout()
	{
		if (s_Layout) return;
		s_Layout = new AZ_VampHunterLayout;
		string world;GetGame().GetWorldName(world);
		if (FileExist(LAYOUT_FILE))
		{
			string error;
			if (!JsonFileLoader<AZ_VampHunterLayout>.LoadFile(LAYOUT_FILE, s_Layout, error) || s_Layout.version != 1 || s_Layout.worldName != world)
			{
				s_LayoutWritable = false;
				s_Layout = new AZ_VampHunterLayout;
				Print("[AZ_Vamp] hunter cache layout invalid; recover only, no new entities: " + error);
			}
		}
		s_Layout.worldName = world;
		if (!s_Layout.slots) s_Layout.slots = new array<ref AZ_VampHunterSlot>;
		while (s_Layout.slots.Count() < CACHE_COUNT) s_Layout.slots.Insert(null);
		for (int slot = 0; slot < CACHE_COUNT; slot++)
		{
			if (!s_Layout.slots[slot]) s_Layout.slots[slot] = new AZ_VampHunterSlot;
			s_Layout.slots[slot].slot = slot;
			if (slot < 2) s_Layout.slots[slot].position = CachePosition(slot);
		}
	}
	static Object CacheChurch(int slot)
	{
		if (slot < 0 || slot >= CACHE_COUNT || !AZ_VampLandmarks.s_Churches) return null;
		vector at = CachePosition(slot);
		if (at == vector.Zero) return null;
		foreach (Object church : AZ_VampLandmarks.s_Churches)
		{
			if (!church || !Building.Cast(church)) continue;
			if (slot < 2 && vector.Distance(church.GetPosition(), at) < 60) return church;
			if (slot >= 2 && s_Layout && church.GetType() == s_Layout.slots[slot].churchClass && vector.Distance(church.GetPosition(), s_Layout.slots[slot].churchPosition) < 0.1) return church;
		}
		return null;
	}
	static bool ChurchHit(Object church, vector from, vector to, Object ignore, out vector hit)
	{
		RaycastRVParams ray = new RaycastRVParams(from, to, ignore);
		ray.type = ObjIntersectGeom;ray.sorted = true;
		array<ref RaycastRVResult> results = new array<ref RaycastRVResult>;
		if (!DayZPhysics.RaycastRVProxy(ray, results) || results.Count() == 0) return false;
		RaycastRVResult first = results[0];
		if (!first || (first.obj != church && first.parent != church)) return false;
		if (from[1] > to[1] && first.dir[1] < 0.7) return false;
		hit = first.pos;return true;
	}
	static bool InteriorClear(Object church, vector at, Object ignore = null)
	{
		if (!church || !Building.Cast(church)) return false;
		vector floor, roof;
		if (!ChurchHit(church, at + "0 0.5 0", at - "0 1 0", ignore, floor)) return false;
		if (!ChurchHit(church, floor + "0 0.8 0", floor + "0 18 0", ignore, roof) || roof[1] - floor[1] < 1.8) return false;
		for (int corner = 0; corner < 4; corner++)
		{
			float dx = -0.55;float dz = -0.45;
			if (corner >= 2) dx = 0.55;
			if ((corner % 2) == 1) dz = 0.45;
			vector support;
			if (!ChurchHit(church, floor + Vector(dx, 0.5, dz), floor + Vector(dx, -0.4, dz), ignore, support) || Math.AbsFloat(support[1] - floor[1]) > 0.08) return false;
		}
		array<Object> exclude = new array<Object>;
		if (ignore) exclude.Insert(ignore);
		array<Object> collision = new array<Object>;
		return !GetGame().IsBoxCollidingGeometry(floor + "0 0.5 0", church.GetOrientation(), "1.8 0.8 1.6", ObjIntersectGeom, ObjIntersectNone, exclude, collision);
	}
	static bool InteriorPosition(Object church, out vector at)
	{
		vector bounds[2];
		if (!church.GetCollisionBox(bounds)) return false;
		// Stable model-local grid; nearest hit must be the church, never a decoration.
		for (int gx = 0; gx < 5; gx++)
		{
			for (int gz = 0; gz < 5; gz++)
			{
				float x = Math.Lerp(bounds[0][0], bounds[1][0], 0.2 + gx * 0.15);
				float z = Math.Lerp(bounds[0][2], bounds[1][2], 0.2 + gz * 0.15);
				vector probe = church.ModelToWorld(Vector(x, bounds[0][1] + 1, z));
				vector floor;
				if (!ChurchHit(church, probe + "0 1.5 0", probe - "0 1.5 0", null, floor)) continue;
				if (floor[1] > probe[1] + 0.5) continue;
				vector put = floor + "0 0.1 0";
				if (InteriorClear(church, put)) { at = put;return true; }
			}
		}
		return false;
	}
	static bool UsedChurch(Object church)
	{
		vector position = church.GetPosition();
		for (int slot = 0; slot < CACHE_COUNT; slot++)
		{
			vector anchor = CachePosition(slot);
			if (anchor != vector.Zero && vector.Distance(position, anchor) < 60) return true;
		}
		return false;
	}
	static bool StableBefore(vector a, vector b)
	{
		if (a[2] != b[2]) return a[2] < b[2];
		if (a[0] != b[0]) return a[0] < b[0];
		return a[1] < b[1];
	}
	static void PlanExtraSlots()
	{
		if (!s_LayoutWritable || ConfiguredCount() <= 2) return;
		if (!AZ_VampLandmarks.s_Churches) AZ_VampLandmarks.s_Churches = new array<Object>;
		if (!s_ChurchSweepDone) { AZ_VampLandmarks.ChurchSweep();s_ChurchSweepDone = true; }
		float south = GetGame().GetWorld().GetWorldSize() * 0.5;
		for (int slot = 2; slot < CACHE_COUNT; slot++)
		{
			if (CachePosition(slot) != vector.Zero) continue;
			Object best;vector bestAt;float score = -1;
			foreach (Object church : AZ_VampLandmarks.s_Churches)
			{
				if (!church || !Building.Cast(church) || UsedChurch(church)) continue;
				if (slot < 4 && church.GetPosition()[2] >= south) continue;
				vector at;if (!InteriorPosition(church, at)) continue;
				float spread = 1000000;
				for (int used = 0; used < CACHE_COUNT; used++)
				{
					vector prior = CachePosition(used);
					if (prior != vector.Zero) spread = Math.Min(spread, vector.Distance(at, prior));
				}
				if (!best || spread > score || (spread == score && StableBefore(church.GetPosition(), best.GetPosition()))) { best = church;bestAt = at;score = spread; }
			}
			if (!best) { Print("[AZ_Vamp] hunter cache: slot " + (slot + 1) + " UNPLACED: no other verified church interior; southernRequired=" + (slot < 4));continue; }
			AZ_VampHunterSlot record = s_Layout.slots[slot];record.position = bestAt;record.churchClass = best.GetType();record.churchPosition = best.GetPosition();
			SaveLayout();
			Print("[AZ_Vamp] hunter cache: stable slot " + (slot + 1) + " church=" + record.churchClass + " churchAt=" + record.churchPosition + " interior=" + bestAt + " south=" + (bestAt[2] < south));
		}
	}
	static void EnsureConfigured()
	{
		EnsureMemory();LoadLayout();PlanExtraSlots();
		int count = ConfiguredCount();
		for (int slot = 0; slot < count; slot++)
		{
			vector at = CachePosition(slot);if (at == vector.Zero) continue;
			if (slot >= 2 && !CacheChurch(slot)) { Print("[AZ_Vamp] hunter cache: saved church missing for slot " + (slot + 1) + "; anchor retained");continue; }
			string where = "church cache " + (slot + 1);
			EntityAI chest = s_Caches[slot];
			if (!chest) chest = RecoverSlot(slot, at);
			AZ_VampHunterSlot record = s_Layout.slots[slot];
			// AZ-2605: a placed chest that is gone (CE cleanup before the lifetime fix, or a lost save) is rebuilt; RecoverSlot merges any later duplicate.
			if (!chest && record.placed && s_LayoutWritable)
			{
				Print("[AZ_Vamp] hunter cache: " + where + " was placed but is gone; rebuilding");
				record.placed = false;
			}
			if (!chest && !record.placed && s_LayoutWritable)
			{
				if (slot >= 2 && !InteriorClear(CacheChurch(slot), at)) { Print("[AZ_Vamp] hunter cache: blocked interior " + where + "; anchor retained, no fallback spawn");continue; }
				chest = EntityAI.Cast(GetGame().CreateObjectEx("AZ_VampHunterChest", at, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_DYNAMIC_PERSISTENCY));
				if (chest)
				{
					Object church = CacheChurch(slot);
					if (slot >= 2 && church) chest.SetOrientation(church.GetOrientation());
					AZ_VampLootPlace.Settle(chest, where);
				}
			}
			if (!chest) { Print("[AZ_Vamp] hunter cache: " + where + " missing; known persistent entity is never replaced");continue; }
			s_Caches[slot] = chest;record.placed = true;SaveLayout();KeepAlive(chest);
			if (!s_FillAttempted[slot])
			{
				s_FillAttempted[slot] = true;s_LastCheckedAt[slot] = GetGame().GetTime();
				if (Empty(chest)) { bool filled = Fill(chest);Print("[AZ_Vamp] hunter cache: " + where + " empty startup fill " + filled + " at " + chest.GetPosition()); }
			}
		}
		s_LastConfiguredCount = count;
	}
	static void RefillAt(int nowMs, int periodMs)
	{
		if (!GetGame().IsServer() || !s_StartupHandled || periodMs <= 0) return;
		EnsureMemory();
		for (int slot = 0; slot < ConfiguredCount(); slot++)
		{
			EntityAI chest = s_Caches[slot];
			vector at = CachePosition(slot);
			if (!chest && at != vector.Zero) { chest = RecoverSlot(slot, at);s_Caches[slot] = chest; }
			if (!chest) continue;
			KeepAlive(chest);
			bool refill = ShouldRefill(Empty(chest), nowMs, s_LastCheckedAt[slot], periodMs);
			if (PeriodDue(nowMs, s_LastCheckedAt[slot], periodMs)) s_LastCheckedAt[slot] = nowMs;
			if (!refill) continue;
			bool filled = Fill(chest);
			Print("[AZ_Vamp] hunter cache: church cache " + (slot + 1) + " empty timed refill " + filled + " items=" + (chest.GetInventory().CountInventory() - 1));
		}
	}
	static void Tick()
	{
		if (!GetGame().IsServer() || !s_StartupHandled) return;
		if (ConfiguredCount() != s_LastConfiguredCount) EnsureConfigured();
		RefillAt(GetGame().GetTime(), ValidRefillMin(AZ_VampState.Cfg().hunterCacheRefillMin) * 60000);
	}
	static void Startup()
	{
		if (s_StartupHandled || !GetGame().IsServer()) return;
		s_StartupHandled = true;EnsureConfigured();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Tick);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, 5000, true);
	}

	static string Status()
	{
		EnsureMemory();
		string status = "hunter caches";
		for (int slot = 0; slot < CACHE_COUNT; slot++)
		{
			EntityAI chest = s_Caches[slot];
			if (!chest || !chest.GetInventory())
				status += " | " + (slot + 1) + ": missing";
			else
				status += " | " + (slot + 1) + ": " + (chest.GetInventory().CountInventory() - 1) + " items at " + chest.GetPosition();
		}
		return status;
	}


	static void ResetStartupLatchForProbe()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Tick);
		s_StartupHandled = false;
		s_LastConfiguredCount = -1;
		s_LastCheckedAt.Clear();
		s_Caches.Clear();
		s_FillAttempted.Clear();
	}
}
