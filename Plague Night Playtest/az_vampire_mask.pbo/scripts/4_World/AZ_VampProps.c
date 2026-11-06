






class AZ_VampProps
{
	static ref array<vector> s_Scenes;
	static ref array<string> s_SceneNames;
	static int s_Objects;

	static void Place()
	{
		string world;
		GetGame().GetWorldName(world);
		world.ToLower();
		if (world != "namalsk" || !AZ_VampState.Cfg().halloween)
			return;
		s_Scenes = new array<vector>;
		s_SceneNames = new array<string>;
		s_Objects = 0;
		AZ_VampCrypt.Build();
		if (!AZ_VampState.Cfg().decorProps)
		{
			Print("[AZ_Vamp] props: decorProps false in config.json, crypt built, no village scenes");
			return;
		}
		Destinations("CfgWorlds namalsk Names");
		if (AZ_VampLandmarks.s_Fires)
		{
			for (int fi = 0; fi < AZ_VampLandmarks.s_Fires.Count(); fi++)
			{
				if (AZ_VampLandmarks.s_Fires[fi])
					Centrepiece(AZ_VampLandmarks.s_Fires[fi].GetPosition(), AZ_VampLandmarks.s_FireNames[fi]);
			}
		}
		if (AZ_VampLandmarks.s_Graveyards)
		{
			foreach (vector g : AZ_VampLandmarks.s_Graveyards)
				Graveyard(g);
		}
		if (AZ_VampLandmarks.s_Churches)
		{
			for (int ci = 0; ci < AZ_VampLandmarks.s_Churches.Count(); ci++)
			{
				if (AZ_VampLandmarks.s_Churches[ci])
					ChurchVigil(AZ_VampLandmarks.s_Churches[ci].GetPosition());
			}
		}
		string base = "CfgWorlds namalsk Names";
		int n = GetGame().ConfigGetChildrenCount(base);
		if (FileExist("$profile:AZ_Vampire/names_dump.txt"))
		{
			for (int ni = 0; ni < n; ni++)
			{
				string ncls;
				GetGame().ConfigGetChildName(base, ni, ncls);
				TFloatArray npos = new TFloatArray;
				GetGame().ConfigGetFloatArray(base + " " + ncls + " position", npos);
				string npos2 = "";
				if (npos.Count() >= 2)
					npos2 = npos[0].ToString() + " " + npos[1].ToString();
				Print("[AZ_Vamp] name " + ncls + " type " + GetGame().ConfigGetTextOut(base + " " + ncls + " type") + " '" + GetGame().ConfigGetTextOut(base + " " + ncls + " name") + "' at " + npos2);
			}
		}
		for (int i = 0; i < n; i++)
		{
			string cls;
			GetGame().ConfigGetChildName(base, i, cls);
			string type = GetGame().ConfigGetTextOut(base + " " + cls + " type");
			if (type != "City" && type != "Village")
				continue;
			TFloatArray pos = new TFloatArray;
			GetGame().ConfigGetFloatArray(base + " " + cls + " position", pos);
			if (pos.Count() < 2)
				continue;
			vector c = Vector(pos[0], GetGame().SurfaceY(pos[0], pos[1]), pos[1]);
			if (type == "City")
			{
				for (int k = 0; k < 3; k++)
					Scene(c, k, cls, 15, 70);
				Quarantine(c, cls);
				Quarantine(c, cls);
			}
			else
			{

				if (c[2] >= AZ_VampState.Cfg().spawnZoneMinZ)
				{
					for (int sk = 0; sk < 3; sk++)
						Scene(c, sk, cls, 12, 45);
				}
				else
				{
					Scene(c, Math.RandomInt(0, 3), cls, 12, 35);
					if (Math.RandomInt(0, 2) == 0)
						Scene(c, Math.RandomInt(0, 3), cls, 12, 35);
				}
			}
		}
		Print("[AZ_Vamp] props done: " + s_Scenes.Count() + " scenes, " + s_Objects + " objects");
	}





	static bool LitDressing(string type)
	{
		return type == "AZ_VampCandelabra" || type == "AZ_VampVotive" || type == "AZ_VampCandleCircle";
	}

	static AZ_VampTorch DecorTorch(vector at, float yaw)
	{
		if (!AZ_VampState.Cfg().scriptDecor)
			return null;
		return AZ_VampTorch.Plant(at, yaw);
	}

	static Object Put(string type, vector origin, float yaw, float dx, float dz, float turn = 0)
	{
		if (!AZ_VampState.Cfg().scriptDecor && LitDressing(type))
			return null;
		vector at;
		bool found = false;
		for (int t = 0; t < 7 && !found; t++)
		{
			float swing = 0;
			if (t > 0)
				swing = Math.Ceil(t / 2.0) * 25;
			if (t % 2 == 0)
				swing = -swing;
			float r = (yaw + swing) * Math.DEG2RAD;
			at = origin + Vector(dx * Math.Cos(r) + dz * Math.Sin(r), 0, -dx * Math.Sin(r) + dz * Math.Cos(r));
			at[1] = GetGame().SurfaceY(at[0], at[2]);
			if (dx == 0 && dz == 0)
				found = true;
			else
				found = PieceClear(at, 0.45);
		}
		if (!found)
		{
			Print("[AZ_Vamp] props: no room for " + type + " near " + origin);
			return null;
		}
		if (AZ_VampScenes.Blocked(at))
			return null;
		Object o = GetGame().CreateObjectEx(type, at, ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY);
		if (!o)
		{
			Print("[AZ_Vamp] props: could not spawn " + type);
			return null;
		}
		o.SetPosition(at);
		o.SetOrientation(Vector(yaw + turn, 0, 0));
		s_Objects++;
		return o;
	}


	static bool PieceClear(vector p, float rad)
	{
		for (int k = 0; k < 5; k++)
		{
			float qx = p[0];
			float qz = p[2];
			if (k == 1) qx += rad;
			if (k == 2) qx -= rad;
			if (k == 3) qz += rad;
			if (k == 4) qz -= rad;
			float g = GetGame().SurfaceY(qx, qz);
			vector hit, nrm;
			int comp;
			set<Object> res = new set<Object>;
			if (DayZPhysics.RaycastRV(Vector(qx, g + 3.0, qz), Vector(qx, g - 0.1, qz), hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom))
			{
				foreach (Object o : res)
				{
					if (o && !o.IsBush() && hit[1] > g + 0.1)
						return false;
				}
			}
		}
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(p, 1.2, near, null);
		foreach (Object n : near)
		{
			if (n && n.IsTree())
				return false;
		}
		return true;
	}


	static bool Open(vector p, float r)
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(p, r, near, null);
		foreach (Object n : near)
		{
			if (n && n.IsTree())
				return false;
		}
		return true;
	}


	static bool OpenSpot(vector c, float rMin, float rMax, float clear, out vector at)
	{
		for (int t = 0; t < 40; t++)
		{
			float a = Math.RandomFloat(0, Math.PI2);
			float r = Math.RandomFloat(rMin, rMax);
			at = c + Vector(Math.Cos(a) * r, 0, Math.Sin(a) * r);
			float y = GetGame().SurfaceY(at[0], at[2]);
			at[1] = y;
			if (GetGame().SurfaceIsSea(at[0], at[2]) || GetGame().SurfaceIsPond(at[0], at[2]))
				continue;
			bool ok = true;
			for (int k = 0; k < 9 && ok; k++)
			{
				float qa = k * Math.PI / 4;
				float qr = clear;
				if (k == 8)
					qr = 0;
				float qx = at[0] + Math.Cos(qa) * qr;
				float qz = at[2] + Math.Sin(qa) * qr;
				float g = GetGame().SurfaceY(qx, qz);
				if (Math.AbsFloat(g - y) > 0.45)
				{
					ok = false;
					break;
				}
				vector hit, nrm;
				int comp;
				set<Object> res = new set<Object>;
				if (DayZPhysics.RaycastRV(Vector(qx, g + 3.0, qz), Vector(qx, g - 0.1, qz), hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom))
				{
					foreach (Object o : res)
					{
						if (o && !o.IsTree() && !o.IsBush() && hit[1] > g + 0.1)
							ok = false;
					}
				}
			}
			if (ok && (t >= 30 || Open(at, 6)))
				return true;
		}
		return false;
	}



	static bool Grassy(vector p)
	{
		string surf;
		GetGame().SurfaceGetType(p[0], p[2], surf);
		surf.ToLower();
		return surf.Contains("grass") || surf.Contains("forest") || surf.Contains("trava");
	}

	static bool FrontYard(vector c, float radius, float clear, out vector at)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(c, radius, objs, null);
		array<Building> homes = new array<Building>;
		foreach (Object o : objs)
		{
			Building b = Building.Cast(o);
			if (b && b.GetDoorCount() > 0 && AZ_VampDecor.IsHome(b.GetType()))
				homes.Insert(b);
		}
		for (int pass = 0; pass < 2; pass++)
		{
			for (int t = 0; t < 30 && homes.Count() > 0; t++)
			{
				Building h = homes.GetRandomElement();
				vector bc = h.GetPosition();
				vector door = h.GetDoorSoundPos(Math.RandomInt(0, h.GetDoorCount()));
				vector outv = door - bc;
				outv[1] = 0;
				if (outv.Length() < 1)
					continue;
				outv.Normalize();
				at = door + outv * Math.RandomFloat(4.5, 7.0) + Vector(outv[2], 0, -outv[0]) * Math.RandomFloat(-2.5, 2.5);
				at[1] = GetGame().SurfaceY(at[0], at[2]);
				if (Math.AbsFloat(door[1] - 1.0 - at[1]) > 1.5)
					continue;
				if (pass == 0 && Grassy(at))
					continue;
				if (s_Scenes && Taken(at, 15))
					continue;
				if (PieceClear(at, clear) && PieceClear(at, clear * 0.5) && Flat(at, clear))
					return true;
			}
		}
		return false;
	}

	static bool Taken(vector p, float r)
	{
		foreach (vector q : s_Scenes)
		{
			if (vector.Distance(q, p) < r)
				return true;
		}
		return false;
	}

	static bool Flat(vector p, float r)
	{
		for (int k = 0; k < 4; k++)
		{
			float a = k * Math.PI / 2;
			if (Math.AbsFloat(GetGame().SurfaceY(p[0] + Math.Cos(a) * r, p[2] + Math.Sin(a) * r) - p[1]) > 0.4)
				return false;
		}
		return true;
	}

	static void Note(vector at, string name)
	{
		s_Scenes.Insert(at);
		s_SceneNames.Insert(name);
		string surf;
		GetGame().SurfaceGetType(at[0], at[2], surf);
		Print("[AZ_Vamp] props scene " + name + " at " + at + " on " + surf);
	}


	static void Scene(vector c, int kind, string place, float rMin, float rMax)
	{
		vector at;
		if (!FrontYard(c, rMax + 60, 2.2, at) && !OpenSpot(c, rMin, rMax, 2.2, at))
		{
			Print("[AZ_Vamp] props: no open ground in " + place + " for scene " + kind);
			return;
		}
		float yaw = Math.RandomFloat(0, 360);


		switch (kind)
		{
			case 0:
				Put("AZ_VampShroudedBody_Row", at, yaw, 0, 0);
				Put("AZ_VampCandelabra", at, yaw, 1.3, 1.0);
				Put("AZ_VampQuarantineBarrier", at, yaw, -0.4, 2.4, 5);
				Note(at, place + " plague dead");
				break;
			case 1:
				Put("AZ_VampGibbet", at, yaw, 0, 0);
				Put("AZ_VampVotive", at, yaw, 0.8, 0.7);
				Note(at, place + " gibbet");
				break;
			default:
				Put("AZ_VampSkullPole", at, yaw, 0, 0);
				Put("AZ_VampCandelabra", at, yaw, 1.1, 0.6);
				Note(at, place + " skull pole");
				break;
		}
	}

	static void Quarantine(vector c, string place)
	{
		vector at;
		if (!OpenSpot(c, 30, 90, 2.6, at))
			return;
		float yaw = Math.RandomFloat(0, 360);
		Put("AZ_VampQuarantineBarrier", at, yaw, -1.6, 0);
		Put("AZ_VampQuarantineBarrier", at, yaw, 1.6, 0, 8);
		Put("AZ_VampQuarantineTape", at, yaw, 0, 1.2);
		Note(at, place + " quarantine");
	}




	static void Centrepiece(vector fire, string place)
	{
		vector at;
		if (!OpenSpot(fire, 6, 12, 2.4, at))
		{
			Print("[AZ_Vamp] props: no open ground for the centrepiece at " + place);
			return;
		}
		vector to = fire - at;
		float yaw = to.VectorToAngles()[0];
		Put("AZ_VampGibbet", at, yaw, 0, 0);
		Put("AZ_VampStakedCorpse", at, yaw, 2.2, 0.6, 15);
		Put("AZ_VampSkullPile", at, yaw, -1.9, 0.3, 0);
		Put("AZ_VampRagLine", at, yaw, 0.2, -2.2, 0);
		float yr = yaw * Math.DEG2RAD;
		vector side = Vector(Math.Cos(yr), 0, -Math.Sin(yr));
		vector fwd = Vector(Math.Sin(yr), 0, Math.Cos(yr));
		DecorTorch(at + side * 1.4 + fwd * 1.2, yaw);
		DecorTorch(at - side * 1.4 + fwd * 1.2, yaw);
		Note(at, place + " centrepiece");
	}




	static void Destinations(string base)
	{
		array<string> names = {"Area_A1", "Area_A2", "Area_ARI"};

		array<string> finds = {AZ_VampCostumes.VELVET_GOWN, AZ_VampCostumes.GREATCOAT, AZ_VampCostumes.GRAVE_GOWN};
		foreach (int di, string cls : names)
		{
			TFloatArray pos = new TFloatArray;
			GetGame().ConfigGetFloatArray(base + " " + cls + " position", pos);
			if (pos.Count() < 2)
				continue;
			vector c = Vector(pos[0], GetGame().SurfaceY(pos[0], pos[1]), pos[1]);
			vector at;
			if (!OpenSpot(c, 5, 60, 3.0, at) && !OpenSpot(c, 60, 120, 3.0, at))
			{
				Print("[AZ_Vamp] props: no open ground at the destination " + cls);
				continue;
			}
			float yaw = Math.RandomFloat(0, 360);

			if (AZ_VampState.Cfg().crypt)
				Put("AZ_VampCandleCircle", at, yaw, 0, 0);
			else
				Put("AZ_VampAltarCircle", at, yaw, 0, 0);
			Put("AZ_VampGibbet", at, yaw, 0, 3.2, 180);
			Put("AZ_VampSkullPole", at, yaw, -2.6, -1.8, 20);
			Put("AZ_VampSkullPole", at, yaw, 2.6, -1.8, -20);
			Put("AZ_VampSkullPile", at, yaw, 0, -2.6, 180);
			Put("AZ_VampRagLine", at, yaw, 0, -4.4, 0);
			float yr = yaw * Math.DEG2RAD;
			vector side = Vector(Math.Cos(yr), 0, -Math.Sin(yr));
			vector fwd = Vector(Math.Sin(yr), 0, Math.Cos(yr));
			DecorTorch(at + side * 3.5 + fwd * 3.5, yaw);
			DecorTorch(at - side * 3.5 + fwd * 3.5, yaw);
			DecorTorch(at + side * 3.5 - fwd * 3.5, yaw);
			DecorTorch(at - side * 3.5 - fwd * 3.5, yaw);



			if (!AZ_VampState.Cfg().crypt)
			{
				EntityAI find = AZ_VampCostumes.Place(finds[di], at + fwd * 0.4, Vector(yaw + 90, 0, 0));
				if (find)
				{
					find.SetLifetimeMax(FIND_LIFETIME);
					find.SetLifetime(FIND_LIFETIME);
				}
			}
			Note(at, cls + " destination");
		}
	}

	static const float FIND_LIFETIME = 14400;



	static void ChurchVigil(vector c)
	{
		vector at;
		if (!OpenSpot(c, 8, 25, 3.0, at))
		{
			Print("[AZ_Vamp] props: no open ground at the church " + c);
			return;
		}
		float yaw = Math.RandomFloat(0, 360);
		Put("AZ_VampCandleCircle", at, yaw, 0, 0);
		Put("AZ_VampLampada", at, yaw, 1.6, 1.4);
		Put("AZ_VampLampada", at, yaw, -1.6, 1.4);
		Put("AZ_VampGraveCross_1", at, yaw, -2.4, -2.2, 180);
		Put("AZ_VampGraveCross_2", at, yaw, 2.4, -2.2, 180);
		float yr = yaw * Math.DEG2RAD;
		vector side = Vector(Math.Cos(yr), 0, -Math.Sin(yr));
		vector fwd = Vector(Math.Sin(yr), 0, Math.Cos(yr));
		DecorTorch(at + side * 3.2 + fwd * 2.6, yaw);
		DecorTorch(at - side * 3.2 + fwd * 2.6, yaw);
		Note(at, "church vigil");
	}

	static void Graveyard(vector g)
	{
		vector at;
		if (!OpenSpot(g, 2, 10, 1.6, at))
		{
			Print("[AZ_Vamp] props: no open ground at the graveyard " + g);
			return;
		}
		float yaw = Math.RandomFloat(0, 360);
		Put("AZ_VampCandleCircle", at, yaw, 0, 0);
		Put("AZ_VampLampada", at, yaw, 1.9, -0.9);
		Put("AZ_VampLampada", at, yaw, -2.2, 1.6);
		Put("AZ_VampSkullPole", at, yaw, -3.2, -1.5, 20);
		Put("AZ_VampTombstone_2", at, yaw, -1.0, 3.0, 175);
		float yr = yaw * Math.DEG2RAD;
		DecorTorch(at + Vector(2.8 * Math.Cos(yr), 0, -2.8 * Math.Sin(yr)) + Vector(Math.Sin(yr), 0, Math.Cos(yr)) * 2.6, yaw);
		DecorTorch(at - Vector(2.8 * Math.Cos(yr), 0, -2.8 * Math.Sin(yr)) + Vector(Math.Sin(yr), 0, Math.Cos(yr)) * 2.6, yaw);
		Note(at, "graveyard " + Math.Round(g[0]) + " " + Math.Round(g[2]));
	}
}
