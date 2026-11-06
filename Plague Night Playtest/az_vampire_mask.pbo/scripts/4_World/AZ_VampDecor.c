







class AZ_VampDecor
{
	static const int PER_TOWN = 80;
	static const float TOWN_RADIUS = 400;
	static ref array<Object> s_Lanterns;



	static bool IsHome(string type)
	{
		type.ToLower();
		if (type.Contains("container") || type.Contains("wreck") || type.Contains("mil_") || type.Contains("guard") || type.Contains("factory") || type.Contains("station"))
			return false;
		if (type.Contains("shed") || type.Contains("slum"))
			return false;
		if (type.Contains("house") || type.Contains("hous") || type.Contains("apartment") || type.Contains("vill_"))
			return true;


		return type.Contains("tenement") || type.Contains("bouda") || type.Contains("hut_") || type.Contains("budova") || type.Contains("dum_");
	}



	static const float LANTERN_GAP = 10;
	static ref map<int, ref array<vector>> s_Grid;

	static int GridKey(float x, float z)
	{
		return Math.Floor(x / LANTERN_GAP) * 100000 + Math.Floor(z / LANTERN_GAP);
	}


	static bool TooClose(vector p, float gap = LANTERN_GAP)
	{
		if (!s_Grid)
			return false;
		int cx = Math.Floor(p[0] / LANTERN_GAP);
		int cz = Math.Floor(p[2] / LANTERN_GAP);
		int reach = Math.Ceil(gap / LANTERN_GAP);
		for (int gx = cx - reach; gx <= cx + reach; gx++)
		{
			for (int gz = cz - reach; gz <= cz + reach; gz++)
			{
				array<vector> cell = s_Grid.Get(gx * 100000 + gz);
				if (!cell)
					continue;
				foreach (vector q : cell)
				{
					if (vector.DistanceSq(p, q) < gap * gap)
						return true;
				}
			}
		}
		return false;
	}

	static void Remember(vector p)
	{
		if (!s_Grid)
			s_Grid = new map<int, ref array<vector>>;
		int k = GridKey(p[0], p[2]);
		array<vector> cell = s_Grid.Get(k);
		if (!cell)
		{
			cell = new array<vector>;
			s_Grid.Set(k, cell);
		}
		cell.Insert(p);
	}




	static bool s_Script;
	static ref array<Object> s_Stale;

	static Object Spawn(string type, vector p)
	{
		s_Script = true;
		Object o = GetGame().CreateObjectEx(type, p, ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD);
		s_Script = false;
		return o;
	}

	static void NoteStale(Object o)
	{
		if (!s_Stale)
			s_Stale = new array<Object>;
		s_Stale.Insert(o);
	}



	static void ForgetStale(Object o)
	{
		if (!s_Stale)
			return;
		int i = s_Stale.Find(o);
		if (i >= 0)
			s_Stale.Remove(i);
	}

	static void Sweep()
	{
		if (!s_Stale)
			return;
		int removed = 0;
		int kept = 0;
		foreach (Object old : s_Stale)
		{
			if (!old)
				continue;
			bool dup = false;
			if (s_Lanterns)
			{
				foreach (Object fresh : s_Lanterns)
				{
					if (fresh && vector.DistanceSq(fresh.GetPosition(), old.GetPosition()) < LANTERN_GAP * LANTERN_GAP)
					{
						dup = true;
						break;
					}
				}
			}
			if (dup)
			{
				GetGame().ObjectDelete(old);
				removed++;
			}
			else
				kept++;
		}
		Print("[AZ_Vamp] decor: " + removed + " saved duplicate pumpkins removed, " + kept + " saved ones kept (not near a placed one)");
		s_Stale = null;
	}


	static void Lantern(vector pos, float yaw)
	{
		if (!AZ_VampState.Cfg().scriptDecor || !AZ_VampState.Cfg().decorLanterns)
			return;
		vector p = pos;
		float floorY = GroundY(pos[0], pos[2]);
		p[1] = floorY;
		if (!OutdoorClear(p))
			return;
		if (AZ_VampScenes.Blocked(p))
			return;
		if (TooClose(p))
			return;
		Object lantern = Spawn("AZ_VampPumpkinLantern", p);
		if (!lantern)
			return;
		Remember(p);
		SitOn(lantern, p);
		lantern.SetOrientation(Vector(yaw, 0, 0));
		if (s_Lanterns)
			s_Lanterns.Insert(lantern);
	}




	static const string LIST = "$profile:AZ_Vampire\lanterns_spawner.json";
	static bool s_FromFile;

	static bool LoadList()
	{
		if (!FileExist(LIST))
			return false;
		ObjectSpawnerJson list;
		string err;
		if (!JsonFileLoader<ObjectSpawnerJson>.LoadFile(LIST, list, err) || !list || !list.Objects)
		{
			Print("[AZ_Vamp] decor: " + LIST + " unreadable, generating instead: " + err);
			return false;
		}
		foreach (ITEM_SpawnerObject it : list.Objects)
		{
			vector at = vector.ArrayToVec(it.pos);
			bool isPumpkin = it.name == "AZ_VampPumpkinLantern";
			if (isPumpkin && !OutdoorClear(at))
				continue;
			if (isPumpkin && TooClose(at))
				continue;
			Object o = Spawn(it.name, at);
			if (!o)
			{
				Print("[AZ_Vamp] decor: list could not spawn " + it.name);
				continue;
			}
			o.SetPosition(at);
			o.SetOrientation(vector.ArrayToVec(it.ypr));
			if (isPumpkin)
				Remember(at);
			s_Lanterns.Insert(o);
		}
		Print("[AZ_Vamp] decor: " + s_Lanterns.Count() + " objects from " + LIST);
		return true;
	}




	static bool OutdoorClear(vector p)
	{
		array<Object> blocked = new array<Object>;
		if (GetGame().IsBoxCollidingGeometry(p + Vector(0, 0.28, 0), vector.Zero, Vector(0.5, 0.5, 0.5), ObjIntersectGeom, ObjIntersectGeom, null, blocked))
			return false;
		for (int roofSample = 0; roofSample < 5; roofSample++)
		{
			vector q = p;
			if (roofSample == 1) q[0] = q[0] + 0.25;
			if (roofSample == 2) q[0] = q[0] - 0.25;
			if (roofSample == 3) q[2] = q[2] + 0.25;
			if (roofSample == 4) q[2] = q[2] - 0.25;
			vector roofHit, roofNormal;
			int roofComponent;
			set<Object> roofs = new set<Object>;
			if (!DayZPhysics.RaycastRV(q + Vector(0, 25, 0), q + Vector(0, 0.1, 0), roofHit, roofNormal, roofComponent, roofs, null, null, false, false, ObjIntersectGeom))
				continue;
			foreach (Object roof : roofs)
			{
				if (roof && Building.Cast(roof))
					return false;
			}
		}
		return true;
	}




	static bool SpotClear(vector p, vector from, Object home, out float h)
	{
		float ground = GroundY(p[0], p[2]);
		h = ground;
		float top = -1;
		for (int k = 0; k < 5; k++)
		{
			vector q = p;
			if (k == 1) q[0] = q[0] + 0.18;
			if (k == 2) q[0] = q[0] - 0.18;
			if (k == 3) q[2] = q[2] + 0.18;
			if (k == 4) q[2] = q[2] - 0.18;
			float g = GroundY(q[0], q[2]);
			vector hit, nrm;
			int comp;
			set<Object> res = new set<Object>;
			vector a = Vector(q[0], g + 1.6, q[2]);
			if (!DayZPhysics.RaycastRV(a, Vector(q[0], g - 0.1, q[2]), hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom))
				continue;
			Object o = null;
			foreach (Object r : res)
				o = r;
			if (!o || o.IsTree() || o.IsBush())
				continue;
			if (o.IsInherited(AZ_VampPumpkinLantern))
				return false;
			if (hit[1] < g + 0.12)
				continue;
			if (hit[1] > ground + 0.7 || nrm[1] < 0.85)
				return false;
			if (top < 0)
				top = hit[1];
			else if (Math.AbsFloat(hit[1] - top) > 0.06)
				return false;
		}
		if (top >= 0)
			h = top;
		if (!OutdoorClear(Vector(p[0], h, p[2])))
			return false;


		vector ra = Vector(from[0], h + 0.3, from[2]);
		vector rb = Vector(p[0], h + 0.3, p[2]);
		vector hit2, nrm2;
		int comp2;
		set<Object> res2 = new set<Object>;
		if (DayZPhysics.RaycastRV(ra, rb, hit2, nrm2, comp2, res2, null, home, false, false, ObjIntersectGeom))
		{
			foreach (Object r2 : res2)
			{
				if (r2 && r2 != home && !r2.IsTree() && !r2.IsBush())
					return false;
			}
		}
		return true;
	}


	static bool LanternAt(vector p, vector from, Object home, float yaw)
	{
		if (!AZ_VampState.Cfg().decorLanterns)
			return false;
		float h;
		if (!SpotClear(p, from, home, h))
			return false;
		p[1] = h;
		if (AZ_VampScenes.Blocked(p))
			return false;
		if (TooClose(p))
			return false;
		Object lantern = Spawn("AZ_VampPumpkinLantern", p);
		if (!lantern)
			return false;
		Remember(p);


		vector mm[2];
		lantern.ClippingInfo(mm);
		p[1] = h - mm[0][1];
		lantern.SetPosition(p);
		lantern.SetOrientation(Vector(yaw, 0, 0));
		s_Lanterns.Insert(lantern);
		return true;
	}


	static string Report(vector from)
	{
		if (!s_Lanterns)
			return "lanterns: none placed (not Namalsk, or decor not run yet)";
		Object best;
		float bestD = 1e9;
		int alive = 0;
		foreach (Object l : s_Lanterns)
		{
			if (!l)
				continue;
			alive++;
			float d = vector.Distance(l.GetPosition(), from);
			if (d < bestD)
			{
				bestD = d;
				best = l;
			}
		}
		if (!best)
			return "lanterns: " + alive + " of " + s_Lanterns.Count() + " exist";
		return "lanterns: " + alive + " of " + s_Lanterns.Count() + " exist, nearest " + Math.Round(bestD) + " m at " + best.GetPosition();
	}

	static void Place()
	{
		string world;
		GetGame().GetWorldName(world);
		world.ToLower();
		if (world != "namalsk")
			return;
		if (!AZ_VampState.Cfg().scriptDecor)
		{
			Print("[AZ_Vamp] decor: scriptDecor false in config.json, no script pumpkins or candles (Object Spawner placements only)");
			return;
		}
		if (!AZ_VampState.Cfg().halloween)
		{
			Print("[AZ_Vamp] decor: halloween off in config.json, no pumpkins or candles");
			return;
		}
		s_Lanterns = new array<Object>;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Sweep, 15000, false);
		s_FromFile = LoadList();
		string base = "CfgWorlds namalsk Names";
		int n = GetGame().ConfigGetChildrenCount(base);
		int total = 0;
		int towns = 0;





		int weightLeft = 0;
		for (int e = 0; e < n; e++)
		{
			string ecls;
			GetGame().ConfigGetChildName(base, e, ecls);
			string et = GetGame().ConfigGetTextOut(base + " " + ecls + " type");
			if (et == "City" || et == "Village" || et == "Local")
			{
				TFloatArray epos = new TFloatArray;
				GetGame().ConfigGetFloatArray(base + " " + ecls + " position", epos);
				weightLeft += Weight(epos);
			}
		}

		for (int pass = 0; pass < 2; pass++)
		{
		s_PropPass = pass == 1;
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
			int want = PER_TOWN;
			if (type == "Local")
				want = PER_TOWN / 2;
			if (type == "City")
				want = PER_TOWN * 2;
			int budget = MAX_LANTERNS;
			if (!s_PropPass)
			{
				int doorCap = MAX_LANTERNS * DOOR_PCT / 100;
				int w = Weight(pos);
				budget = Math.Max(0, doorCap - s_Lanterns.Count()) * w / Math.Max(1, weightLeft);
				weightLeft -= w;
			}
			int got = Town(Vector(pos[0], GetGame().SurfaceY(pos[0], pos[1]), pos[1]), want, budget);
			Print("[AZ_Vamp] decor " + cls + " (" + type + "): " + got + " lanterns");
			total += got;
			if (!s_PropPass)
				towns++;
		}
		if (pass == 0)
			Print("[AZ_Vamp] decor doorsteps: " + s_Lanterns.Count() + " pumpkins");
		}
		CensusDump();
		Print("[AZ_Vamp] decor done: " + total + " buildings, " + s_Lanterns.Count() + " pumpkins (cap " + MAX_LANTERNS + ") in " + towns + " settlements");
	}





	static const int SPAWN_WEIGHT = 2;

	static int Weight(TFloatArray pos)
	{
		if (pos.Count() >= 2 && pos[1] >= AZ_VampState.Cfg().spawnZoneMinZ)
			return SPAWN_WEIGHT;
		return 1;
	}

	static bool s_PropPass;
	static const int DOOR_PCT = 85;
	static const int MAX_LANTERNS = 2500;

	static bool IsDecor(string type)
	{
		type.ToLower();
		if (IsHome(type))
			return true;
		if (type.Contains("container") || type.Contains("wreck") || type.Contains("mil_") || type.Contains("guard") || type.Contains("tower") || type.Contains("tent"))
			return false;
		return type.Contains("shed") || type.Contains("slum") || type.Contains("garage") || type.Contains("workshop") || type.Contains("barn") || type.Contains("church") || type.Contains("pub") || type.Contains("shop") || type.Contains("school") || type.Contains("office") || type.Contains("hospital");
	}



	static bool IsWell(string id)
	{
		return id.Contains("well") && !id.Contains("wreck") && !id.Contains("wall");
	}



	static int PropPumpkins(string id)
	{
		if (id.Contains("hay_block") || id.Contains("haystack") || id.Contains("hay_bale"))
			return 2;
		if (id.Contains("barel_fire") || id.Contains("barrel_fire"))
			return 2;
		if (id.Contains("smallcross") || id.Contains("tombstone") || id.Contains("cemetery_grave"))
			return 1;
		return 0;
	}



	static void BesideProp(Object o, int n)
	{
		vector op = o.GetPosition();
		vector box[2];
		bool hasBox = o.GetCollisionBox(box);
		for (int q = 0; q < n; q++)
		{
			vector spot = op + Vector(Math.RandomFloat(-0.8, 0.8), 0, Math.RandomFloat(-0.8, 0.8));
			if (hasBox)
			{
				float gap = 0.45;
				bool neg = Math.RandomInt(0, 2) == 0;
				vector lp;
				if (box[1][0] - box[0][0] < box[1][2] - box[0][2])
				{
					float lx = box[1][0] + gap;
					if (neg)
						lx = box[0][0] - gap;
					lp = Vector(lx, 0, Math.Lerp(box[0][2], box[1][2], Math.RandomFloat(0.2, 0.8)));
				}
				else
				{
					float lz = box[1][2] + gap;
					if (neg)
						lz = box[0][2] - gap;
					lp = Vector(Math.Lerp(box[0][0], box[1][0], Math.RandomFloat(0.2, 0.8)), 0, lz);
				}
				spot = o.ModelToWorld(lp);
				if (vector.Distance(spot, op) > 4)
					continue;
			}
			Lantern(spot, Math.RandomFloat(0, 360));
		}
	}



	static void WellTop(Object o)
	{
		vector op = o.GetPosition();
		float ground = GetGame().SurfaceY(op[0], op[2]);
		for (int t = 0; t < 6; t++)
		{
			float a = Math.RandomFloat(0, Math.PI2);
			float r = Math.RandomFloat(0.45, 0.7);
			vector top = Vector(op[0] + Math.Cos(a) * r, ground + 3.0, op[2] + Math.Sin(a) * r);
			vector hit, nrm;
			int comp;
			set<Object> res = new set<Object>;
			if (!DayZPhysics.RaycastRV(top, top - "0 3.2 0", hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom))
				continue;
			if (hit[1] < ground + 0.4 || hit[1] > ground + 1.6 || nrm[1] < 0.7)
				continue;
			if (AZ_VampScenes.Blocked(hit))
				return;
			if (!OutdoorClear(hit))
				continue;
			if (TooClose(hit))
				return;
			Object lantern = Spawn("AZ_VampPumpkinLantern", hit);
			if (!lantern)
				return;
			Remember(hit);
			SitOn(lantern, hit);
			lantern.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
			s_Lanterns.Insert(lantern);
			return;
		}
	}





	static const float ROAD_REACH = 1.2;

	static float GroundY(float x, float z)
	{
		float terrain = GetGame().SurfaceY(x, z);
		float road = GetGame().SurfaceRoadY3D(x, terrain + ROAD_REACH, z, RoadSurfaceDetection.UNDER);
		if (road > terrain && road <= terrain + ROAD_REACH)
			return road;
		return terrain;
	}


	static float FloorY(vector p)
	{
		float ground = GroundY(p[0], p[2]);
		vector top = Vector(p[0], ground + 2.5, p[2]);
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (DayZPhysics.RaycastRV(top, Vector(p[0], ground - 0.2, p[2]), hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom) && nrm[1] > 0.7 && hit[1] > ground + 0.02)
			return hit[1];
		return ground;
	}


	static void SitOn(Object o, vector at)
	{
		vector mm[2];
		o.ClippingInfo(mm);
		at[1] = at[1] - mm[0][1];
		o.SetPosition(at);
	}

	static bool Near(vector p, float r)
	{
		foreach (Object l : s_Lanterns)
		{
			if (l && vector.Distance(l.GetPosition(), p) < r)
				return true;
		}
		return false;
	}

	static int Town(vector centre, int want, int budget)
	{
		int start = s_Lanterns.Count();
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(centre, TOWN_RADIUS, objs, proxies);
		if (FileExist("$profile:AZ_Vampire/decor_scan.txt"))
			Census(objs);
		int placed = 0;
		foreach (Object o : objs)
		{
			if (s_Lanterns.Count() >= MAX_LANTERNS)
				break;
			if (!s_PropPass && (placed >= want || s_Lanterns.Count() - start >= budget))
				break;
			string id = o.GetType() + " " + o.GetShapeName();
			id.ToLower();
			Building b = Building.Cast(o);
			if (b && b.GetDoorCount() > 0 && IsDecor(b.GetType()))
			{
				if (s_PropPass)
					continue;
				if (Doorsteps(b))
					placed++;
			}
			else if (!s_PropPass || s_FromFile)
				continue;
			else if (IsWell(id) && !Near(o.GetPosition(), 3))
				WellTop(o);
			else if (PropPumpkins(id) > 0 && !Near(o.GetPosition(), 4))
				BesideProp(o, PropPumpkins(id));
		}
		return placed;
	}

	static ref map<string, int> s_Census;
	static void Census(array<Object> objs)
	{
		if (!s_Census)
			s_Census = new map<string, int>;
		foreach (Object o : objs)
		{
			string shape = o.GetShapeName();
			array<string> parts = new array<string>;
			shape.Split("\\", parts);
			string k = o.GetType();
			if (k == "" && parts.Count() > 0)
				k = parts[parts.Count() - 1];
			k.ToLower();
			if (k.Contains("tree") || k.Contains("bush") || k.Contains("grass") || k.Contains("_t_") || k.Contains("_b_") || k.Contains("stone_") || k.Contains("rock"))
				continue;
			int c = 0;
			if (s_Census.Contains(k))
				c = s_Census.Get(k);
			s_Census.Set(k, c + 1);
		}
	}

	static void CensusDump()
	{
		if (!s_Census)
			return;
		foreach (string k, int c : s_Census)
		{
			if (c >= 3)
				Print("[AZ_Vamp] census " + c + " " + k);
		}
	}


	static bool Doorsteps(Building b)
	{
		vector bc = b.GetPosition();
		float far = 0;
		int d;
		for (d = 0; d < b.GetDoorCount(); d++)
		{
			vector f0 = b.GetDoorSoundPos(d) - bc;
			f0[1] = 0;
			far = Math.Max(far, f0.Length());
		}
		if (far < 1)
			return false;
		bool any = false;
		for (d = 0; d < b.GetDoorCount(); d++)
		{
			vector door = b.GetDoorSoundPos(d);
			vector outv = door - bc;
			outv[1] = 0;
			if (outv.Length() < far * 0.6)
				continue;
			outv.Normalize();
			vector side = Vector(outv[2], 0, -outv[0]);
			vector p = door + outv * 0.9 + side * 0.6;
			float ground = GetGame().SurfaceY(p[0], p[2]);
			if (door[1] - ground > 2.0)
				continue;
			if (s_FromFile)
			{
				any = true;
				continue;
			}
			if (Near(door + outv * 0.9, 2.0))
				continue;
			float yaw = outv.VectorToAngles()[0];
			vector from = door + outv * 0.3;

			array<vector> spots = new array<vector>;
			spots.Insert(p);
			spots.Insert(door + outv * 0.9 - side * 0.6);
			spots.Insert(door + outv * 1.2);
			spots.Insert(p + outv * 0.45 + side * 0.35);
			spots.Insert(door + outv * 1.35 - side * 0.95);


			spots.Insert(door + outv * 2.0 + side * 0.8);
			spots.Insert(door + outv * 2.0 - side * 0.8);
			spots.Insert(door + outv * 3.0);
			int want = Math.Clamp(AZ_VampState.Cfg().lanternsPerDoor, 0, 4);
			if (door[2] >= AZ_VampState.Cfg().spawnZoneMinZ)
				want = Math.Clamp(AZ_VampState.Cfg().spawnLanternsPerDoor, 0, 4);
			int put = 0;
			foreach (vector sp : spots)
			{
				if (put >= want)
					break;
				if (LanternAt(sp, from, b, yaw + Math.RandomFloat(-30, 30)))
					put++;
			}
			if (put == 0)
				continue;
			any = true;


			if (Math.RandomFloat01() < AZ_VampState.Cfg().torchDoorShare && IsHome(b.GetType()))
				AZ_VampTorch.Plant(door + outv * 1.1 - side * 1.3, yaw, true);
		}
		if (any && IsHome(b.GetType()))
			AZ_VampCandles.House(b, b.GetDoorSoundPos(0)[1] - 1.0);
		return any;
	}
}


class AZ_VampPumpkinLantern extends PumpkinHelmet
{
	protected bool m_AZOn;


	void AZ_VampPumpkinLantern()
	{
		if (GetGame().IsServer() && !AZ_VampDecor.s_Script)
			AZ_VampDecor.NoteStale(this);
		if (!GetGame().IsDedicatedServer() && AZ_VampLightBudget.Active())
			AZ_VampLightBudget.Add(this);
	}

	override void OnSpawnByObjectSpawner(ITEM_SpawnerObject item)
	{
		super.OnSpawnByObjectSpawner(item);
		AZ_VampDecor.ForgetStale(this);
	}

	override void EEDelete(EntityAI parent)
	{
		AZ_VampLightBudget.Remove(this);
		super.EEDelete(parent);
	}

	override void UpdateGlowState()
	{
		if (!AZ_VampLightBudget.Active())
		{
			super.UpdateGlowState();
			return;
		}
		SetPilotLight(m_AZOn);
	}

	void AZ_SetLit(bool on)
	{
		m_AZOn = on;
		SetPilotLight(on);
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override bool CanPutAsAttachment(EntityAI parent)
	{
		return false;
	}

	override bool IsTakeable()
	{
		return false;
	}
}
