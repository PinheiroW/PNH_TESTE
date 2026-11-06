












class AZ_VampCryptLoot
{
	static int s_Night;
	static ref array<string> s_UsedGuns;
	static int s_SarcLoots;
	static int s_Rot;
	static int s_HoardsTonight;
	static int s_FindsTonight;

	static const int HOARD_SIDES = 2;
	static ref array<EntityAI> s_Chests;
	static ref array<int> s_HoardFills;
	static const int CACHE_COUNT = 3;
	static ref array<EntityAI> s_Caches;
	static ref array<int> s_CacheFills;

	static ref array<vector> s_SpotAt;
	static ref array<string> s_SpotWhere;
	static ref array<bool> s_SpotGun;
	static ref array<int> s_SpotEmpty;
	static const int SPOT_TICK_MS = 60000;

	static bool On()
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		return c && c.cryptLoot;
	}


	static void Night()
	{
		int n = AZ_VampCryptExit.NightKey();
		if (n == s_Night && s_UsedGuns)
			return;
		if (s_UsedGuns)
			Print("[AZ_Vamp] crypt loot: night " + s_Night + " ends: " + s_FindsTonight + " finds, " + s_SarcLoots + " sarcophagus, " + s_HoardsTonight + " hoards, guns " + s_UsedGuns.Count());
		s_Night = n;
		s_UsedGuns = new array<string>;
		s_SarcLoots = 0;
		s_FindsTonight = 0;
		s_HoardsTonight = 0;
		s_HoardFills = null;
		s_CacheFills = null;
	}

	static array<string> List(string s)
	{
		array<string> res = new array<string>;
		array<string> raw = new array<string>;
		s.Split(",", raw);
		foreach (string r : raw)
		{
			string t = r.Trim();
			if (t != "")
				res.Insert(t);
		}
		return res;
	}

	static EntityAI Make(string cls, vector at, EntityAI into)
	{
		if (cls == "")
			return null;
		EntityAI e;
		GameInventory intoInventory;
		if (into)
			intoInventory = into.GetInventory();
		if (intoInventory)
			e = intoInventory.CreateInInventory(cls);
		if (into && !e)
			e = EntityAI.Cast(GetGame().CreateObjectEx(cls, into.GetPosition() + Vector(Math.RandomFloatInclusive(-0.5, 0.5), 0.05, Math.RandomFloatInclusive(-0.5, 0.5)), ECE_PLACE_ON_SURFACE | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY));
		if (!into)
			e = EntityAI.Cast(GetGame().CreateObjectEx(cls, at, ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY));
		if (!e)
			Print("[AZ_Vamp] crypt loot: could not spawn " + cls + " (classname, or no room)");
		else if (!e.GetHierarchyParent())
			AZ_VampLootPlace.Settle(e, "crypt find");
		return e;
	}


	static void Wear(EntityAI e)
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		if (!e || !c)
			return;
		e.SetHealth01("", "", Math.RandomFloatInclusive(c.cryptLootHealthMin, c.cryptLootHealthMax));
	}


	static string PickGun()
	{
		Night();
		array<string> free = new array<string>;
		array<string> guns = List(AZ_VampState.Cfg().cryptLootGuns);
		foreach (string g : guns)
		{
			array<string> f = new array<string>;
			g.Split("|", f);
			if (f.Count() > 0 && s_UsedGuns.Find(f[0].Trim()) < 0)
				free.Insert(g);
		}
		if (free.Count() == 0)
			return "";
		string pick = free[Math.RandomInt(0, free.Count())];
		array<string> pf = new array<string>;
		pick.Split("|", pf);
		s_UsedGuns.Insert(pf[0].Trim());
		return pick;
	}


	static EntityAI Gun(string spec, vector at, EntityAI into, bool full)
	{
		array<string> f = new array<string>;
		spec.Split("|", f);
		while (f.Count() < 5)
			f.Insert("");
		EntityAI gun = Make(f[0].Trim(), at, into);
		if (!gun)
			return null;
		GameInventory gunInventory = gun.GetInventory();
		if (!gunInventory)
		{
			Print("[AZ_Vamp] crypt loot: " + gun.GetType() + " has no inventory for its kit");
			return gun;
		}
		array<string> atts = new array<string>;
		f[2].Split("+", atts);
		foreach (string a : atts)
		{
			if (a.Trim() != "")
				Wear(gunInventory.CreateAttachment(a.Trim()));
		}
		if (full && f[3].Trim() != "")
			Wear(gunInventory.CreateAttachment(f[3].Trim()));
		Weapon_Base w = Weapon_Base.Cast(gun);
		if (w && f[1].Trim() != "")
			w.SpawnAttachedMagazine(f[1].Trim());
		if (full)
		{
			Make(f[1].Trim(), at, into);
			Make(f[4].Trim(), at, into);
		}
		Wear(gun);
		return gun;
	}


	static EntityAI Item(string spec, vector at, EntityAI into)
	{
		string head = spec;
		string cargo = "";
		int gt = spec.IndexOf(">");
		if (gt >= 0)
		{
			head = spec.Substring(0, gt);
			cargo = spec.Substring(gt + 1, spec.Length() - gt - 1);
		}
		array<string> parts = new array<string>;
		head.Split("+", parts);
		EntityAI base = Make(parts[0].Trim(), at, into);
		if (!base)
			return null;
		GameInventory baseInventory = base.GetInventory();
		if (!baseInventory)
		{
			Print("[AZ_Vamp] crypt loot: " + base.GetType() + " has no inventory for attachments or cargo");
			return base;
		}
		for (int i = 1; i < parts.Count(); i++)
		{
			array<string> chain = new array<string>;
			parts[i].Split("/", chain);
			EntityAI holder = base;
			foreach (string c : chain)
			{
				if (!holder)
					break;
				GameInventory holderInventory = holder.GetInventory();
				if (!holderInventory)
				{
					holder = null;
					break;
				}
				holder = holderInventory.CreateAttachment(c.Trim());
			}
		}
		if (cargo != "")
		{
			array<string> cs = new array<string>;
			cargo.Split("+", cs);
			foreach (string cc : cs)
				baseInventory.CreateInInventory(cc.Trim());
		}
		return base;
	}




	static string s_There;

	static bool HasPiece(vector at, float r)
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		array<string> ours = new array<string>;
		array<string> guns = List(c.cryptLootGuns);
		foreach (string g : guns)
		{
			array<string> gf = new array<string>;
			g.Split("|", gf);
			ours.Insert(gf[0].Trim());
		}
		array<string> armor = List(c.cryptLootArmor);
		foreach (string a : armor)
		{
			array<string> af = new array<string>;
			a.Split("+", af);
			ours.Insert(af[0].Trim());
		}
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(at, r, near, null);
		foreach (Object o : near)
		{
			EntityAI e = EntityAI.Cast(o);
			if (!e || e.GetHierarchyParent() || ours.Find(e.GetType()) < 0)
				continue;


			float dy = at[1] - e.GetPosition()[1];
			if (dy > 3.0)
			{
				Print("[AZ_Vamp] crypt loot: deleting a buried find " + e.GetType() + " at " + e.GetPosition() + " (" + dy + " m under the spot)");
				GetGame().ObjectDelete(e);
				continue;
			}
			if (vector.Distance(e.GetPosition(), at) > r)
				continue;
			s_There = e.GetType() + " at " + e.GetPosition();
			AZ_VampLootPlace.Track(e, "a find from an earlier build");
			return true;
		}
		return false;
	}


	static EntityAI Piece(vector at, string where)
	{
		s_Rot++;
		return Spot(at, where, s_Rot % 2 == 1);
	}


	static EntityAI Spot(vector at, string where, bool gun)
	{
		if (!s_SpotAt)
		{
			s_SpotAt = new array<vector>;
			s_SpotWhere = new array<string>;
			s_SpotGun = new array<bool>;
			s_SpotEmpty = new array<int>;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpotTick, SPOT_TICK_MS, true);
		}
		if (s_SpotWhere.Find(where) < 0)
		{
			s_SpotAt.Insert(at);
			s_SpotWhere.Insert(where);
			s_SpotGun.Insert(gun);
			s_SpotEmpty.Insert(-1);
		}
		return Find(at, where, gun);
	}

	static void SpotTick()
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		if (!On() || c.cryptLootRespawnMin <= 0)
			return;
		int now = GetGame().GetTime();
		for (int i = 0; i < s_SpotAt.Count(); i++)
		{
			if (HasPiece(s_SpotAt[i], 1.5))
			{
				s_SpotEmpty[i] = -1;
				continue;
			}
			if (s_SpotEmpty[i] < 0)
			{
				s_SpotEmpty[i] = now;
				continue;
			}
			if (now - s_SpotEmpty[i] >= c.cryptLootRespawnMin * 60000)
			{
				s_SpotEmpty[i] = -1;
				Find(s_SpotAt[i], s_SpotWhere[i] + " (respawn)", s_SpotGun[i]);
			}
		}
	}


	static EntityAI Find(vector at, string where, bool gun)
	{
		if (!On())
		{
			Print("[AZ_Vamp] crypt loot: " + where + " -> off (cryptLoot false)");
			return null;
		}
		if (HasPiece(at, 1.5))
		{
			Print("[AZ_Vamp] crypt loot: " + where + " -> " + s_There + " still lies there, nothing new");
			return null;
		}
		Night();
		EntityAI e;
		if (gun)
		{
			string g = PickGun();
			if (g != "")
				e = Gun(g, at, null, false);
		}
		if (!e)
		{
			array<string> armor = List(AZ_VampState.Cfg().cryptLootArmor);
			if (armor.Count() > 0)
				e = Item(armor[Math.RandomInt(0, armor.Count())], at, null);
		}
		string what = "nothing";
		if (e)
		{
			what = e.GetType();
			s_FindsTonight++;
		}
		Print("[AZ_Vamp] crypt loot: " + where + " -> " + what + " at " + at + " (" + s_FindsTonight + " finds tonight)");
		return e;
	}



	static void SideCaches()
	{
		if (!On() || !AZ_VampCrypt.AnteOn())
			return;
		vector weaponAt = AZ_VampCrypt.DigChamberLoot();
		if (weaponAt != vector.Zero)
			SideCache(weaponAt, AZ_VampCrypt.HALL_YAW, 0, "Dig chamber weapon cache");
		SideCache(AZ_VampCryptGallery.DropOnto(32.0, -12.0, 1.0) + Vector(0, 0.05, 0), AZ_VampCrypt.HALL_YAW, 1, "Ossuary medical cache");
		SideCache(AZ_VampCryptGallery.DropOnto(38.4, -13.55, 1.0) + Vector(0, 0.05, 0), AZ_VampCrypt.HALL_YAW, 2, "Ossuary armor cache");
	}

	static EntityAI SideCache(vector at, float yaw, int slot, string where)
	{
		Night();
		AZ_VampConfig c = AZ_VampState.Cfg();
		if (!c || slot < 0 || slot >= CACHE_COUNT || c.cryptLootCacheChest == "")
			return null;
		if (!s_Caches)
		{
			s_Caches = new array<EntityAI>;
			for (int cacheInit = 0; cacheInit < CACHE_COUNT; cacheInit++)
				s_Caches.Insert(null);
		}
		if (!s_CacheFills)
		{
			s_CacheFills = new array<int>;
			for (int fillInit = 0; fillInit < CACHE_COUNT; fillInit++)
				s_CacheFills.Insert(0);
		}
		EntityAI cache = s_Caches[slot];
		if (!cache)
		{
			int cacheDuplicates = 0;
			array<Object> cacheNear = new array<Object>;
			GetGame().GetObjectsAtPosition(at, 1.5, cacheNear, null);
			foreach (Object cacheObject : cacheNear)
			{
				if (!cacheObject || cacheObject.GetType() != c.cryptLootCacheChest)
					continue;
				if (cache)
				{
					Print("[AZ_Vamp] crypt loot: duplicate " + where + " removed at " + cacheObject.GetPosition());
					GetGame().ObjectDelete(cacheObject);
					cacheDuplicates++;
					continue;
				}
				cache = EntityAI.Cast(cacheObject);
				s_Caches[slot] = cache;
			}
			Print("[AZ_Vamp] crypt loot: " + where + ": " + cacheDuplicates + " duplicate caches removed, cache kept " + (cache != null));
		}
		if (cache)
		{
			cache.SetOrientation(Vector(yaw, 0, 0));
			cache.SetPosition(at);
			AZ_VampLootPlace.Settle(cache, where + " (kept)");
		}
		GameInventory keptInventory;
		if (cache)
			keptInventory = cache.GetInventory();
		if (cache && !keptInventory)
		{
			Print("[AZ_Vamp] crypt loot: " + where + " has no inventory, no refill");
			return cache;
		}
		if (keptInventory && keptInventory.CountInventory() > 1)
		{
			Print("[AZ_Vamp] crypt loot: " + where + " still holds " + (keptInventory.CountInventory() - 1) + " things, no refill");
			return cache;
		}
		if (s_CacheFills[slot] >= c.cryptLootCachePerNight)
		{
			Print("[AZ_Vamp] crypt loot: " + where + " filled " + s_CacheFills[slot] + " times tonight already, no refill");
			return cache;
		}
		if (cache)
			GetGame().ObjectDelete(cache);
		cache = EntityAI.Cast(GetGame().CreateObjectEx(c.cryptLootCacheChest, at, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_DYNAMIC_PERSISTENCY));
		s_Caches[slot] = cache;
		if (!cache)
		{
			Print("[AZ_Vamp] crypt loot: " + where + " chest " + c.cryptLootCacheChest + " did not spawn");
			return null;
		}
		GameInventory cacheInventory = cache.GetInventory();
		if (!cacheInventory)
		{
			Print("[AZ_Vamp] crypt loot: " + where + " chest has no inventory");
			return cache;
		}
		s_CacheFills[slot] = s_CacheFills[slot] + 1;
		cache.SetOrientation(Vector(yaw, 0, 0));
		AZ_VampLootPlace.Settle(cache, where);
		if (slot == 0)
		{
			string cacheGun = PickGun();
			if (cacheGun != "")
				Gun(cacheGun, at, cache, true);
		}
		else
		{
			string cacheList = c.cryptLootCacheMedical;
			if (slot == 2)
				cacheList = c.cryptLootCacheArmor;
			array<string> cacheItems = List(cacheList);
			foreach (string cacheSpec : cacheItems)
				Item(cacheSpec, at, cache);
		}
		Print("[AZ_Vamp] crypt loot: " + where + " filled at " + at + " with " + (cacheInventory.CountInventory() - 1) + " things");
		return cache;
	}



	static string Sarc(PlayerBase p)
	{
		if (!p) return "nothing (false sarcophagus player missing)";

		Night();
		AZ_VampConfig c = AZ_VampState.Cfg();
		array<string> list = List(c.cryptLootSarc);
		string spec = "BandageDressing";
		if (list.Count() > 0 && s_SarcLoots < c.cryptLootSarcPerNight)
		{
			spec = list[Math.RandomInt(0, list.Count())];
			s_SarcLoots++;
		}
		EntityAI item;
		HumanInventory humanInventory;
		if (p)
			humanInventory = p.GetHumanInventory();
		if (humanInventory && spec.IndexOf("+") < 0 && spec.IndexOf(">") < 0)
			item = humanInventory.CreateInHands(spec);
		if (!item)
			item = Item(spec, p.GetPosition() + Vector(0, 0.05, 0), null);
		return spec + " (" + (item != null) + ", " + s_SarcLoots + " tonight)";
	}

	static void HoardType(map<string, int> types, string cls)
	{
		cls = cls.Trim();
		if (cls == "")
			return;
		int count;
		if (types.Contains(cls))
			count = types.Get(cls);
		types.Set(cls, count + 1);
	}

	static void HoardItemTypes(map<string, int> types, string spec)
	{
		string head = spec;
		string cargo = "";
		int gt = spec.IndexOf(">");
		if (gt >= 0)
		{
			head = spec.Substring(0, gt);
			cargo = spec.Substring(gt + 1, spec.Length() - gt - 1);
		}
		array<string> parts = new array<string>;
		head.Split("+", parts);
		foreach (string part : parts)
		{
			array<string> chain = new array<string>;
			part.Split("/", chain);
			foreach (string link : chain)
				HoardType(types, link);
		}
		array<string> cargoParts = new array<string>;
		cargo.Split("+", cargoParts);
		foreach (string cargoPart : cargoParts)
			HoardType(types, cargoPart);
	}

	static void HoardGunTypes(map<string, int> types, string spec)
	{
		array<string> f = new array<string>;
		spec.Split("|", f);
		while (f.Count() < 5)
			f.Insert("");
		HoardType(types, f[0]);
		if (f[1].Trim() != "")
		{
			HoardType(types, f[1]);
			HoardType(types, f[1]);
		}
		array<string> atts = new array<string>;
		f[2].Split("+", atts);
		foreach (string att : atts)
			HoardType(types, att);
		HoardType(types, f[3]);
		HoardType(types, f[4]);
	}

	static bool HoardTypesMatch(EntityAI chest, map<string, int> expected)
	{
		if (!chest || !chest.GetInventory())
			return false;
		map<string, int> actual = new map<string, int>;
		array<EntityAI> contents = new array<EntityAI>;
		chest.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, contents);
		foreach (EntityAI content : contents)
		{
			if (content && content != chest)
				HoardType(actual, content.GetType());
		}
		if (actual.Count() != expected.Count())
			return false;
		foreach (string cls, int expectedCount : expected)
		{
			if (!actual.Contains(cls) || actual.Get(cls) != expectedCount)
				return false;
		}
		return true;
	}

	static bool IsCurrentHoard(EntityAI chest, AZ_VampConfig c)
	{
		array<string> hoard = List(c.cryptLootHoard);
		int itemCount = Math.Min(c.cryptLootHoardItemsPerChest, hoard.Count());
		if (itemCount <= 0)
			return false;
		for (int start = 0; start < hoard.Count(); start++)
		{
			map<string, int> expected = new map<string, int>;
			for (int item = 0; item < itemCount; item++)
				HoardItemTypes(expected, hoard[(start + item) % hoard.Count()]);
			if (HoardTypesMatch(chest, expected))
				return true;
		}
		return false;
	}

	static bool IsFullLegacyHoard(EntityAI chest, AZ_VampConfig c)
	{
		array<string> guns = List(c.cryptLootGuns);
		foreach (string gunSpec : guns)
		{
			map<string, int> expected = new map<string, int>;
			array<string> legacy = List(c.cryptLootHoardLegacy);
			foreach (string itemSpec : legacy)
				HoardItemTypes(expected, itemSpec);
			HoardGunTypes(expected, gunSpec);
			if (HoardTypesMatch(chest, expected))
				return true;
		}
		return false;
	}

	static void ClearHoardCargo(EntityAI chest)
	{
		if (!chest || !chest.GetInventory())
			return;
		array<EntityAI> contents = new array<EntityAI>;
		chest.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, contents);
		for (int item = contents.Count() - 1; item >= 0; item--)
		{
			if (contents[item] && contents[item].GetHierarchyParent() == chest)
				GetGame().ObjectDelete(contents[item]);
		}
	}




	static void ServerReset()
	{
		s_Chests = null;
		s_HoardFills = null;
		s_Caches = null;
		s_CacheFills = null;
		if (s_SpotAt)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(SpotTick);
		s_SpotAt = null;
		s_SpotWhere = null;
		s_SpotGun = null;
		s_SpotEmpty = null;
	}

	static void Hoard(vector at, float yaw, int side = 0)
	{
		if (!On())
			return;
		Night();
		AZ_VampConfig c = AZ_VampState.Cfg();
		if (!c || side < 0 || side >= HOARD_SIDES)
			return;
		if (side == 0)
			SideCaches();
		if (!s_Chests)
		{
			s_Chests = new array<EntityAI>;
			for (int sc = 0; sc < HOARD_SIDES; sc++)
				s_Chests.Insert(null);
		}
		if (!s_HoardFills)
		{
			s_HoardFills = new array<int>;
			for (int sf = 0; sf < HOARD_SIDES; sf++)
				s_HoardFills.Insert(0);
		}
		EntityAI s_Chest = s_Chests[side];
		if (!s_Chest)
		{
			int dupChests = 0;
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(at, 1.5, near, null);
			foreach (Object o : near)
			{
				if (!o || o.GetType() != c.cryptLootHoardChest)
					continue;
				EntityAI foundChest = EntityAI.Cast(o);
				if (!foundChest)
					continue;
				if (s_Chest)
				{
					GameInventory keptInventory = s_Chest.GetInventory();
					GameInventory foundInventory = foundChest.GetInventory();
					bool keptEmpty = keptInventory && keptInventory.CountInventory() <= 1;
					bool foundEmpty = foundInventory && foundInventory.CountInventory() <= 1;
					if (keptEmpty && !foundEmpty)
					{
						Print("[AZ_Vamp] crypt loot: empty duplicate hoard chest removed; occupied chest kept (" + foundChest.GetPosition() + ")");
						GetGame().ObjectDelete(s_Chest);
						s_Chest = foundChest;
						dupChests++;
					}
					else if (foundEmpty)
					{
						Print("[AZ_Vamp] crypt loot: empty duplicate hoard chest removed (" + foundChest.GetPosition() + ")");
						GetGame().ObjectDelete(foundChest);
						dupChests++;
					}
					else
						Print("[AZ_Vamp] crypt loot: occupied duplicate hoard chest preserved for manual cleanup (" + foundChest.GetPosition() + ")");
					continue;
				}
				s_Chest = foundChest;
			}
			s_Chests[side] = s_Chest;
			Print("[AZ_Vamp] crypt loot: " + dupChests + " saved duplicate hoard chests removed at " + at + ", chest kept " + (s_Chest != null));
		}


		if (s_Chest)
		{
			s_Chest.SetOrientation(Vector(yaw, 0, 0));
			s_Chest.SetPosition(at);
			AZ_VampLootPlace.Settle(s_Chest, "the hoard's chest (kept)");
		}
		GameInventory keptHoardInventory;
		if (s_Chest)
			keptHoardInventory = s_Chest.GetInventory();
		if (s_Chest && !keptHoardInventory)
		{
			Print("[AZ_Vamp] crypt loot: the hoard's chest has no inventory, no refill");
			return;
		}


		bool migrationFill;
		if (keptHoardInventory && AZ_VampState.CryptHoardFormat(side) != c.cryptLootHoardFormat)
		{
			if (keptHoardInventory.CountInventory() <= 1)
				Print("[AZ_Vamp] crypt loot: side " + side + " unmarked empty hoard will be normalized to format " + c.cryptLootHoardFormat);
			else if (IsFullLegacyHoard(s_Chest, c))
			{
				Print("[AZ_Vamp] crypt loot: side " + side + " exact full legacy hoard migrating once to format " + c.cryptLootHoardFormat);
				ClearHoardCargo(s_Chest);
				migrationFill = true;
			}
			else if (IsCurrentHoard(s_Chest, c))
			{
				AZ_VampState.SetCryptHoardFormat(side, c.cryptLootHoardFormat);
				Print("[AZ_Vamp] crypt loot: side " + side + " exact current hoard marked format " + c.cryptLootHoardFormat + ", cargo preserved");
				return;
			}
			else
			{
				Print("[AZ_Vamp] crypt loot: side " + side + " unmarked cargo is partial, mixed or player-modified; manual migration required, cargo preserved");
				return;
			}
		}


		if (!migrationFill && keptHoardInventory && keptHoardInventory.CountInventory() > 1)
		{
			Print("[AZ_Vamp] crypt loot: the hoard's chest still holds " + (keptHoardInventory.CountInventory() - 1) + " things, no refill");
			return;
		}
		if (s_HoardFills[side] >= c.cryptLootHoardPerNight)
		{
			Print("[AZ_Vamp] crypt loot: side " + side + ": " + s_HoardFills[side] + " hoards tonight already (cryptLootHoardPerNight), no refill");
			return;
		}
		if (!s_Chest)
		{
			s_Chest = EntityAI.Cast(GetGame().CreateObjectEx(c.cryptLootHoardChest, at, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_DYNAMIC_PERSISTENCY));
			s_Chests[side] = s_Chest;
		}
		if (!s_Chest)
		{
			Print("[AZ_Vamp] crypt loot: the hoard's chest " + c.cryptLootHoardChest + " did not spawn");
			return;
		}
		GameInventory hoardInventory = s_Chest.GetInventory();
		if (!hoardInventory)
		{
			Print("[AZ_Vamp] crypt loot: the hoard's chest has no inventory");
			return;
		}
		s_HoardsTonight++;
		s_HoardFills[side] = s_HoardFills[side] + 1;
		s_Chest.SetOrientation(Vector(yaw, 0, 0));
		AZ_VampLootPlace.Settle(s_Chest, "the hoard's chest");
		string g;
		EntityAI gun;
		if (c.cryptLootHoardGun)
		{
			g = PickGun();
			if (g != "")
				gun = Gun(g, at, s_Chest, true);
		}
		array<string> hoard = List(c.cryptLootHoard);
		int hoardItems = Math.Min(c.cryptLootHoardItemsPerChest, hoard.Count());
		for (int hoardItem = 0; hoardItem < hoardItems; hoardItem++)
		{
			int hoardPick = (side + s_HoardFills[side] - 1 + hoardItem) % hoard.Count();
			Item(hoard[hoardPick], at, s_Chest);
		}
		if (IsCurrentHoard(s_Chest, c))
			AZ_VampState.SetCryptHoardFormat(side, c.cryptLootHoardFormat);
		else
			Print("[AZ_Vamp] crypt loot: side " + side + " did not match current format after fill; provenance not marked");
		Print("[AZ_Vamp] crypt loot: the Countess's hoard (side " + side + ") " + s_HoardsTonight + " tonight at " + at + " (night " + s_Night + "), gun " + gun + ", " + (hoardInventory.CountInventory() - 1) + " things");
	}
}
