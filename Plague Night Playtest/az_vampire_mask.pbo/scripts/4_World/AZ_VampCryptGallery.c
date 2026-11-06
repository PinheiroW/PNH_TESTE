










class AZ_VampShortcutData
{
	ref array<string> uids;
}

class AZ_VampCryptGallery
{
	static const float SLAB_F = -14.0;
	static const float WALL_R = -6.25;
	static const float BIER_CANDLE_F = 34.5;
	static const float BIER_R = -12.9;
	static const string FILE = "$profile:AZ_Vampire\\crypt_shortcut.json";
	static const int OPEN_MS = 20000;
	static AZ_Crypt_Slab_Short s_Slab;
	static Object s_Bar;
	static const float BAR_R = -6.72;
	static Object s_Eye;
	static bool s_EyeRed;

	static vector BarAt(bool open)
	{
		float z = 1.05;
		if (open)
			z = 2.6;
		return AZ_VampCrypt.Hall(SLAB_F, BAR_R, z);
	}

	static void BarOpen(bool open)
	{
		if (s_Bar)
			s_Bar.SetPosition(BarAt(open));
	}
	static ref AZ_VampShortcutData s_Data;


	static void Build()
	{
		s_Slab = AZ_Crypt_Slab_Short.Cast(AZ_VampCrypt.PutCrypt("AZ_Crypt_Slab_Short", AZ_VampCrypt.Hall(SLAB_F, WALL_R + 0.03), AZ_VampCrypt.HALL_YAW));


		s_Bar = AZ_VampCrypt.PutCrypt("AZ_Crypt_Slab_Bar", BarAt(false), AZ_VampCrypt.HALL_YAW + 90);
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Gallery", AZ_VampCrypt.Hall(0, 0), AZ_VampCrypt.HALL_YAW);
		AZ_VampCrypt.HallProp("AZ_Crypt_Bank_Low", 0.9, -11.95, 0, 90);
		Web("AZ_VampCobwebCorner_L", AZ_VampCrypt.Hall(-14.6, -10.9, 2.1), AZ_VampCrypt.HALL_YAW);
		Web("AZ_VampCobwebCorner_S", AZ_VampCrypt.Hall(6.6, -12.3, 3.1), AZ_VampCrypt.HALL_YAW + 180);
		Load();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, 60000, true);

		string cover = "";
		array<vector> corners = new array<vector>;
		corners.Insert(AZ_VampCrypt.Hall(-15.0, -12.7, 3.5));
		corners.Insert(AZ_VampCrypt.Hall(7.0, -12.7, 3.5));
		corners.Insert(AZ_VampCrypt.Hall(-9.0, -12.7, 3.5));
		foreach (vector c : corners)
			cover += " " + Math.Round(GetGame().SurfaceY(c[0], c[2]) - c[1]);
		Print("[AZ_Vamp] gallery terrain cover over the roof (m):" + cover);
		AZ_VampCryptTomb.Build();
		Print("[AZ_Vamp] gallery built: slab " + (s_Slab != null) + " at " + AZ_VampCrypt.Hall(SLAB_F, WALL_R) + ", " + s_Data.uids.Count() + " unlocked");
	}

	static void Load()
	{
		s_Data = new AZ_VampShortcutData;
		if (FileExist(FILE))
		{
			string err;
			if (!JsonFileLoader<AZ_VampShortcutData>.LoadFile(FILE, s_Data, err))
				s_Data = new AZ_VampShortcutData;
		}
		if (!s_Data.uids)
			s_Data.uids = new array<string>;
	}

	static void Save()
	{
		if (s_Data)
			JsonFileLoader<AZ_VampShortcutData>.JsonSaveFile(FILE, s_Data);
	}


	static void Tick()
	{
		if (!s_Data || s_Data.uids.Count() == 0 || !AZ_VampState.s_Data || AZ_VampState.s_Data.eventActive)
			return;
		s_Data.uids.Clear();
		Save();
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase p : list)
			SetFlag(p, false);
		Print("[AZ_Vamp] gallery: the event is over, shortcut unlocks cleared");
	}

	static bool Unlocked(PlayerBase p)
	{
		if (!s_Data)
			Load();
		return p && s_Data.uids.Find(AZ_VampState.Uid(p)) >= 0;
	}

	static void SetFlag(PlayerBase p, bool on)
	{
		if (!p || p.m_AZShortcut == on)
			return;
		p.m_AZShortcut = on;
		p.SetSynchDirty();
	}


	static void OnConnect(PlayerBase p)
	{
		SetFlag(p, Unlocked(p));
	}


	static int Side(vector pos)
	{
		if (!AZ_VampCrypt.Site())
			return 0;
		vector l = AZ_VampCrypt.HallLocal(pos);
		if (Math.AbsFloat(l[0] - SLAB_F) > 1.0 || l[2] < -0.6 || l[2] > 2.2)
			return 0;
		if (l[1] < WALL_R - 0.6 && l[1] > WALL_R - 2.4)
			return 1;
		if (l[1] > WALL_R + 0.2 && l[1] < WALL_R + 1.9)
			return -1;
		return 0;
	}


	static bool CanLift(PlayerBase p)
	{
		return p && AZ_VampCrypt.EntryOn() && Side(p.GetPosition()) == 1 && !SlabOpen();
	}

	static bool CanPush(PlayerBase p)
	{
		return p && AZ_VampCrypt.EntryOn() && Side(p.GetPosition()) == -1 && !SlabOpen() && (p.m_AZShortcut || p.m_AZVampire);
	}

	static bool SlabOpen()
	{
		if (!s_Slab)
			s_Slab = FindSlab();
		return s_Slab && s_Slab.IsOpen();
	}


	static AZ_Crypt_Slab_Short FindSlab()
	{
		if (!AZ_VampCrypt.Site())
			return null;
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(AZ_VampCrypt.Hall(SLAB_F, WALL_R), 2.0, near, null);
		foreach (Object o : near)
		{
			AZ_Crypt_Slab_Short s = AZ_Crypt_Slab_Short.Cast(o);
			if (s)
				return s;
		}
		return null;
	}

	static void Lift(PlayerBase p)
	{
		if (!CanLift(p) || !s_Slab)
			return;
		s_Slab.OpenFor(OPEN_MS);
		int n = 0;
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		list.Insert(p);
		foreach (PlayerBase q : list)
		{
			if (!q || !q.IsAlive() || (q != p && (Side(q.GetPosition()) != 1 || vector.Distance(q.GetPosition(), p.GetPosition()) > 3.0)))
				continue;
			string uid = AZ_VampState.Uid(q);
			if (s_Data.uids.Find(uid) < 0)
			{
				s_Data.uids.Insert(uid);
				n++;
			}
			SetFlag(q, true);
		}
		Save();
		p.Message("You heave the slab up. Light spills in: the arrival hall, the way you came.", "colorAction");
		Print("[AZ_Vamp] gallery: slab lifted by " + AZ_VampState.Uid(p) + ", " + n + " newly unlocked, " + s_Data.uids.Count() + " in all");
	}

	static void Push(PlayerBase p)
	{
		if (!CanPush(p) || !s_Slab)
			return;
		s_Slab.OpenFor(OPEN_MS);
		p.Message("The slab grinds aside. It will not stay open long.", "colorAction");
		string who = AZ_VampState.Uid(p);
		if (p.m_AZVampire)
			who += " (vampire)";
		Print("[AZ_Vamp] gallery: slab pushed by " + who);
	}


	static void BuildTomb()
	{
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Dig_Tomb", AZ_VampCrypt.Nave(0, 0), AZ_VampCrypt.HALL_YAW);
		AZ_VampCrypt.PutCrypt("AZ_VampShroudedBody", AZ_VampCrypt.Nave(22.4, 10.2, 0.02), AZ_VampCrypt.HALL_YAW + 75);
		AZ_VampCrypt.PutCrypt("AZ_VampShroudedBody", AZ_VampCrypt.Nave(27.2, 13.2, 0.02), AZ_VampCrypt.HALL_YAW + 160);
		Web("AZ_VampCobwebCorner_L", AZ_VampCrypt.Nave(28.0, 9.6, 2.6), AZ_VampCrypt.HALL_YAW + 180);
		Web("AZ_VampCobwebCorner_S", AZ_VampCrypt.Nave(21.7, 14.2, 2.6), AZ_VampCrypt.HALL_YAW);
		Print("[AZ_Vamp] gallery: dig tomb built at " + AZ_VampCrypt.Nave(24.8, 11.9));
	}






	static void SpawnLore()
	{
		vector crack = AZ_VampCryptA1.Crack() + Vector(1.1, 0, 0.7);
		crack[1] = GetGame().SurfaceY(crack[0], crack[2]) + 0.02;
		array<vector> at = new array<vector>;
		at.Insert(AZ_VampCrypt.Nave(21.0, 11.6, 0.03));
		at.Insert(AZ_VampCrypt.Nave(26.2, 12.9, 0.03));
		at.Insert(AZ_VampCrypt.Hall(-15.2, 2.0, 0.03));
		at.Insert(crack);
		at.Insert(AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 6.4, 1.3, AZ_VampCrypt.DAIS_Y + 0.03));
		at.Insert(AZ_VampCrypt.Hall(0.6, -10.8, 0.03));
		int made = 0;
		for (int i = 0; i < at.Count(); i++)
		{
			string cls = "AZ_VampLore_" + (i + 1);
			bool there = false;
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(at[i], 2.0, near, null);
			foreach (Object o : near)
			{
				if (o && o.GetType() == cls)
					there = true;
			}
			if (there)
			{
				Print("[AZ_Vamp] crypt lore: " + cls + " skipped, one already lies there");
				continue;
			}

			Object page = GetGame().CreateObjectEx(cls, at[i], ECE_KEEPHEIGHT);
			if (!page)
			{
				Print("[AZ_Vamp] crypt lore: " + cls + " NULL (class not packed?)");
				continue;
			}
			page.SetPosition(at[i]);
			page.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LoreWhere, 2000, false, page, at[i]);
			made++;
		}
		Print("[AZ_Vamp] crypt lore: " + made + " pages laid (of " + at.Count() + ")");
	}




	static void BuildFrescoes()
	{
		float y = AZ_VampCrypt.HALL_YAW;
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Fresco_Ritual", AZ_VampCrypt.Nave(40.985, -12.05, 1.85), y);

		AZ_VampCrypt.PutCrypt("AZ_Crypt_Fresco_Court", AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 6.6, -7.9, 3.5), y + 90);

		AZ_VampCrypt.PutCrypt("AZ_Crypt_Apse_Band", AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 5, -7.9), y);
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Apse_Band", AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 5, 7.9), y + 180);
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Fresco_Moon", AZ_VampCrypt.Nave(42.0, -7.985, 4.6), y + 90);
	}

	static void LoreWhere(Object page, vector want)
	{
		if (!page)
		{
			Print("[AZ_Vamp] crypt lore: a page is gone 2 s after laying (at " + want + ")");
			return;
		}
		Print("[AZ_Vamp] crypt lore: " + page.GetType() + " laid at " + page.GetPosition() + " (wanted " + want + ", off " + vector.Distance(page.GetPosition(), want) + " m)");
	}

	static bool SealStopped(vector from, vector d)
	{
		vector pos, dir;
		int comp;
		set<Object> hits = new set<Object>;
		DayZPhysics.RaycastRV(from, from + d * 45.0, pos, dir, comp, hits, null, null, true, false, ObjIntersectView);
		foreach (Object ho : hits)
		{
			if (ho)
				return ho.GetType() != "AZ_Crypt_Outer_Shell";
		}
		return false;
	}

	static void AuditSeal()
	{
		array<vector> pts = new array<vector>;
		for (int a = 0; a < 5; a++)
		{
			for (int ar = -1; ar <= 1; ar++)
				pts.Insert(AZ_VampCrypt.Hall(-19.0 + 4.0 * a, 5.0 * ar, 1.2));
		}
		for (int h = 0; h < 9; h++)
		{
			for (int hr = -2; hr <= 2; hr++)
			{
				pts.Insert(AZ_VampCrypt.Nave(6.0 + 6.0 * h, 3.25 * hr, 1.2));
				pts.Insert(AZ_VampCrypt.Nave(6.0 + 6.0 * h, 3.25 * hr, 6.0));
			}
		}
		for (int ap = -1; ap <= 1; ap++)
			pts.Insert(AZ_VampCrypt.Nave(56.0, 3.0 * ap, 2.0));
		pts.Insert(AZ_VampCrypt.Hall(1.0, 0, 1.2));
		if (AZ_VampCrypt.AnteOn())
		{
			for (int g = 0; g < 6; g++)
			{
				float gy = 1.0;
				if (g >= 5)
					gy = 1.9;
				pts.Insert(AZ_VampCrypt.Hall(-13.0 + 3.6 * g, -11.6, gy));
			}
			pts.Insert(AZ_VampCrypt.Hall(-9.0, -8.5, 1.0));
			pts.Insert(AZ_VampCrypt.Hall(-14.0, -8.5, 1.0));
			pts.Insert(AZ_VampCrypt.Nave(6.0, -9.8, 1.0));
			pts.Insert(AZ_VampCrypt.Nave(23.0, 11.5, 1.2));
			pts.Insert(AZ_VampCrypt.Nave(26.5, 12.5, 1.2));
			pts.Insert(AZ_VampCrypt.Nave(18.0, 11.0, 1.2));
			for (int o = 0; o < 3; o++)
				pts.Insert(AZ_VampCrypt.Nave(32.0 + 4.0 * o, -12.0, 1.5));
		}
		array<vector> dirs = new array<vector>;
		dirs.Insert(Vector(1, 0, 0));
		dirs.Insert(Vector(-1, 0, 0));
		dirs.Insert(Vector(0, 1, 0));
		dirs.Insert(Vector(0, -1, 0));
		dirs.Insert(Vector(0, 0, 1));
		dirs.Insert(Vector(0, 0, -1));
		for (int dx = -1; dx <= 1; dx += 2)
		{
			for (int dy = -1; dy <= 1; dy += 2)
			{
				for (int dz = -1; dz <= 1; dz += 2)
					dirs.Insert(Vector(dx, dy, dz).Normalized());
			}
		}
		int leaks = 0;
		int rays = 0;
		foreach (vector from : pts)
		{
			foreach (vector d : dirs)
			{
				rays++;
				vector pos, dir;
				int comp;
				set<Object> hits = new set<Object>;
				DayZPhysics.RaycastRV(from, from + d * 45.0, pos, dir, comp, hits, null, null, true, false, ObjIntersectView);
				string first = "nothing";
				foreach (Object ho : hits)
				{
					if (ho)
					{
						first = ho.GetType();
						break;
					}
				}
				if (first == "AZ_Crypt_Outer_Shell" || first == "nothing")
				{


					vector side = Vector(d[2], 0, -d[0]);
					if (side.Length() < 0.1)
						side = Vector(1, 0, 0);
					side.Normalize();
					if (SealStopped(from + side * 0.05, d) && SealStopped(from - side * 0.05, d))
					{
						Print("[AZ_Vamp] seal seam (not a leak): from " + AZ_VampCrypt.HallLocal(from) + " dir " + d);
						continue;
					}
					leaks++;
					if (leaks <= 40)
						Print("[AZ_Vamp] seal leak: from " + AZ_VampCrypt.HallLocal(from) + " dir " + d + " first " + first + " at " + AZ_VampCrypt.HallLocal(pos));
				}
			}
		}
		Print("[AZ_Vamp] seal audit: " + pts.Count() + " points, " + rays + " rays, " + leaks + " leak");
	}



	static void BuildOssuary()
	{
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Ossuary", AZ_VampCrypt.Nave(0, 0), AZ_VampCrypt.HALL_YAW);
		for (int t = 0; t < 2; t++)
		{
			float z = 1.22 * t;
			for (int k = 0; k < 3; k++)
				AZ_VampCrypt.NaveProp("AZ_Crypt_Ante_Ossuary", 32.5 + 2.6 * k, -14.2, z, -90);
		}


		LooseSkull(33.2, -12.5, 2.0);
		LooseSkull(33.9, -13.3, 2.0);
		LooseSkull(31.8, -13.75, 0.6);
		LooseSkull(35.2, -13.8, 0.6);
		LooseSkull(37.6, -13.7, 0.6);
		LooseSkull(39.0, -13.8, 0.6);


		AZ_VampCrypt.PutCrypt("AZ_Crypt_Ossuary_Screen", AZ_VampCrypt.Nave(36.0, -11.45, 0), AZ_VampCrypt.HALL_YAW + 90);


		AZ_VampCrypt.NaveProp("AZ_Crypt_Bier", 33.5, -12.9, 0, 90);


		AZ_VampCrypt.PutCrypt("AZ_VampVotive", DropOnto(BIER_CANDLE_F, BIER_R, 2.0), AZ_VampCrypt.HALL_YAW);
		Web("AZ_VampCobwebCorner_L", AZ_VampCrypt.Nave(41.0, -14.2, 3.0), AZ_VampCrypt.HALL_YAW + 180);
		AZ_VampCrypt.PutCrypt("AZ_VampBatColony", AZ_VampCrypt.Nave(36.5, -12.05, 5.42), AZ_VampCrypt.HALL_YAW);
		vector gown = AZ_VampCrypt.Nave(40.2, -12.4, 1.05);
		AZ_VampCostumes.Place(AZ_VampCostumes.VELVET_GOWN, gown, Vector(AZ_VampCrypt.HALL_YAW + 90, 0, 0), true);
		Print("[AZ_Vamp] gallery: ossuary built at " + AZ_VampCrypt.Nave(36.0, -12.0));
	}

	static bool InOssuary(vector pos)
	{
		vector l = AZ_VampCrypt.HallLocal(pos);
		return l[0] > 30.5 && l[0] < 41.5 && l[1] > -14.5 && l[1] < -8.6 && l[2] > -0.5 && l[2] < 7.0;
	}





	static vector DropOnto(float f, float r, float up)
	{
		vector from = AZ_VampCrypt.Nave(f, r, up);
		vector to = AZ_VampCrypt.Nave(f, r, -0.2);
		vector hit = AZ_VampCrypt.Nave(f, r, 0);
		vector normal;
		int comp;
		set<Object> results = new set<Object>;
		vector rayHit;
		if (DayZPhysics.RaycastRV(from, to, rayHit, normal, comp, results, null, null, false, false, ObjIntersectGeom))
			hit = rayHit;
		return hit;
	}


	static void LooseSkull(float f, float r, float up)
	{
		Object s = AZ_VampCrypt.PutCrypt("AZ_VampSkull_Human", DropOnto(f, r, up), Math.RandomFloat(0, 360));
		if (s)
			s.SetScale(Math.RandomFloat(0.9, 1.1));
	}

	static void Web(string cls, vector at, float yaw)
	{
		Object o = AZ_VampCrypt.PutCrypt(cls, at, yaw);
		if (o)
			o.SetOrientation(Vector(yaw, 0, 180));
	}

	static bool InTomb(vector pos)
	{
		vector l = AZ_VampCrypt.HallLocal(pos);
		return l[0] > 19.5 && l[0] < 28.3 && l[1] > 9.3 && l[1] < 14.5 && l[2] > -0.5 && l[2] < 4.0;
	}



	static ScriptedLightBase s_Votive;
	static ScriptedLightBase s_Lamp;
	static ScriptedLightBase s_Bones;


	static ref array<ScriptedLightBase> s_Pools;
	static ScriptedLightBase s_BonesPool;



	static void EyeTick(PlayerBase p)
	{
		bool near = p && AZ_VampCrypt.Site() && vector.Distance(p.GetPosition(), AZ_VampCrypt.Hall(SLAB_F, WALL_R)) < 40;
		bool red = p && p.m_AZShortcut;
		if (s_Eye && (!near || red != s_EyeRed))
		{
			GetGame().ObjectDelete(s_Eye);
			s_Eye = null;
		}
		if (!near || s_Eye)
			return;
		string cls = "AZ_Crypt_Slab_Eye";
		if (red)
			cls = "AZ_Crypt_Slab_Eye_Red";
		s_Eye = GetGame().CreateObjectEx(cls, AZ_VampCrypt.Hall(SLAB_F, WALL_R + 0.01, 2.55), ECE_LOCAL);
		if (s_Eye)
			s_Eye.SetOrientation(Vector(AZ_VampCrypt.HALL_YAW - 90, 0, 0));
		s_EyeRed = red;
	}



	static ref array<ScriptedLightBase> s_FrescoPools;
	static void FrescoTick(PlayerBase p)
	{
		float ap = AZ_VampCrypt.APSE_F;
		vector moon = AZ_VampCrypt.Nave(42.0, -7.2, 3.2);
		vector court = AZ_VampCrypt.Nave(ap + 6.6, -7.2, 2.2);
		bool want = p && AZ_VampCrypt.Site() && (vector.Distance(p.GetPosition(), moon) < 20 || vector.Distance(p.GetPosition(), court) < 20);
		if (want && !s_FrescoPools)
		{
			s_FrescoPools = new array<ScriptedLightBase>;
			s_FrescoPools.Insert(AZ_VampLightGate.Light(AZ_VampFrescoPool, moon));
			s_FrescoPools.Insert(AZ_VampLightGate.Light(AZ_VampFrescoPool, court));
		}
		else if (!want && s_FrescoPools)
		{
			foreach (ScriptedLightBase fp : s_FrescoPools)
			{
				if (fp)
					fp.Destroy();
			}
			s_FrescoPools = null;
		}
	}

	static bool InGallery(vector pos)
	{
		vector l = AZ_VampCrypt.HallLocal(pos);
		return l[0] > -15.2 && l[0] < 7.2 && l[1] > -12.9 && l[1] < -6.9 && l[2] > -0.8 && l[2] < 3.8;
	}

	static void ClientTick(PlayerBase p)
	{
		EyeTick(p);
		bool want = p && AZ_VampCrypt.Site() && InGallery(p.GetPosition());
		if (want && !s_Votive)
		{
			s_Votive = AZ_VampLightGate.Light(AZ_VampGalleryVotive, AZ_VampCrypt.Hall(0.9, -11.7, 0.45));
			s_Pools = new array<ScriptedLightBase>;

			s_Pools.Insert(AZ_VampLightGate.Light(AZ_VampBunkPool, AZ_VampCrypt.Hall(-12.4, -10.6, 0.5)));


			s_Pools.Insert(AZ_VampLightGate.Light(AZ_VampGalleryVotive, AZ_VampCrypt.Hall(AZ_VampCryptTomb.VOTIVE_F, AZ_VampCryptTomb.VOTIVE_R, 0.25)));
			Print("[AZ_Vamp] gallery votive on " + (s_Votive != null) + " pools " + s_Pools.Count());
		}
		else if (!want && s_Votive)
		{
			s_Votive.Destroy();
			s_Votive = null;
			foreach (ScriptedLightBase gp : s_Pools)
			{
				if (gp)
					gp.Destroy();
			}
			s_Pools = null;
		}
		bool bones = p && AZ_VampCrypt.Site() && AZ_VampCrypt.AnteOn() && InOssuary(p.GetPosition());
		if (bones && !s_Bones)
		{
			s_Bones = AZ_VampLightGate.Light(AZ_VampOssuaryLight, DropOnto(BIER_CANDLE_F, BIER_R, 2.0) + Vector(0, 0.2, 0));

		}
		else if (!bones && s_Bones)
		{
			s_Bones.Destroy();
			s_Bones = null;
			if (s_BonesPool)
				s_BonesPool.Destroy();
			s_BonesPool = null;
		}
		FrescoTick(p);
		bool lamp = p && AZ_VampCrypt.Site() && AZ_VampCrypt.AnteOn() && InTomb(p.GetPosition());
		if (lamp && !s_Lamp)
			s_Lamp = AZ_VampLightGate.Light(AZ_VampTombLamp, AZ_VampCrypt.Nave(26.8, 13.4, 0.35));
		else if (!lamp && s_Lamp)
		{
			s_Lamp.Destroy();
			s_Lamp = null;
		}
	}
}











class AZ_Crypt_Slab_Short extends AZ_Crypt_Slab_Door
{
	override void SetOpen(bool open)
	{
		super.SetOpen(open);
		if (GetGame().IsServer())
			AZ_VampCryptGallery.BarOpen(open);
	}

	void OpenFor(int ms)
	{
		SetOpen(true);
		m_CloseAt = GetGame().GetTime() + ms;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CheckClose, ms, false);
	}

	override protected void CheckClose()
	{
		if (!m_Open)
			return;
		if (GetGame().GetTime() < m_CloseAt || InDoorway())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CheckClose, 2000, false);
			return;
		}
		SetOpen(false);
		Print("[AZ_Vamp] gallery: slab shut");
	}

	protected bool InDoorway()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		vector hole = AZ_VampCrypt.Hall(AZ_VampCryptGallery.SLAB_F, AZ_VampCryptGallery.WALL_R - 0.4, 1.0);
		foreach (PlayerBase p : list)
		{
			if (p && vector.Distance(p.GetPosition() + "0 1 0", hole) < 1.1)
				return true;
		}
		return false;
	}
}

class ActionAZGalleryLift : ActionInteractBase
{
	void ActionAZGalleryLift()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Lift the slab";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return AZ_VampCryptGallery.CanLift(player);
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampCryptGallery.Lift(action_data.m_Player);
	}
}

class ActionAZGalleryPush : ActionInteractBase
{
	void ActionAZGalleryPush()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Push the slab";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return AZ_VampCryptGallery.CanPush(player);
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampCryptGallery.Push(action_data.m_Player);
	}
}

modded class PlayerBase
{
	bool m_AZShortcut;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZShortcut");
	}

	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZGalleryLift, InputActionMap);
		AddAction(ActionAZGalleryPush, InputActionMap);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZGalleryLift);
		actions.Insert(ActionAZGalleryPush);
	}
}






