







class AZ_VampLandmarks
{
	static const float SEARCH_RADIUS = 500;
	static const float MERGE_DIST = 60;
	static ref array<Object> s_Churches;
	static ref array<vector> s_Graveyards;
	static ref array<Fireplace> s_Fires;
	static ref array<string> s_FireNames;
	static ref array<Object> s_MistZones;

	static bool IsChurch(string s)
	{
		return s.Contains("church") || s.Contains("chapel") || s.Contains("kaple") || s.Contains("kostel") || s.Contains("cerkev");
	}

	static bool IsGrave(string s)
	{
		if (s.Contains("engraved"))
			return false;
		return s.Contains("cemetery") || s.Contains("grave") || s.Contains("tomb") || s.Contains("hrob") || s.Contains("hrbitov");
	}

	static void Place()
	{
		s_Churches = new array<Object>;
		s_Graveyards = new array<vector>;
		s_Fires = new array<Fireplace>;
		s_FireNames = new array<string>;
		string world;
		GetGame().GetWorldName(world);
		world.ToLower();
		string base = "CfgWorlds " + world + " Names";
		int n = GetGame().ConfigGetChildrenCount(base);
		for (int i = 0; i < n; i++)
		{
			string cls;
			GetGame().ConfigGetChildName(base, i, cls);
			string type = GetGame().ConfigGetTextOut(base + " " + cls + " type");
			if (type != "City" && type != "Village" && type != "Local")
				continue;
			TFloatArray pos = new TFloatArray;
			GetGame().ConfigGetFloatArray(base + " " + cls + " position", pos);
			if (pos.Count() < 2)
				continue;
			vector c = Vector(pos[0], GetGame().SurfaceY(pos[0], pos[1]), pos[1]);
			Scan(c);
			if (type == "City" || type == "Village")
			{
				string place = GetGame().ConfigGetTextOut(base + " " + cls + " name");
				if (place == "" || place.IndexOf("$") == 0)
					place = cls;
				Campfire(c, place);
			}
		}
		if (s_Churches.Count() == 0)
			ChurchSweep();
		if (AZ_VampState.Cfg().halloween)
		{
			foreach (Object ch : s_Churches)
				Mist(ch.GetPosition(), 30);
			foreach (vector g : s_Graveyards)
			{
				Mist(g, 35);
				Goat(g);
				Goat(g);
			}


			float vm = AZ_VampState.Cfg().villageMistM;
			if (vm > 0)
			{
				foreach (Fireplace vf : s_Fires)
				{
					if (vf)
						Mist(vf.GetPosition(), vm, false);
				}
			}
		}
		Print("[AZ_Vamp] landmarks done: " + s_Churches.Count() + " churches, " + s_Graveyards.Count() + " graveyards, " + s_Fires.Count() + " campfires");
		AZ_VampProps.Place();


	}

	static void Scan(vector c)
	{
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(c, SEARCH_RADIUS, objs, proxies);
		foreach (Object o : objs)
		{
			string id = o.GetType() + " " + o.GetShapeName();
			id.ToLower();
			if (IsChurch(id) && !NearChurch(o.GetPosition()))
			{
				s_Churches.Insert(o);
				Print("[AZ_Vamp] landmark church " + id + " at " + o.GetPosition());
			}
			else if (IsGrave(id) && !Near(o.GetPosition()))
			{
				s_Graveyards.Insert(o.GetPosition());
				Print("[AZ_Vamp] landmark graveyard " + id + " at " + o.GetPosition());
			}
		}
	}



	static void ChurchSweep()
	{
		float size = GetGame().GetWorld().GetWorldSize();
		for (float x = 500; x < size; x += 1000)
		{
			for (float z = 500; z < size; z += 1000)
			{
				array<Object> objs = new array<Object>;
				array<CargoBase> proxies = new array<CargoBase>;
				GetGame().GetObjectsAtPosition(Vector(x, GetGame().SurfaceY(x, z), z), 710, objs, proxies);
				foreach (Object o : objs)
				{
					string id = o.GetType() + " " + o.GetShapeName();
					id.ToLower();
					if (IsChurch(id) && !NearChurch(o.GetPosition()))
					{
						s_Churches.Insert(o);
						Print("[AZ_Vamp] landmark church (sweep) " + id + " at " + o.GetPosition());
					}
				}
			}
		}
	}



	static bool NearChurch(vector p)
	{
		foreach (Object ch : s_Churches)
		{
			if (vector.Distance(ch.GetPosition(), p) < MERGE_DIST)
				return true;
		}
		return false;
	}

	static bool Near(vector p)
	{
		foreach (Object ch : s_Churches)
		{
			if (vector.Distance(ch.GetPosition(), p) < MERGE_DIST)
				return true;
		}
		foreach (vector g : s_Graveyards)
		{
			if (vector.Distance(g, p) < MERGE_DIST)
				return true;
		}
		return false;
	}

	static void Mist(vector p, float radius, bool tint = true)
	{


		vector hc = AZ_VampCrypt.Hall(20, 0);
		hc[1] = p[1];
		if (vector.Distance(hc, p) < radius + 60)
		{
			Print("[AZ_Vamp] mist skipped near the crypt at " + p);
			return;
		}
		EffectAreaParams prm = new EffectAreaParams();
		prm.m_ParamName = "AZ_VampMist";
		prm.m_ParamTriggerType = "SpookyTrigger23";
		prm.m_ParamRadius = radius;
		prm.m_ParamPosHeight = 8;
		prm.m_ParamNegHeight = 4;
		prm.m_ParamInnerRings = 1;
		prm.m_ParamInnerSpace = 15;
		prm.m_ParamOuterToggle = true;
		prm.m_ParamOuterSpace = 12;
		prm.m_ParamOuterOffset = 0;
		prm.m_ParamVertLayers = 0;
		prm.m_ParamVerticalOffset = 0;
		prm.m_ParamPartId = ParticleList.SPOOKY_MIST;
		prm.m_ParamAroundPartId = 0;
		prm.m_ParamTinyPartId = 0;
		prm.m_ParamPpeRequesterType = "";
		if (tint)
			prm.m_ParamPpeRequesterType = "PPERequester_SpookyAreaTint";
		EffectArea zone = EffectArea.Cast(GetGame().CreateObjectEx("SpookyArea23", p, ECE_PLACE_ON_SURFACE));
		if (zone)
		{
			zone.SetupZoneData(prm);
			if (!s_MistZones)
				s_MistZones = new array<Object>;
			s_MistZones.Insert(zone);
		}
	}

	static void Goat(vector p)
	{
		vector at = p + Vector(Math.RandomFloat(-12, 12), 0, Math.RandomFloat(-12, 12));
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		GetGame().CreateObjectEx("Animal_CapraHircus_Spooky", at, ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD);
	}


	static void Campfire(vector c, string place = "")
	{
		for (int t = 0; t < 12; t++)
		{
			vector p = c + Vector(Math.RandomFloat(-25, 25), 0, Math.RandomFloat(-25, 25));
			p[1] = GetGame().SurfaceY(p[0], p[2]);
			array<Object> objs = new array<Object>;
			array<CargoBase> proxies = new array<CargoBase>;
			GetGame().GetObjectsAtPosition(p, 3, objs, proxies);
			if (objs.Count() > 0 || GetGame().SurfaceIsSea(p[0], p[2]) || GetGame().SurfaceIsPond(p[0], p[2]))
				continue;
			Fireplace fire = Fireplace.Cast(GetGame().CreateObjectEx("Fireplace", p, ECE_PLACE_ON_SURFACE | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD));
			if (!fire)
				return;
			Fuel(fire);
			fire.GetInventory().CreateAttachment("Paper");
			fire.AZ_StartSanctuaryFire();
			s_Fires.Insert(fire);
			s_FireNames.Insert(place);
			for (int k = 0; k < 6 && AZ_VampState.Cfg().halloween; k++)
			{
				float a = k * 60 + Math.RandomFloat(-12, 12);
				vector off = Vector(Math.Sin(a * Math.DEG2RAD), 0, Math.Cos(a * Math.DEG2RAD)) * Math.RandomFloat(1.8, 2.6);
				AZ_VampDecor.Lantern(p + off, a + 180);
			}
			return;
		}
	}

	static void Fuel(Fireplace fire)
	{
		ItemBase wood = ItemBase.Cast(fire.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Firewood")));
		if (!wood)
			wood = ItemBase.Cast(fire.GetInventory().CreateAttachment("Firewood"));
		if (wood)
			wood.SetQuantity(wood.GetQuantityMax());
	}


	static void Relight()
	{
		if (!s_Fires)
			return;
		foreach (Fireplace fire : s_Fires)
		{
			if (!fire)
				continue;
			ItemBase wood = ItemBase.Cast(fire.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Firewood")));
			if (!wood)
				wood = ItemBase.Cast(fire.GetInventory().CreateAttachment("Firewood"));
			if (wood && wood.GetQuantity() < AZ_VampState.Cfg().fireWoodAtDusk)
				wood.SetQuantity(AZ_VampState.Cfg().fireWoodAtDusk);
			if (!fire.IsBurning())
			{
				if (!fire.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Paper")))
					fire.GetInventory().CreateAttachment("Paper");
				fire.AZ_StartSanctuaryFire();
			}
		}
	}


	static bool NearSanctuaryFire(PlayerBase p)
	{
		if (!s_Fires || AZ_VampState.Cfg().fireSanctuaryM <= 0)
			return false;
		foreach (Fireplace fire : s_Fires)
		{
			if (fire && fire.IsBurning() && vector.Distance(fire.GetPosition(), p.GetPosition()) < AZ_VampState.Cfg().fireSanctuaryM)
				return true;
		}
		return false;
	}

	static int FiresBurning()
	{
		int n = 0;
		if (!s_Fires)
			return 0;
		foreach (Fireplace fire : s_Fires)
		{
			if (fire && fire.IsBurning())
				n++;
		}
		return n;
	}


	static bool OnHolyGround(PlayerBase p)
	{
		if (!s_Churches || AZ_VampState.Cfg().holyBurnPerSec <= 0)
			return false;
		vector pos = p.GetPosition() + "0 0.5 0";
		foreach (Object ch : s_Churches)
		{
			if (!ch || vector.Distance(ch.GetPosition(), pos) > 30)
				continue;
			vector box[2];
			if (!ch.GetCollisionBox(box))
				continue;
			vector m = ch.WorldToModel(pos);
			if (m[0] > box[0][0] && m[0] < box[1][0] && m[1] > box[0][1] && m[1] < box[1][1] && m[2] > box[0][2] && m[2] < box[1][2])
				return true;
		}
		return false;
	}
}
