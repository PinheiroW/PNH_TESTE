













class AZ_VampCrypt
{


	static string s_World;
	static float DOOR_X;
	static float DOOR_Z;
	static float DOOR_YAW;
	static float HALL_X;
	static float HALL_Z;
	static float HALL_FLOOR;
	static float HALL_YAW;
	static const float DOOR_FRONT = 7.9;
	static const float ROOM_FACE = 10.453;
	static const float HALF_LEN = 20.48;
	static const float HALF_W = 6.1;
	static const float ARRIVE = 18.0;
	static const float ARRIVE_R = -4.0;
	static const float LEG1 = -15.5;
	static const float LEG2 = -12.5;




	static const float LEDGE_H = -0.9;
	static const float BAY = 6.0;
	static const int BAYS = 8;
	static const string PILLARS_L = "ACBDXADC";
	static const string PILLARS_R = "BDACBXCA";
	static const float HALL_F0 = 3.0;
	static const float APSE_F = 51.0;
	static const float NAVE_DEPTH = 1.0;
	static const float RAISE = 3.0;
	static const float WALK_R = 3.75;
	static const float AISLE_R = 6.75;
	static const float PILLAR_R = 4.2;
	static const float ALTAR_F = 57.0;
	static const float DAIS_Y = 0.6;
	static const float PLINTH_TOP = 1.2;
	static const float STEP_TOP_F = 0.0;
	static const float DRUM_TOP_F = 0.9;
	static const float COUNTESS_SCALE_F = 1.8;
	static const float DRUM_SCALE = 1.5;
	static const float DRUM_ALTAR_SCALE = 1.58;
	static const float COUNTESS_SCALE = 1.5;
	static const float MOON_H = 15.94;
	static const float BLOOD_MIN = 0.5;
	static const float BLOOD_MAX = 0.6;
	static const float BLOOD_STEP = 0.01;


	static const int DOOR_BAY = 2;
	static const float SKULL_X = -1.5;
	static const float SKULL_Y = -1.15;
	static const float SKULL_Z = 1.57;
	static AZ_Crypt_Slab_Door s_Slab;
	static const int GRACE_MS = 5000;
	static const int HIT_NO_GRACE_MS = 15000;
	static const int GRACE_COOLDOWN_MS = 60000;
	static const int WATCH_MS = 5000;
	static const int MOVE_MS = 500;
	static const int FADE_MS = 1500;
	static const int RPC_FADE = 4702232;

	static bool s_Built;
	static int s_Count;


	// Read-only readiness check for observers; never initializes site geometry.
	static bool IsReady()
	{
		return s_World != "" && HALL_FLOOR != 0;
	}

	static bool Site()
	{
		if (s_World == "")
		{
			GetGame().GetWorldName(s_World);
			s_World.ToLower();
			if (s_World == "namalsk")
			{







				DOOR_X = 5493.76;
				DOOR_Z = 8199.41;
				DOOR_YAW = -20.33;
				HALL_X = 7609.9;
				HALL_Z = 8854.1;
				HALL_FLOOR = 24.1;
				HALL_YAW = 135;
			}
		}
		return HALL_FLOOR != 0;
	}

	static vector Fwd(float yaw)
	{
		float a = yaw * Math.DEG2RAD;
		return Vector(Math.Sin(a), 0, Math.Cos(a));
	}

	static vector Side(float yaw)
	{
		float a = yaw * Math.DEG2RAD;
		return Vector(Math.Cos(a), 0, -Math.Sin(a));
	}


	static vector DoorOrigin()
	{

		vector fwd = Fwd(DOOR_YAW);
		float fx = DOOR_X - fwd[0] * DOOR_FRONT;
		float fz = DOOR_Z - fwd[2] * DOOR_FRONT;
		return Vector(DOOR_X, DoorFootMin(fx, fz) - DOOR_SINK, DOOR_Z);
	}



	static const float DOOR_SINK = 0.25;
	static float s_DoorFootMin = -9999;
	static float DoorFootMin(float fx, float fz)
	{
		if (s_DoorFootMin > -9999)
			return s_DoorFootMin;
		vector fwd = Fwd(DOOR_YAW);
		vector side = Side(DOOR_YAW);
		float lo = 9999;
		for (float u = -2.1; u <= 2.11; u += 0.5)
		{
			for (float v = 0; v <= 3.51; v += 0.5)
			{
				float qx = fx + side[0] * u + fwd[0] * v;
				float qz = fz + side[2] * u + fwd[2] * v;
				float qy = GetGame().SurfaceY(qx, qz);
				if (qy < lo)
					lo = qy;
			}
		}
		s_DoorFootMin = lo;
		return lo;
	}


	static vector Door(float x, float z, bool onGround = false)
	{
		vector p = DoorOrigin();
		vector side = Side(DOOR_YAW);
		vector fwd = Fwd(DOOR_YAW);
		p = p + side * x;
		p = p + fwd * z;
		if (onGround)
			p[1] = GetGame().SurfaceY(p[0], p[2]);
		return p;
	}

	static vector Mouth()
	{
		vector m = Door(0, -DOOR_FRONT - 1.6);
		m[1] = m[1] + 0.2;
		return m;
	}


	static vector Hall(float f, float r, float y = 0)
	{
		vector p = Vector(HALL_X, HALL_FLOOR + y, HALL_Z);
		vector fwd = Fwd(HALL_YAW);
		vector side = Side(HALL_YAW);
		p = p + fwd * f;
		p = p + side * r;
		return p;
	}


	static vector Nave(float f, float r, float y = 0)
	{
		return Hall(f, r, y - LEDGE_H);
	}

	static bool Inside(vector pos)
	{
		if (!Site())
			return false;
		vector d = pos - Vector(HALL_X, HALL_FLOOR, HALL_Z);
		vector fwd = Fwd(HALL_YAW);
		vector side = Side(HALL_YAW);
		float f = vector.Dot(d, fwd);
		float r = vector.Dot(d, side);
		if (f <= -HALF_LEN - 3 || f >= APSE_F + 13 || Math.AbsFloat(r) >= 14.5 || d[1] <= -LEDGE_H - 2.5 || d[1] >= 17.5 - LEDGE_H)
			return false;
		return pos[1] < GetGame().SurfaceY(pos[0], pos[2]) - 2;
	}


	static vector HallLocal(vector pos)
	{
		vector d = pos - Vector(HALL_X, HALL_FLOOR, HALL_Z);
		return Vector(vector.Dot(d, Fwd(HALL_YAW)), vector.Dot(d, Side(HALL_YAW)), d[1]);
	}

	static Object s_DigChamber;



	static vector RadioPos()
	{
		float fc = HALL_F0 + BAY * (DOOR_BAY + 0.5);
		vector p = Nave(fc, AISLE_R, 1.0);
		return p - Side(HALL_YAW + 180) * 4.0;
	}



	static vector DigChamberLoot()
	{
		if (!s_DigChamber)
		{
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(RadioPos(), 7.0, near, null);
			foreach (Object candidate : near)
			{
				if (candidate && (candidate.GetType() == "AZ_Crypt_Chamber_Breach" || candidate.GetType() == "AZ_Crypt_Chamber"))
				{
					s_DigChamber = candidate;
					break;
				}
			}
		}
		if (s_DigChamber && s_DigChamber.MemoryPointExists("loot"))
			return s_DigChamber.ModelToWorld(s_DigChamber.GetMemoryPointPos("loot"));
		Print("[AZ_Vamp] crypt loot: dig chamber asset or loot memory point missing");
		return vector.Zero;
	}

	static bool AtDoor(vector pos)
	{
		if (!Site())
			return false;
		vector m = Mouth();
		if (vector.Distance(pos, m) >= 4.0)
			return false;
		return DoorStands();
	}


	static int s_DoorCheckAt;
	static bool s_DoorStands;
	static bool DoorStands()
	{
		if (GetGame().IsServer())
			return s_Built && TombDoorOn();
		int now = GetGame().GetTime();
		if (now < s_DoorCheckAt)
			return s_DoorStands;
		s_DoorCheckAt = now + 1000;
		s_DoorStands = false;
		vector origin = DoorOrigin();
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(origin, 3, objs, null);
		foreach (Object o : objs)
		{
			if (o && (o.IsKindOf("AZ_Crypt_Portal") || o.IsKindOf("Land_Underground_Entrance")))
			{
				s_DoorStands = true;
				break;
			}
		}
		return s_DoorStands;
	}

	static bool AtGate(vector pos)
	{
		if (!Inside(pos))
			return false;
		vector d = pos - Vector(HALL_X, HALL_FLOOR, HALL_Z);
		vector fwd = Fwd(HALL_YAW);
		return vector.Dot(d, fwd) < -HALF_LEN + 4.5;
	}


	static Object PutCrypt(string type, vector at, float yaw)
	{
		Object o = GetGame().CreateObjectEx(type, at, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_NOLIFETIME);
		if (!o)
		{
			Print("[AZ_Vamp] crypt: could not spawn " + type);
			return null;
		}
		o.SetPosition(at);
		o.SetOrientation(Vector(yaw, 0, 0));
		s_Count++;

		Print("[AZ_Vamp] place " + type + " " + o.GetShapeName() + " " + o.GetPosition() + " " + o.GetOrientation());
		return o;
	}



	static Object HallProp(string type, float f, float r, float y, float face)
	{
		vector at = Hall(f, r, y);
		return Audited(PutCrypt(type, at, HALL_YAW + face + 180));
	}




	static ref array<Object> s_Audit;

	static Object Audited(Object o)
	{
		if (!o)
			return o;
		string t = o.GetType();
		if (t.Contains("Fire_Bowl") || t.Contains("Brazier") || t.Contains("Bank_") || t.Contains("Votive"))
		{
			if (!s_Audit)
				s_Audit = new array<Object>;
			s_Audit.Insert(o);
		}
		return o;
	}

	static void AuditProps()
	{
		if (!s_Audit)
			return;
		int bad = 0;
		foreach (Object o : s_Audit)
		{
			vector mm[2];
			if (!o.GetCollisionBox(mm))
				continue;
			vector size = (mm[1] - mm[0]) * 0.8;
			size[1] = Math.Max(0.05, size[1] - 0.2);
			vector mid = (mm[0] + mm[1]) * 0.5;
			mid[1] = mid[1] + 0.1;
			array<Object> excl = new array<Object>;
			excl.Insert(o);
			array<Object> hit = new array<Object>;
			GetGame().IsBoxCollidingGeometry(o.ModelToWorld(mid), o.GetOrientation(), size, ObjIntersectGeom, ObjIntersectView, excl, hit);
			string into = "";
			foreach (Object h : hit)
			{
				if (h && s_Audit.Find(h) < 0 && h.GetType().IndexOf("AZ_Crypt_") == 0)
					into += " " + h.GetType();
			}
			if (into != "")
			{
				bad++;
				Print("[AZ_Vamp] clip: " + o.GetType() + " at " + HallLocal(o.GetPosition()) + " into" + into);
			}
		}
		Print("[AZ_Vamp] placement audit: " + s_Audit.Count() + " props, " + bad + " clip");
	}


	static Object DoorProp(string type, float x, float z, float face, bool onGround, float up = 0)
	{
		vector at = Door(x, z, onGround);
		at[1] = at[1] + up;
		return PutCrypt(type, at, DOOR_YAW + face + 180);
	}





	static void SpookySweep()
	{
		vector hc = Hall(20, 0);
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(hc, 200, near, null);
		foreach (Object o : near)
		{
			if (!o || o.GetType().IndexOf("Spooky") != 0)
				continue;
			vector op = o.GetPosition();
			vector flat = Vector(op[0], hc[1], op[2]);
			float d = vector.Distance(flat, hc);
			EffectArea ea = EffectArea.Cast(o);
			float reach = 0;
			if (ea)
				reach = ea.m_Radius;
			if (d > reach + 60)
				continue;
			Print("[AZ_Vamp] crypt spooky sweep: removed " + o.GetType() + " at " + op + " radius " + reach + " (" + d + " m from the crypt centre)");
			if (ea && ea.m_Trigger)
				GetGame().ObjectDelete(ea.m_Trigger);
			GetGame().ObjectDelete(o);
		}
	}

	static void Build()
	{
		if (s_Built || !Site() || !AZ_VampState.Cfg().crypt)
			return;
		s_Built = true;
		s_Count = 0;
		BuildDoor();
		BuildHall();
		AZ_VampCryptA1.Build();
		AZ_VampCryptA3.Build();
		AZ_VampCryptBlood.Restock();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Watch, WATCH_MS, true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpookySweep, 20000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpookySweep, 60000, true);
		vector mouth = Mouth();
		vector altar = Nave(ALTAR_F, 0, DAIS_Y);
		vector origin = DoorOrigin();
		AZ_VampProps.Note(mouth, "crypt door");
		AZ_VampProps.Note(altar, "crypt altar");

		vector inHall = Nave(30, 3.5, 1);
		vector above = Hall(-10, 0);
		above[1] = GetGame().SurfaceY(above[0], above[2]) + 0.1;
		vector arrive = Hall(-ARRIVE, ARRIVE_R, 0.1);

		int ugN = -1;
		int ugCar = 0;
		if (UndergroundAreaLoader.m_JsonData && UndergroundAreaLoader.m_JsonData.Triggers)
			ugN = UndergroundAreaLoader.m_JsonData.Triggers.Count();
		array<Object> ugNear = new array<Object>;
		GetGame().GetObjectsAtPosition3D(Vector(7624, 33, 8840), 30, ugNear, null);
		foreach (Object ugO : ugNear)
		{
			if (ugO && ugO.IsKindOf("UndergroundTriggerCarrier"))
				ugCar++;
		}
		Print("[AZ_Vamp] crypt underground: server json triggers " + ugN + ", carriers at the crypt " + ugCar);
		Print("[AZ_Vamp] crypt check: inside hall " + Inside(inHall) + ", inside on the plain above " + Inside(above) + ", mouth at door " + AtDoor(mouth) + ", arrival at gate " + AtGate(arrive) + ", river level " + s_Blood);

		vector dais = Nave(ALTAR_F, 0, DAIS_Y + 0.1);
		vector plinth = Nave(CountessF(), 0, CountessFeet());
		Print("[AZ_Vamp] crypt check: inside dais " + Inside(dais) + " (surface " + GetGame().SurfaceY(dais[0], dais[2]) + ", y " + dais[1] + "), inside plinth " + Inside(plinth) + " (surface " + GetGame().SurfaceY(plinth[0], plinth[2]) + ", y " + plinth[1] + ")");


		for (int ch = 0; ch < 3; ch++)
		{
			float chH = 0.6 + ch * 1.2;
			vector chFrom = Nave(CountessF() - 2.5, 0, CountessFeet() + chH);
			vector chTo = Nave(CountessF() + 0.5, 0, CountessFeet() + chH);
			vector chHit, chNorm;
			int chComp;
			set<Object> chRes = new set<Object>;
			bool chGot = DayZPhysics.RaycastRV(chFrom, chTo, chHit, chNorm, chComp, chRes, null, null, false, false, ObjIntersectGeom);
			bool chHer = chGot && s_Countess && chRes.Find(s_Countess) >= 0;
			Print("[AZ_Vamp] crypt check: countess collision at +" + chH + " m: hit " + chGot + " on her " + chHer + ", " + vector.Distance(chHit, chTo) + " m in front of her far side (scale " + CountessScale() + ")");
		}


		if (FileExist("$profile:AZ_Vampire\\audit.txt"))
		{
			AuditProps();
			AZ_VampCryptGallery.AuditSeal();
		}
		s_Audit = null;
		Print("[AZ_Vamp] crypt built: " + s_Count + " objects, door " + origin + " mouth " + mouth + ", altar " + altar + " under terrain " + GetGame().SurfaceY(altar[0], altar[2]));
	}

	static void BuildDoor()
	{
		AZ_VampDoorSurvey.Arm();



		if (!TombDoorOn())
		{
			Print("[AZ_Vamp] crypt door: tombDoor false, no outside tomb door built");
			return;
		}
		Print("[AZ_Vamp] crypt door: tombDoor true, the outside tomb door at " + DOOR_X + " " + DOOR_Z);
		vector origin = DoorOrigin();


		if (!AZ_VampState.Cfg() || AZ_VampState.Cfg().cryptPortal)
		{



			PutCrypt("AZ_Crypt_Vault", origin, DOOR_YAW);
			vector grilleAt = origin + Vector(0, 0.92, 0);
			AZ_Crypt_Tomb_Grille grille = AZ_Crypt_Tomb_Grille.Cast(PutCrypt("AZ_Crypt_Tomb_Grille", grilleAt, DOOR_YAW));
			if (grille)
				grille.AZ_SetExitOnly(!EntryOn());
			Print("[AZ_Vamp] crypt door: exit only " + !EntryOn() + " (cryptEntrance " + EntryOn() + ")");
		}
		else
		{
			Building door = Building.Cast(PutCrypt("Land_Underground_Entrance", origin, DOOR_YAW));
			if (door)
			{
				for (int i = 0; i < door.GetDoorCount(); i++)
				{
					door.CloseDoor(i);
					door.LockDoor(i, true);
				}
			}
		}
		float ax = DOOR_FRONT;

		DoorProp("AZ_VampCandelabra", -3.3, -ax - 1.3, 180, false);
		DoorProp("AZ_VampCandelabra", 3.3, -ax - 1.3, 180, false);
		DoorProp("AZ_VampSkullPile", -1.6, -7.7, 160, false, 0);
		DoorProp("AZ_VampDecal_BloodDrops", 0.3, -ax - 1.0, 0, false);
		DoorProp("AZ_VampDecal_BloodDrops", -0.4, -ax - 2.2, 40, false);
		DoorProp("AZ_VampDecal_BloodDrops", 0.6, -ax - 3.4, 80, false);

		DoorProp("AZ_VampLampada", -6.0, -ax - 5.9, 180, true);
		DoorProp("AZ_VampLampada", 6.0, -ax - 5.9, 180, true);
		DoorProp("AZ_VampGraveCross_1", -8.6, -ax - 3.9, 170, true);
		DoorProp("AZ_VampGraveCross_2", 9.2, -ax - 4.7, 195, true);
		DoorProp("AZ_VampTombstone_1", -11.0, -ax - 6.6, 185, true);
		DoorProp("AZ_VampGrave_Broken", 7.4, -ax - 7.6, 175, true);
		DoorProp("AZ_VampSkullPole", -5.6, -ax - 5.1, 200, true);

		vector t1 = Door(-6.0, 14.0, true);
		vector t2 = Door(6.0, 14.0, true);
		AZ_VampTorch.Plant(t1, DOOR_YAW + 180);
		AZ_VampTorch.Plant(t2, DOOR_YAW + 180);
	}


	static Object NaveProp(string type, float f, float r, float y, float face)
	{
		vector at = Nave(f, r, y);
		return Audited(PutCrypt(type, at, HALL_YAW + face + 180));
	}


	static float s_Blood;
	static float BloodLevel()
	{
		float lvl = AZ_VampState.s_Data.cryptBlood;
		return Math.Clamp(lvl, BLOOD_MIN, BLOOD_MAX);
	}

	static void BloodRise()
	{
		float lvl = BloodLevel() + BLOOD_STEP;
		AZ_VampState.s_Data.cryptBlood = Math.Min(lvl, BLOOD_MAX);
		AZ_VampState.Save();
		Print("[AZ_Vamp] crypt river rises to " + AZ_VampState.s_Data.cryptBlood + " (shows after the next restart)");
	}

	static void Robe(string cls, float f, float r)
	{
		vector at = Nave(f, r, DAIS_Y + 0.15);
		AZ_VampCostumes.Place(cls, at, Vector(HALL_YAW + 90, 0, 0), true);
	}

	static Object s_DoorWall;


	static void BuildDoorBay(float fc)
	{
		float yaw = HALL_YAW + 180;
		Object wall = PutCrypt("AZ_Crypt_Wall_Bay_Door_Rev", Nave(fc, AISLE_R), yaw);
		s_DoorWall = wall;
		if (!wall)
			return;
		vector door = wall.ModelToWorld("-1.25 0 0");
		if (wall.MemoryPointExists("door_pos"))
			door = wall.ModelToWorld(wall.GetMemoryPointPos("door_pos"));
		vector side = Side(yaw);
		s_Slab = AZ_Crypt_Slab_Door.Cast(PutCrypt("AZ_Crypt_Slab_Door", door, yaw));
		vector room = door - side * 0.75;
		if (AnteOn())
		{

			s_DigChamber = PutCrypt("AZ_Crypt_Chamber_Breach", room, yaw);
			AZ_VampCryptGallery.BuildTomb();
		}
		else
			s_DigChamber = PutCrypt("AZ_Crypt_Chamber", room, yaw);
		vector back = room - side * 4.0;
		PutCrypt("AZ_VampShroudedBody", back, yaw + 90);
		vector camp = wall.GetPosition() - side * 4.4 + Fwd(yaw) * 0.8;
		PutCrypt("AZ_Crypt_Campfire_Dead", camp, yaw);


		vector coat = wall.GetPosition() - side * 5.2 + Fwd(yaw) * 1.3;
		coat[1] = coat[1] + 0.15;
		AZ_VampCostumes.Place(AZ_VampCostumes.GREATCOAT, coat, Vector(yaw + 90, 0, 0), true);
		Print("[AZ_Vamp] crypt door: wall " + wall.GetPosition() + " door " + door + " slab " + (s_Slab != null) + " room " + room);
	}


	static vector SkullPos()
	{
		float fc = HALL_F0 + BAY * (DOOR_BAY + 0.5);
		vector wallAt = Nave(fc, AISLE_R);
		float yaw = HALL_YAW + 180;
		vector side = Side(yaw);
		vector fwd = Fwd(yaw);
		vector p = wallAt + side * SKULL_X;
		p = p + fwd * SKULL_Y;
		p[1] = p[1] + SKULL_Z;
		return p;
	}

	static bool AtSkull(PlayerBase p)
	{
		if (!Site() || !p)
			return false;
		vector sk = SkullPos();
		vector at = p.GetPosition();
		at[1] = at[1] + 1.5;
		return vector.Distance(at, sk) < 1.7;
	}

	static void PressSkull(PlayerBase p)
	{
		if (!AtSkull(p))
			return;
		if (!s_Slab)
		{
			Print("[AZ_Vamp] crypt skull pressed, but no slab");
			return;
		}
		if (s_Slab.IsOpen())
			return;
		s_Slab.SetOpen(true);
		p.Message("Stone grinds on stone. The wall beside you slides open.", "colorAction");
		Print("[AZ_Vamp] crypt door opened by " + AZ_VampState.Uid(p));
	}




	static AZ_Crypt_Countess s_Countess;



	static const string ALTAR = "AZ_Crypt_Altar_Rodin";

	static string AltarClass()
	{
		if (FileExist("$profile:AZ_Vampire/carved_altar.txt"))
			return "AZ_Crypt_Altar";
		return ALTAR;
	}




	static bool LayoutF()
	{
		if (FileExist("$profile:AZ_Vampire/altar_F.txt"))
			return true;
		return AZ_VampState.Cfg() && AZ_VampState.Cfg().altarLayout == "F";
	}


	static float CountessScale()
	{
		if (LayoutF())
			return COUNTESS_SCALE_F;
		return COUNTESS_SCALE;
	}

	static float CountessF()
	{
		if (LayoutF())
			return APSE_F + 9.2;
		return APSE_F + 8.6;
	}


	static float CountessFeet()
	{
		if (LayoutF())
			return DAIS_Y + STEP_TOP_F;
		return DAIS_Y + PLINTH_TOP;
	}

	static void StatueEyes(vector altarPos, bool on)
	{
		if (s_Countess)
			s_Countess.AZ_SetEyes(on);
		Print("[AZ_Vamp] countess eyes " + on + " (statue " + (s_Countess != null) + ")");
	}



	static int CfgBitsNow()
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		int b = 16;
		if (c.cryptEntrance)
			b = b | 1;
		if (c.cryptAnte)
			b = b | 2;
		if (c.tombDoor)
			b = b | 4;
		if (c.portalNeedsKey)
			b = b | 8;
		return b;
	}


	static int ClientBits()
	{
		if (GetGame().IsServer())
			return 0;
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (me && (me.m_AZCfgBits & 16))
			return me.m_AZCfgBits;
		return 0;
	}

	static bool AnteOn()
	{
		int cb = ClientBits();
		if (cb)
			return (cb & 2) != 0;
		return !AZ_VampState.Cfg() || AZ_VampState.Cfg().cryptAnte;
	}



	static bool TombDoorOn()
	{
		int cb = ClientBits();
		if (cb)
			return (cb & 4) != 0;
		return !AZ_VampState.Cfg() || AZ_VampState.Cfg().tombDoor;
	}


	static bool IsGuest(PlayerBase p)
	{
		return AZ_VampGuest.IsGuest(p);
	}

	static bool EntryOn()
	{
		int cb = ClientBits();
		if (cb)
			return (cb & 1) != 0;
		return !AZ_VampState.Cfg() || AZ_VampState.Cfg().cryptEntrance;
	}

	static void BuildHall()
	{
		float L = HALF_LEN;
		float W = HALF_W;
		s_Blood = BloodLevel();




		if (AnteOn())
		{


			for (int a = 0; a < 4; a++)
			{
				string bay = "AZ_Crypt_Ante_Bay";
				if (a == 1)
					bay = "AZ_Crypt_Ante_Bay_Slab";
				else if (a == 2)
					bay = "AZ_Crypt_Ante_Bay_Nook";
				PutCrypt(bay, Hall(-21.0 + 5.25 * (a + 0.5), 0), HALL_YAW);
			}
			AZ_VampCryptGallery.Build();
			AZ_VampCryptGallery.BuildOssuary();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptGallery.SpawnLore, 5000, false);
			AZ_VampCryptGallery.BuildFrescoes();
			PutCrypt("AZ_Crypt_Ante_End", Hall(-L, 0), HALL_YAW);
			PutCrypt("AZ_Crypt_Ante_Leg", Hall(LEG1, -3.25), HALL_YAW);
			PutCrypt("AZ_Crypt_Ante_Leg", Hall(LEG1 + 0.06, 0.5), HALL_YAW);
			PutCrypt("AZ_Crypt_Ante_Leg", Hall(LEG2, 2.8), HALL_YAW);
			if (EntryOn())
			{


				for (int m = 0; m < 2; m++)
				{
					float mf = -19.4 + 1.8 * m;
					PutCrypt("AZ_Crypt_Ante_Mirror", Hall(mf, -HALF_W - 0.13), HALL_YAW + 90);
					PutCrypt("AZ_Crypt_Ante_Mirror", Hall(mf, HALF_W + 0.13), HALL_YAW - 90);


					if (m == 1)
						HallProp("AZ_Crypt_Bank_Low", mf, 5.6, 0, -90);
				}
				PutCrypt("AZ_Crypt_Ante_Rug", Hall(-19.0, 0), HALL_YAW);
				HallProp("AZ_Crypt_Brazier", -16.1, 0, 0, 0);


				HallProp("AZ_Crypt_Wolf", -19.75, -2.5, 0, 168);
				HallProp("AZ_Crypt_Wolf", -19.75, 2.5, 0, 192);
			}
		}
		else
		{
			PutCrypt("Land_Underground_Storage_Big", Hall(-ROOM_FACE, 0), HALL_YAW - 180);

			PutCrypt("StaticObj_Wall_IndCnc_4", Hall(LEG1, -3.1, 1.75), HALL_YAW);
			PutCrypt("StaticObj_Wall_IndCnc_4", Hall(LEG1, 0.5, 1.75), HALL_YAW);
			PutCrypt("StaticObj_Wall_IndCnc_4", Hall(LEG2, 2.8, 1.75), HALL_YAW);
		}
		if (AnteOn() && EntryOn())
		{

			PutCrypt("AZ_Crypt_Ante_Portcullis", Hall(-L - 0.4, 0), HALL_YAW);

			PutCrypt("AZ_Crypt_Ante_Niche", Hall(-15.7, 0), HALL_YAW);

			HallProp("AZ_Crypt_Bank_Low", -9.6, W - 0.45, 0, -90);
		}
		else
		{
			PutCrypt("StaticObj_Underground_Corridor_Main_Gate_L", Hall(-L + 0.18, -1.8), HALL_YAW);
			PutCrypt("StaticObj_Underground_Corridor_Main_Gate_R", Hall(-L + 0.18, 1.8, 2.316), HALL_YAW);
		}

		if (EntryOn())
		{
			HallProp("AZ_VampCoffin_Closed", -18.6, W - 0.7, 0, 180);
			HallProp("AZ_VampSkull_Pile", -10.6, W - 0.7, 0, -90);
			HallProp("AZ_VampCoffin_Cross", -8.0, W - 0.8, 0, 180);
			HallProp("AZ_VampShroudedBody_Row", -8.8, -W + 1.3, 0, 90);
			HallProp("AZ_VampSkull_Pile", -10.3, -W + 0.7, 0, 90);
			HallProp("AZ_VampCoffin_Open", -6.3, -W + 0.8, 0, 180);
			HallProp("AZ_VampShackles", -18.0, W - 0.05, 1.4, -90);
			HallProp("AZ_VampShackles", -10.0, -W + 0.05, 1.4, 90);
			HallProp("AZ_VampBanner", -6.0, -W + 0.05, 1.9, 90);
			HallProp("AZ_VampBanner", -13.0, W - 0.05, 1.9, -90);
			HallProp("AZ_VampChainsHanging", -15.0, 1.5, 6.45, 0);
			HallProp("AZ_VampChainsHanging", -9.0, -1.8, 6.35, 40);
			HallProp("AZ_VampGibbet_Hanging", -12.0, -3.0, 5.7, 20);
			HallProp("AZ_VampCandelabra", -7.0, -3.6, 0, 90);
			PutCrypt("AZ_VampDecal_DragMark", Hall(-11.0, 0.3, 0.01), HALL_YAW - 90);
		}


		PutCrypt("AZ_Crypt_Crawl_Collapse", Hall(0, 0), HALL_YAW);
		NaveProp("AZ_Crypt_Brazier", 2.6, -3.2, 0, 0);
		NaveProp("AZ_Crypt_Brazier", 2.6, 3.2, 0, 0);
		PutCrypt("AZ_Crypt_Breach", Hall(0, 0), HALL_YAW);
		if (!EntryOn())
		{

			PutCrypt("AZ_Crypt_Plug_Crawl", Hall(0, 0), HALL_YAW);
			PutCrypt("AZ_Crypt_Plug_Nook", Hall(0, 0), HALL_YAW);
			PutCrypt("AZ_Crypt_Plug_Slab", Hall(0, 0), HALL_YAW);
			PutCrypt("AZ_Crypt_Plug_Squeeze", Nave(0, 0), HALL_YAW);
		}
		PutCrypt("AZ_Crypt_Wall_Back", Nave(0, 0), HALL_YAW);
		PutCrypt("AZ_Crypt_Ledge", Nave(0, 0), HALL_YAW);
		PutCrypt("AZ_Crypt_Ledge_Balustrade", Nave(0, 0), HALL_YAW);

		PutCrypt("AZ_Crypt_Ledge_Mourner", Hall(2.6, 7.2), HALL_YAW);
		PutCrypt("AZ_Crypt_Ledge_Mourner", Hall(2.6, -7.2), HALL_YAW);
		PutCrypt("AZ_Crypt_Spring_Blood", Nave(HALL_F0, 0, -NAVE_DEPTH), HALL_YAW);



		PutCrypt("AZ_Crypt_Outer_Shell", Nave(0, 0), HALL_YAW);
		PutCrypt("AZ_Crypt_Seal_Cap", Nave(0, 0), HALL_YAW);


		for (int sb = -1; sb < BAYS; sb++)
		{
			float fs = HALL_F0 + BAY * (sb + 0.5);
			float roof = 12.75;
			if (sb >= BAYS - 2)
				roof += RAISE;
			if ((sb != 0 && sb != 5) || !AnteOn())
				PutCrypt("AZ_Crypt_Seal_Side", Nave(fs, -9.1), HALL_YAW);
			if (sb != DOOR_BAY && (sb != DOOR_BAY + 1 || !AnteOn()))
				PutCrypt("AZ_Crypt_Seal_Side", Nave(fs, 9.1), HALL_YAW);
			PutCrypt("AZ_Crypt_Seal_Slab", Nave(fs, 0, roof), HALL_YAW);
			PutCrypt("AZ_Crypt_Seal_Slab", Nave(fs, 0, -2.6), HALL_YAW);
		}


		for (int i = 0; i < BAYS; i++)
		{
			float fc = HALL_F0 + BAY * (i + 0.5);
			PutCrypt("AZ_Crypt_Walkway_Bay", Nave(fc, -WALK_R), HALL_YAW);
			PutCrypt("AZ_Crypt_Walkway_Bay", Nave(fc, WALK_R), HALL_YAW + 180);
			PutCrypt("AZ_Crypt_Nave_Bay", Nave(fc, 0, -NAVE_DEPTH), HALL_YAW);
			PutCrypt("AZ_Crypt_Blood_Nave", Nave(fc, 0, -NAVE_DEPTH + s_Blood), HALL_YAW);


			if (i == 1 || i == 4)
				PutCrypt("AZ_Crypt_Blood_Debris_A", Nave(fc, 0, -NAVE_DEPTH + s_Blood - 0.03), HALL_YAW);
			else if (i == 2 || i == 5)
				PutCrypt("AZ_Crypt_Blood_Debris_B", Nave(fc, 0, -NAVE_DEPTH + s_Blood - 0.03), HALL_YAW);
			if ((i == 0 || i == 5) && AnteOn())
			{
				PutCrypt("AZ_Crypt_Wall_Bay_Door", Nave(fc, -AISLE_R), HALL_YAW);
				PutCrypt("AZ_Crypt_Door_Frame", Nave(fc, -AISLE_R - 1.25), HALL_YAW);
			}
			else if (i == 1 || i == 3)
				PutCrypt("AZ_Crypt_Wall_Bay_Arco", Nave(fc, -AISLE_R), HALL_YAW);
			else
				PutCrypt("AZ_Crypt_Wall_Bay", Nave(fc, -AISLE_R), HALL_YAW);
			if (i == DOOR_BAY)
				BuildDoorBay(fc);
			else if (i == 4 || i == 6)
				PutCrypt("AZ_Crypt_Wall_Bay_Arco_Rev", Nave(fc, AISLE_R), HALL_YAW + 180);
			else
				PutCrypt("AZ_Crypt_Wall_Bay_Rev", Nave(fc, AISLE_R), HALL_YAW + 180);


			float lift = 0;
			if (i >= BAYS - 2)
				lift = RAISE;
			PutCrypt("AZ_Crypt_Wall_Impost", Nave(fc, -AISLE_R, lift), HALL_YAW);
			PutCrypt("AZ_Crypt_Wall_Impost", Nave(fc, AISLE_R, lift), HALL_YAW + 180);



			if (i == 2)
				PutCrypt("AZ_Crypt_Rubble_Wall_A", Nave(fc + 1.9, -AISLE_R), HALL_YAW);
			else if (i == 6)
				PutCrypt("AZ_Crypt_Rubble_Wall_B", Nave(fc - 1.8, -AISLE_R), HALL_YAW);
			else if (i == 7)
				PutCrypt("AZ_Crypt_Rubble_Wall_A", Nave(fc + 1.6, -AISLE_R), HALL_YAW);
			else if (i == 3)
				PutCrypt("AZ_Crypt_Bones_Arco", Nave(fc, -AISLE_R), HALL_YAW);
			if (i == 1)
				PutCrypt("AZ_Crypt_Rubble_Wall_B", Nave(fc - 1.9, AISLE_R), HALL_YAW + 180);
			else if (i == 5)
				PutCrypt("AZ_Crypt_Rubble_Wall_A", Nave(fc + 1.8, AISLE_R), HALL_YAW + 180);
			else if (i == 7)
				PutCrypt("AZ_Crypt_Rubble_Wall_B", Nave(fc - 1.5, AISLE_R), HALL_YAW + 180);
			else if (i == 4)
				PutCrypt("AZ_Crypt_Bones_Arco", Nave(fc, AISLE_R), HALL_YAW + 180);


			if (i >= BAYS - 2)
			{
				PutCrypt("AZ_Crypt_Vault_Bay", Nave(fc, 0, RAISE), HALL_YAW);
				PutCrypt("AZ_Crypt_Wall_Clere", Nave(fc, -AISLE_R), HALL_YAW);
				PutCrypt("AZ_Crypt_Wall_Clere", Nave(fc, AISLE_R), HALL_YAW + 180);
				if (i == BAYS - 2)
					PutCrypt("AZ_Crypt_Vault_Step", Nave(HALL_F0 + BAY * i, 0), HALL_YAW);
			}
			else
				PutCrypt("AZ_Crypt_Vault_Bay", Nave(fc, 0), HALL_YAW);


			float fp = HALL_F0 + BAY * (i + 1);

			string pl = PILLARS_L.Get(i);
			string pr = PILLARS_R.Get(i);
			string pa = "AZ_Crypt_Pillar_" + pl;
			string pb = "AZ_Crypt_Pillar_" + pr;
			if (pl == "X")
				pa = "AZ_Crypt_Pillar_Bare";
			if (pr == "X")
				pb = "AZ_Crypt_Pillar_Bare";
			PutCrypt(pa, Nave(fp, -PILLAR_R), HALL_YAW);
			PutCrypt(pb, Nave(fp, PILLAR_R), HALL_YAW + 180);
			if (pl == "X")
				PutCrypt("AZ_Crypt_Vine_Fallen", Nave(fp, -PILLAR_R), HALL_YAW);
			if (pr == "X")
				PutCrypt("AZ_Crypt_Vine_Fallen", Nave(fp, PILLAR_R), HALL_YAW + 180);
			PutCrypt("AZ_Crypt_Arch_Cross", Nave(fp, 0), HALL_YAW);


			bool leftBowl = (i % 2 == 0 && i != 2) || i == 1;
			float leftBowlF = fp - 1.8;
			if (i == 1)
				leftBowlF = fp + 1.8;
			if (i > 0)
			{

				string arcL = "AZ_Crypt_Arch_Arcade";
				string arcR = "AZ_Crypt_Arch_Arcade";
				if (i % 2 == 0)
					arcL = "AZ_Crypt_Arch_Arcade_Soot";
				if (i % 2 == 0 && i != 6)
					arcR = "AZ_Crypt_Arch_Arcade_Soot";
				PutCrypt(arcL, Nave(fp - BAY / 2, -PILLAR_R), HALL_YAW);
				PutCrypt(arcR, Nave(fp - BAY / 2, PILLAR_R), HALL_YAW);
			}
			if (leftBowl)
				NaveProp("AZ_Crypt_Fire_Bowl", leftBowlF, -PILLAR_R, 0, 0);
			if (i % 2 == 0)
			{
				float crownY = 11.2;
				if (i == 6)
					crownY = 11.95;
				string crownCls = "AZ_Crypt_Chandelier";
				if (i == 0 || i == 4)
					crownCls = "AZ_Crypt_Chandelier_Flames";
				PutCrypt(crownCls, Nave(fp, 0, crownY), HALL_YAW);




				if (i == 6)
				{
					NaveProp("AZ_Crypt_Fire_Bowl", 52.4, 2.85, 0, 0);


					NaveProp("AZ_Crypt_Fire_Bowl", 52.4, -2.85, 0, 0);
				}
				else
					NaveProp("AZ_Crypt_Fire_Bowl", fp - 1.8, PILLAR_R, 0, 0);
			}



			int n = 0;
			if (i == 0)
				n = 1;
			else if (i == 3 || i == BAYS - 1)
				n = 2;
			for (int k = 0; k < n; k++)
			{
				float fb = HALL_F0 + BAY * i + 1.6 + (k + 0.5) * 2.8 / n;

				int csl = i * 3 + k * 2;
				csl = csl - csl / 5 * 5;
				int csr = i * 2 + k * 3 + 2;
				csr = csr - csr / 5 * 5;
				Object cb = NaveProp("AZ_Crypt_Bank_Low", fb, -3.35, 0, 90);
				if (cb)
					cb.SetScale(0.65 + 0.175 * csl);
				cb = NaveProp("AZ_Crypt_Bank_Low", fb, 3.35, 0, -90);
				if (cb)
					cb.SetScale(0.65 + 0.175 * csr);
			}
		}


		NaveProp("AZ_VampShroudedBody", 24.0, 5.0, 0, -90);
		NaveProp("AZ_VampSkull_Pile", 22.6, 5.1, 0, -120);
		NaveProp("AZ_Crypt_Stand_Single", 25.4, 5.2, 0, -90);



		PutCrypt("AZ_Crypt_Apse", Nave(APSE_F, 0), HALL_YAW);

		if (!LayoutF())
			PutCrypt("AZ_Crypt_Apse_Step_Box", Nave(APSE_F, 0), HALL_YAW);
		PutCrypt("AZ_Crypt_Apse_Niche", Nave(APSE_F, 0), HALL_YAW);
		AZ_VampCryptExit.Build();
		PutCrypt("AZ_Crypt_River_End", Nave(APSE_F + 1.25, 0), HALL_YAW);
		PutCrypt("AZ_Crypt_Seal_Side", Nave(APSE_F + 11.4, 0), HALL_YAW + 90);

		for (int ap = 0; ap < 3; ap++)
		{
			float apf = APSE_F + 0.8 + 3.8 * ap;
			PutCrypt("AZ_Crypt_Apse_Pilaster", Nave(apf, -7.9), HALL_YAW);
			PutCrypt("AZ_Crypt_Apse_Pilaster", Nave(apf, 7.9), HALL_YAW + 180);
		}
		PutCrypt("AZ_Crypt_Blood_RiverEnd", Nave(APSE_F + 1.25, 0, s_Blood), HALL_YAW);
		if (LayoutF())
		{


			Object drumAltar = NaveProp("AZ_VampAltar_Drum", ALTAR_F, 0, DAIS_Y + DRUM_TOP_F, 180);
			if (drumAltar)
			{
				drumAltar.SetScale(DRUM_ALTAR_SCALE);
				AZ_VampRitual.Register(drumAltar);
			}
			Print("[AZ_Vamp] crypt altar layout F: drum altar " + (drumAltar != null));
		}
		else
		{
			NaveProp(AltarClass(), ALTAR_F, 0, DAIS_Y, 180);
			NaveProp("AZ_VampAltarCircle", ALTAR_F, 0, DAIS_Y + 0.01, 180);
		}
		NaveProp("AZ_VampAltarBasin", ALTAR_F, 2.9, DAIS_Y, 180);


		NaveProp("AZ_Crypt_Brazier", APSE_F + 8.75, -3.65, DAIS_Y, 180);
		NaveProp("AZ_Crypt_Brazier", APSE_F + 8.75, 3.65, DAIS_Y, 180);



		NaveProp("AZ_Crypt_Brazier", 46.1, -3.2, 0, 0);
		NaveProp("AZ_Crypt_Brazier", 46.1, 3.2, 0, 0);
		NaveProp("AZ_Crypt_Brazier", 53.6, -3.5, 0, 0);
		NaveProp("AZ_Crypt_Brazier", 53.6, 3.5, 0, 0);


		PutCrypt("AZ_Crypt_Candelabrum", Nave(57.9, -3.55, DAIS_Y), HALL_YAW);
		PutCrypt("AZ_Crypt_Candelabrum", Nave(57.9, 3.55, DAIS_Y), HALL_YAW);

		NaveProp("AZ_Crypt_Stand_Three", APSE_F + 3.0, -4.6, 0, 180);
		NaveProp("AZ_Crypt_Stand_Three", APSE_F + 3.0, 4.6, 0, 180);



		Robe(AZ_VampCostumes.GRAVE_GOWN, ALTAR_F + 3.3, -2.4);
		Robe(AZ_VampCostumes.VELVET_GOWN, ALTAR_F + 3.3, 2.4);
		Robe(AZ_VampCostumes.GREATCOAT, ALTAR_F + 1.6, -2.9);


		PutCrypt("AZ_Crypt_Moon_Grate", Nave(CountessF(), 0, MOON_H), HALL_YAW);
		PutCrypt("AZ_Crypt_Moon_Shaft", Nave(CountessF(), 0, MOON_H), HALL_YAW);


		if (!LayoutF())
		{
			Object drum = PutCrypt("AZ_Crypt_Countess_Drum", Nave(APSE_F + 8.6, 0, CountessFeet()), HALL_YAW);
			if (drum)
				drum.SetScale(DRUM_SCALE);
		}
		s_Countess = AZ_Crypt_Countess.Cast(PutCrypt("AZ_Crypt_Countess", Nave(CountessF(), 0, CountessFeet()), HALL_YAW));
		if (s_Countess)
			s_Countess.SetScale(CountessScale());
		NaveProp("AZ_Crypt_Ante_Ossuary", HALL_F0 + BAY * 1.5, -AISLE_R - 1.25, 0, -90);
		NaveProp("AZ_Crypt_Ante_Ossuary", HALL_F0 + BAY * 4.5, -AISLE_R - 1.25, 0, -90);
		NaveProp("AZ_Crypt_Ante_Ossuary", HALL_F0 + BAY * 3.5, AISLE_R + 1.25, 0, 90);
		NaveProp("AZ_Crypt_Ante_Ossuary", HALL_F0 + BAY * 6.5, AISLE_R + 1.25, 0, 90);



		PutCrypt("AZ_Crypt_Hall_Gibbet_Remains", Nave(18.0, -6.6, 9.87), HALL_YAW);
		PutCrypt("AZ_Crypt_Hall_Gibbet_Open", Nave(30.0, 6.6, 9.87), HALL_YAW);
	}




	static bool ShotSpot(int i, out vector stand, out vector cam, out vector look)
	{
		switch (i)
		{
			case 0:
				stand = Door(2.5, -DOOR_FRONT - 9.0, true);
				cam = Door(3.0, -DOOR_FRONT - 11.0, true);
				cam[1] = cam[1] + 1.7;
				look = Door(0, -DOOR_FRONT, false);
				look[1] = look[1] + 2.4;
				return true;
			case 1:
				stand = Hall(0.15, 0.8, 0.1);
				cam = Hall(0.15, 0, 1.6);
				look = Nave(40.0, 0, 4.0);
				return true;
			case 2:
				stand = Nave(14.0, -5.0, 0.1);
				cam = Nave(13.0, -3.0, 1.6);
				look = Nave(50.0, 0, 1.5);
				return true;
			case 3:
				stand = Nave(50.0, 2.6, 0.1);
				cam = Nave(50.5, 1.8, 1.4);
				look = Nave(CountessF(), 0, 3.4);
				return true;
		}
		return false;
	}


	static bool Free(PlayerBase p)
	{
		if (!p || !p.IsAlive() || p.IsUnconscious() || p.IsRestrained() || p.IsInVehicle())
			return false;
		return !p.m_AZVampHeld && !p.m_AZVampTurning;
	}

	static bool CanMove(PlayerBase p)
	{
		return Free(p) && GetGame().GetTime() >= p.m_AZCryptAt;
	}



	static int s_ExitOnlyCheckAt;
	static bool s_ExitOnly;
	static bool DoorExitOnly()
	{
		if (GetGame().IsServer())
			return !EntryOn();
		int now = GetGame().GetTime();
		if (now < s_ExitOnlyCheckAt)
			return s_ExitOnly;
		s_ExitOnlyCheckAt = now + 1000;
		s_ExitOnly = false;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(DoorOrigin(), 4, objs, null);
		foreach (Object o : objs)
		{
			AZ_Crypt_Tomb_Grille g = AZ_Crypt_Tomb_Grille.Cast(o);
			if (g && g.AZ_ExitOnly())
				s_ExitOnly = true;
		}
		return s_ExitOnly;
	}

	static void Enter(PlayerBase p)
	{
		if (!AtDoor(p.GetPosition()))
			return;
		if (DoorExitOnly())
		{
			p.Message("Barred from within. This way only lets the dead out.", "colorAction");
			Print("[AZ_Vamp] crypt door: " + AZ_VampState.Uid(p) + " enter refused (exit only, cryptEntrance 0)");
			return;
		}
		EnterFrom(p, 0);
	}


	static ref map<string, int> s_BlockedSaid;

	static void SayBlocked(PlayerBase p)
	{
		if (!s_BlockedSaid)
			s_BlockedSaid = new map<string, int>;
		string uid = AZ_VampState.Uid(p);
		int now = GetGame().GetTime();
		if (s_BlockedSaid.Contains(uid) && now < s_BlockedSaid.Get(uid))
			return;
		s_BlockedSaid.Set(uid, now + 5000);
		p.Message("The way is blocked. Try again.", "colorAction");
	}

	// AZ-2374: an A1 trigger seconds after an entry (server still saw the old surface position) sent the player down
	// again; the repeated snaps got him kicked with code 10. One entry per player per RE_ENTRY_MS.
	static const int RE_ENTRY_MS = 15000;
	static bool JustEntered(PlayerBase p)
	{
		return p && p.m_AZCryptEnteredAt > 0 && GetGame().GetTime() - p.m_AZCryptEnteredAt < RE_ENTRY_MS;
	}

	static void EnterFrom(PlayerBase p, int from, string route = "tomb_door")
	{
		if (!s_Built || !CanMove(p))
			return;
		if (JustEntered(p))
		{
			Print("[AZ_Vamp] crypt: " + AZ_VampState.Uid(p) + " re-entry via " + route + " ignored (" + (GetGame().GetTime() - p.m_AZCryptEnteredAt) + " ms after the last)");
			return;
		}
		p.m_AZCryptEnteredAt = GetGame().GetTime();


		p.m_AZCryptFallUntil = GetGame().GetTime() + AZ_VampCryptLedge.WAIT_MAX_MS + 3000;
		p.m_AZCryptFrom = from;
		vector at = PickSlot(ArriveSlots());
		if (!EntryOn())
			at = AZ_VampCryptLedge.Pick();
		vector gap = Hall(LEG1, 4.8, 0.1);
		vector dir = gap - at;
		dir[1] = 0;
		dir.Normalize();
		if (!EntryOn())
			dir = AZ_VampCryptLedge.Facing(at);
		if (from == 0 && AZ_VampCryptTomb.On())
		{
			at = AZ_VampCryptTomb.Pick();
			dir = AZ_VampCryptTomb.Facing(at);
		}
		else if (!EntryOn())
		{


			AZ_VampCryptLedge.Release(at);
			AZ_VampCryptLedge.Arrive(p, 0, route + "_ledge");
			p.Message("Steps go down a long way in the dark. The air is cold and smells of iron.", "colorAction");
			Print("[AZ_Vamp] crypt: " + AZ_VampState.Uid(p) + " went down (ledge)");
			return;
		}


		vector safeAt;
		if (!AZ_VampSnap.Landing(p, at, safeAt))
		{
			bool found = false;
			array<vector> others = ArriveSlots();
			foreach (vector other : others)
			{
				if (AZ_VampSnap.Landing(p, other, safeAt))
				{
					Print("[AZ_Vamp] crypt: arrival slot " + HallLocal(at) + " blocked, using " + HallLocal(other));
					at = other;
					found = true;
					break;
				}
			}
			if (!found)
			{

				SayBlocked(p);
				return;
			}
		}
		MoveTo(p, at, dir, route);
		p.Message("Steps go down a long way in the dark. The air is cold and smells of iron.", "colorAction");
		Print("[AZ_Vamp] crypt: " + AZ_VampState.Uid(p) + " went down");
	}

	static void Leave(PlayerBase p)
	{
		if (!CanMove(p) || !AtGate(p.GetPosition()))
			return;
		MoveOut(p);
		Print("[AZ_Vamp] crypt: " + AZ_VampState.Uid(p) + " came up (way " + p.m_AZCryptFrom + ")");
	}


	static void MoveOut(PlayerBase p)
	{

		if ((p.m_AZCryptFrom == 1 || !TombDoorOn()) && AZ_VampCryptA1.Ready())
		{
			vector a1At, a1Dir;
			if (!AZ_VampCryptA1.PickExit(p, a1At, a1Dir))
			{
				SayBlocked(p);
				return;
			}
			p.m_AZCryptExitGrace = true;
			MoveTo(p, a1At, a1Dir, "a1_exit");
			return;
		}
		vector at = AZ_VampCryptLedge.PickFrom(MouthSlots(), "door exit");
		p.m_AZCryptExitGrace = true;
		vector dir = Fwd(DOOR_YAW);
		MoveTo(p, at, -dir, "tomb_exit");
	}






	static ref array<vector> s_Held;
	static ref array<int> s_HeldUntil;

	static array<vector> ArriveSlots()
	{
		if (!EntryOn())
			return AZ_VampCryptLedge.Slots();
		array<vector> r = new array<vector>;
		for (int row = 0; row < 2; row++)
		{
			for (int col = 0; col < 4; col++)
				r.Insert(Hall(-ARRIVE - 0.2 + 1.2 * row, ARRIVE_R - 1.2 + 1.2 * col, 0.1));
		}
		return r;
	}

	static array<vector> MouthSlots()
	{
		array<vector> r = new array<vector>;
		r.Insert(Mouth());
		vector side = Side(DOOR_YAW);
		vector away = -Fwd(DOOR_YAW);
		r.Insert(Mouth() - side * 1.2);
		r.Insert(Mouth() + side * 1.2);
		r.Insert(Mouth() + away * 1.2);
		r.Insert(Mouth() + away * 1.2 - side * 1.2);
		r.Insert(Mouth() + away * 1.2 + side * 1.2);
		for (int g = 0; g < r.Count(); g++)
		{
			vector gs = r[g];
			float gy = GetGame().SurfaceY(gs[0], gs[2]) + 0.05;
			gs[1] = gy;
			r[g] = gs;
		}
		return r;
	}

	static vector PickSlot(array<vector> slots)
	{
		if (!s_Held)
		{
			s_Held = new array<vector>;
			s_HeldUntil = new array<int>;
		}
		int now = GetGame().GetTime();
		for (int h = s_Held.Count() - 1; h >= 0; h--)
		{
			if (s_HeldUntil[h] < now)
			{
				s_Held.Remove(h);
				s_HeldUntil.Remove(h);
			}
		}
		int best = 0;
		int bestScore = 1000000;
		for (int i = 0; i < slots.Count(); i++)
		{
			int score = 0;
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition3D(slots[i], 2.5, near, null);
			foreach (Object o : near)
			{
				PlayerBase pb = PlayerBase.Cast(o);
				if (!pb || !pb.IsAlive())
					continue;
				if (vector.Distance(pb.GetPosition(), slots[i]) < 1.0)
					score += 100;
				else
					score += 1;
			}
			foreach (vector hv : s_Held)
			{
				float hd = vector.Distance(hv, slots[i]);
				if (hd < 1.0)
					score += 100;
				else if (hd < 2.5)
					score += 1;
			}
			if (score < 100 && bestScore >= 100)
			{
				best = i;
				bestScore = score;
				break;
			}
			if (score < bestScore)
			{
				best = i;
				bestScore = score;
			}
		}
		s_Held.Insert(slots[best]);
		s_HeldUntil.Insert(now + 3000);
		return slots[best];
	}

	static void MoveTo(PlayerBase p, vector at, vector dir, string route = "crypt_box")
	{
		vector safe;
		if (!p)
			return;
		if (!AZ_VampSnap.Landing(p, at, safe))
		{
			p.m_AZCryptFallUntil = GetGame().GetTime() + 3000;
			p.m_AZCryptExitGrace = false;
			SayBlocked(p);
			return;
		}
		at = safe;
		p.m_AZCryptFallUntil = GetGame().GetTime() + TRANSIT_MAX_MS;
		p.m_AZCryptAt = GetGame().GetTime() + 3000;
		Transit(p);
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, RPC_FADE, new Param1<int>(FADE_MS), true, p.GetIdentity());
		AZ_VampSnap.Arm(p, at, dir);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Land, MOVE_MS, false, p, at, dir, route);
	}




	static const int TRANSIT_MAX_MS = 9000;



	static void Transit(PlayerBase p)
	{
		if (!p || p.m_AZCryptTransit || p.m_AZArrivalPrepared)
			return;
		p.m_AZArrivalPrepared = true;
		p.m_AZArrivalRefused = p.m_AZCryptTransitForfeit || !p.GetAllowDamage();
		int now = GetGame().GetTime();
		if ((p.m_AZLastHitAt > 0 && now - p.m_AZLastHitAt < HIT_NO_GRACE_MS) || (p.m_AZCryptGraceEndAt > 0 && now - p.m_AZCryptGraceEndAt < GRACE_COOLDOWN_MS))
			p.m_AZArrivalRefused = true;
		if (p.m_AZArrivalRefused)
			return;
		p.m_AZCryptTransit = true;
		p.m_AZCryptTransitGen++;
		p.SetAllowDamage(false);
		// Cover the existing 30 s ledge queue; Far replaces this with the move-based cap.
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TransitTimeout, AZ_VampCryptLedge.WAIT_MAX_MS + TRANSIT_MAX_MS, false, p, p.m_AZCryptTransitGen);
	}

	static void TransitTimeout(PlayerBase p, int gen)
	{
		if (p && p.m_AZCryptTransitGen == gen)
		{
			TransitEnd(p);
			p.m_AZArrivalPrepared = false;
		}
	}

	static void TransitEnd(PlayerBase p)
	{
		if (!p || !p.m_AZCryptTransit)
			return;
		p.m_AZCryptTransit = false;
		p.SetAllowDamage(p.m_AZCryptGraceUntil == 0);
	}

	static const int EXIT_GRACE_MS = 6000;

	static void Land(PlayerBase p, vector at, vector dir, string route = "crypt_box")
	{
		bool exitGrace = p && p.m_AZCryptExitGrace;
		if (p)
			p.m_AZCryptExitGrace = false;
		if (!Free(p))
		{
			TransitEnd(p);
			if (p)
				p.m_AZArrivalPrepared = false;
			return;
		}
		vector safe;
		if (!AZ_VampSnap.Landing(p, at, safe))
		{
			TransitEnd(p);
			p.m_AZArrivalPrepared = false;
			SayBlocked(p);
			return;
		}
		at = safe;
		p.m_AZArrivalGraceMS = 0;
		if (Inside(at))
			p.m_AZArrivalGraceMS = GRACE_MS;
		else if (exitGrace)
			p.m_AZArrivalGraceMS = EXIT_GRACE_MS;
		AZ_VampGuest.Landed(p);
		AZ_VampSnap.Far(p, at, dir, route);
		if (Inside(at))
		{
			p.m_AZCryptSince = GetGame().GetTime();
			p.m_AZCryptWarnAt = 0;
			if (AZ_VampState.Cfg() && AZ_VampState.Cfg().riverFlow && p.GetIdentity())
				AZ_VampBand.Send(p, "F");
			if (EntryOn())
				AZ_VampWhisperLines.Trigger(p, "ante");
		}
		else
		{
			AZ_VampWhisperLines.Trigger(p, "exit");
		}
	}



	static void Grace(PlayerBase p, int ms)
	{
		if (!p || p.m_AZCryptGraceUntil != 0)
			return;
		int now = GetGame().GetTime();
		if (p.m_AZLastHitAt > 0 && now - p.m_AZLastHitAt < HIT_NO_GRACE_MS)
		{
			Print("[AZ_Vamp] crypt grace " + AZ_VampState.Uid(p) + " refused (hit " + ((now - p.m_AZLastHitAt) / 1000) + " s ago)");
			return;
		}
		if (p.m_AZCryptGraceEndAt > 0 && now - p.m_AZCryptGraceEndAt < GRACE_COOLDOWN_MS)
		{
			Print("[AZ_Vamp] crypt grace " + AZ_VampState.Uid(p) + " refused (cooldown)");
			return;
		}
		if (!p.GetAllowDamage())
			return;
		p.m_AZCryptGraceUntil = now + ms;
		p.m_AZCryptGraceHands = p.GetItemInHands();
		p.SetAllowDamage(false);
		Print("[AZ_Vamp] crypt grace " + AZ_VampState.Uid(p) + " start " + ms + " ms");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GraceTick, 200, false, p);
	}

	static void GraceTick(PlayerBase p)
	{
		if (!p || p.m_AZCryptGraceUntil == 0)
			return;
		if (!p.IsAlive())
		{
			GraceEnd(p, "died");
			return;
		}
		if (GetGame().GetTime() >= p.m_AZCryptGraceUntil)
		{
			GraceEnd(p, "time");
			return;
		}
		ActionBase running;
		if (p.GetActionManager())
			running = p.GetActionManager().GetRunningAction();
		if (ActionAZCryptEnter.Cast(running))
			running = null;

		if (p.GetCommand_Melee2() || running || p.GetItemInHands() != p.m_AZCryptGraceHands)
		{
			GraceEnd(p, "attacked or acted");
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GraceTick, 200, false, p);
	}

	static void GraceEnd(PlayerBase p, string why)
	{
		if (!p || p.m_AZCryptGraceUntil == 0)
			return;
		p.m_AZCryptGraceUntil = 0;
		p.m_AZCryptGraceEndAt = GetGame().GetTime();
		p.m_AZCryptGraceHands = null;
		// Natural grace expiry must not expose a client still reholding. An action
		// cancels both owned protections, so a rehold cannot hide an attack.
		if (p.m_AZCryptTransit && why != "time")
		{
			p.m_AZCryptTransit = false;
			p.m_AZCryptTransitForfeit = true;
		}
		p.SetAllowDamage(!p.m_AZCryptTransit);
		Print("[AZ_Vamp] crypt grace " + AZ_VampState.Uid(p) + " end (" + why + ")");
	}




	static const int RITUAL_GRACE_MS = 120000;
	static const int EVICT_AFTER_MS = 60000;
	static const int RPC_WHISPER = 4702233;
	static const int RPC_FLARE = 4702234;


	static void FireFlare()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		int n = 0;
		foreach (PlayerBase p : list)
		{
			if (p && p.GetIdentity() && Inside(p.GetPosition()))
			{
				GetGame().RPCSingleParam(p, RPC_FLARE, new Param1<int>(1), true, p.GetIdentity());
				n++;
			}
		}
		Print("[AZ_Vamp] crypt fires flare for " + n + " players");
	}




	static void MarkRitual(PlayerBase p)
	{
		if (!p)
			return;
		p.m_AZCryptRitualAt = GetGame().GetTime();
		if (p.m_AZCryptSince != 0)
			p.m_AZCryptSince = p.m_AZCryptRitualAt;
	}


	static bool LoiterExempt(PlayerBase p)
	{
		return p.GetIdentity() && AZ_VampState.Cfg().adminUids && AZ_VampState.Cfg().adminUids.Find(p.GetIdentity().GetId()) >= 0;
	}

	static bool Engaged(PlayerBase p)
	{
		if (p.GetActionManager() && ActionAZOfferBlood.Cast(p.GetActionManager().GetRunningAction()))
			return true;
		if (AZ_VampAltar.s_Pending.Contains(AZ_VampState.Uid(p)))
			return true;
		return p.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH) && AZ_VampAltar.Near(p.GetPosition());
	}

	static void Watch()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		AZ_VampGuest.CampTick(list);
		int loiter = AZ_VampState.Cfg().cryptLoiterMin * 60000;
		if (loiter <= 0)
			return;
		int now = GetGame().GetTime();
		foreach (PlayerBase p : list)
		{
			if (!p)
				continue;
			if (!p.IsAlive() || !Inside(p.GetPosition()) || LoiterExempt(p))
			{
				p.m_AZCryptSince = 0;
				p.m_AZCryptWarnAt = 0;
				continue;
			}
			if (p.m_AZCryptSince == 0)
				p.m_AZCryptSince = now;
			if (Engaged(p))
				MarkRitual(p);
			if (p.m_AZCryptRitualAt > 0 && now - p.m_AZCryptRitualAt < RITUAL_GRACE_MS)
			{
				p.m_AZCryptWarnAt = 0;
				continue;
			}
			if (p.m_AZCryptWarnAt == 0 && now - p.m_AZCryptSince >= loiter)
			{
				p.m_AZCryptWarnAt = now;
				p.Message("The dead tire of being watched. Leave, or be thrown out.", "colorImportant");
				if (p.GetIdentity())
					GetGame().RPCSingleParam(p, RPC_WHISPER, new Param1<int>(1), true, p.GetIdentity());
				Print("[AZ_Vamp] crypt warn " + AZ_VampState.Uid(p) + " after " + ((now - p.m_AZCryptSince) / 1000) + " s inside");
			}
			else if (p.m_AZCryptWarnAt > 0 && now - p.m_AZCryptWarnAt >= EVICT_AFTER_MS && CanMove(p))
			{
				MoveOut(p);
				p.m_AZCryptWarnAt = 0;
				p.Message("Cold hands push you up the stairs. The door shuts behind you.", "colorImportant");
				Print("[AZ_Vamp] crypt evict " + AZ_VampState.Uid(p));
			}
		}
	}


	static void OnConnect(PlayerBase p)
	{
		AZ_VampCryptGallery.OnConnect(p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampTie.ClearStale, 2000, false, p);
		if (!p || !Inside(p.GetPosition()))
			return;
		vector at = PickSlot(MouthSlots());
		vector dir = Fwd(DOOR_YAW);
		// AZ-2431: announce the login move to COT's movement check.
		AZ_VampSnap.AllowMove(p);
		p.SetPosition(at);
		p.SetDirection(-dir);
		Grace(p, GRACE_MS);
		// AZ-2491: whatever the grace chain does, nobody keeps a login-time protection or a stopped metabolism.
		if (p.m_AZCryptGraceUntil != 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LoginFailsafe, GRACE_MS + 4000, false, p);
		Print("[AZ_Vamp] crypt: " + AZ_VampState.Uid(p) + " logged in below, moved to the door");
	}

	static void LoginFailsafe(PlayerBase p)
	{
		if (!p || !p.IsAlive())
			return;
		if (p.m_AZCryptGraceUntil != 0)
		{
			GraceEnd(p, "login failsafe");
			Print("[AZ_Vamp] crypt: login failsafe ended a stuck grace for " + AZ_VampState.Uid(p));
		}
		if (p.IsPlayerSelected() && p.GetModifiersManager() && !p.GetModifiersManager().IsModifiersEnable())
		{
			p.GetModifiersManager().SetModifiers(true);
			Print("[AZ_Vamp] crypt: login failsafe re-enabled stopped modifiers for " + AZ_VampState.Uid(p));
		}
	}
}


class CCTAZAny : CCTBase
{
	override bool Can(PlayerBase player, ActionTarget target)
	{
		return target != null;
	}
}

class ActionAZCryptEnter : ActionInteractBase
{
	void ActionAZCryptEnter()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Enter the crypt";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player && !player.IsInVehicle() && AZ_VampCrypt.AtDoor(player.GetPosition()) && !AZ_VampCrypt.DoorExitOnly();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampCrypt.Enter(action_data.m_Player);
	}
}

class ActionAZCryptLeave : ActionInteractBase
{
	void ActionAZCryptLeave()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Leave the crypt";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !AZ_VampCrypt.AtGate(player.GetPosition()))
			return false;


		vector gate = AZ_VampCrypt.Hall(-AZ_VampCrypt.HALF_LEN - 0.4, 0);
		vector toGate = gate - player.GetPosition();
		toGate[1] = 0;
		if (toGate.Length() > 3.0)
			return false;
		vector look = player.GetDirection();
		look[1] = 0;
		if (vector.Dot(look.Normalized(), toGate.Normalized()) < 0.6)
			return false;

		return vector.Distance(player.GetPosition(), AZ_VampCrypt.Hall(AZ_VampCryptGallery.SLAB_F, AZ_VampCryptGallery.WALL_R)) > 5.0;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampCrypt.Leave(action_data.m_Player);
	}
}

modded class PlayerBase
{
	int m_AZCryptAt;
	int m_AZCryptFallUntil;
	int m_AZCryptEnteredAt;	// server time of the last EnterFrom (AZ-2374 re-entry guard)
	bool m_AZCryptTransit;
	int m_AZCryptTransitGen;
	bool m_AZCryptTransitForfeit;
	int m_AZLastPlayerHitAt;
	int m_AZGuestUntil;
	int m_AZCampSince;
	int m_AZCampStage;
	bool m_AZCryptExitGrace;
	int m_AZCryptFrom;
	int m_AZCryptGraceUntil;
	int m_AZCryptGraceEndAt;
	EntityAI m_AZCryptGraceHands;
	int m_AZCryptSince;
	int m_AZCryptWarnAt;
	int m_AZCryptRitualAt;
	int m_AZLastHitAt;

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (!GetGame().IsServer())
			return;
		m_AZLastHitAt = GetGame().GetTime();
		if (source && AZ_VampRitual.Protecting(this))
		{
			PlayerBase hitter = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			if (!hitter)
				hitter = PlayerBase.Cast(source);
			if (hitter && hitter != this)
				AZ_VampRitual.Protect(this, hitter, "struck someone at the altar");
		}
		if (source)
		{
			PlayerBase striker = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			if (striker && striker != this && AZ_VampRitual.Protecting(striker))
				AZ_VampRitual.ProtectedStruck(striker);


			PlayerBase instigator = striker;
			if (!instigator)
				instigator = PlayerBase.Cast(source);
			if (instigator != this)
				m_AZLastPlayerHitAt = GetGame().GetTime();
			if (instigator && instigator != this)
				instigator.m_AZGuestUntil = 0;
			if (striker && striker != this)
			{
				AZ_VampGuest.Struck(this, striker);
				if (striker.m_AZCryptTransit)
				{
					AZ_VampCrypt.TransitEnd(striker);
					striker.m_AZCryptTransitForfeit = true;
				}
			}
		}
		if (!source)
			return;

		PlayerBase from = PlayerBase.Cast(source.GetHierarchyRootPlayer());
		if (!from)
			from = PlayerBase.Cast(source);
		if (from && from != this && from.m_AZCryptGraceUntil != 0)
			AZ_VampCrypt.GraceEnd(from, "did harm");
	}

	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptEnter, InputActionMap);
		AddAction(ActionAZCryptLeave, InputActionMap);
		AddAction(ActionAZPressSkull, InputActionMap);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == AZ_VampCrypt.RPC_FLARE && GetGame().IsClient())
		{
			array<Object> all = new array<Object>;
			GetGame().GetObjectsAtPosition(GetPosition(), 90, all, null);
			foreach (Object fo : all)
			{


				AZ_Crypt_FireBase fire = AZ_Crypt_FireBase.Cast(fo);
				if (fire)
					fire.AZ_FireFlare();
			}
			return;
		}
		if (rpc_type == AZ_VampCrypt.RPC_WHISPER && GetGame().IsClient())
		{
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(GetPosition(), 30, near, null);
			foreach (Object o : near)
			{
				AZ_VampLitDecor lit = AZ_VampLitDecor.Cast(o);
				if (lit)
					lit.AZ_Flicker();
			}
			return;
		}
		if (rpc_type != AZ_VampCrypt.RPC_FADE || !GetGame().IsClient())
			return;
		Param1<int> p;
		if (!ctx.Read(p))
			return;

		PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Start();
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(AZ_CryptFadeEnd, p.param1, false);
	}

	void AZ_CryptFadeEnd()
	{
		EntityAI head = FindAttachmentBySlotName("Headgear");
		if (head && head.IsKindOf("BurlapSackCover"))
			return;
		if (AZ_VampSnap.s_ClientHolding)
			return;
		PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Stop();
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptEnter);
		actions.Insert(ActionAZCryptLeave);
		actions.Insert(ActionAZPressSkull);
	}
}



class AZ_Crypt_Chandelier extends AZ_VampLitDecor
{
	override bool AZ_LitInCrypt()
	{
		return true;
	}

	override typename AZLightType()
	{
		return AZ_VampChandelierLight;
	}
}




class AZ_Crypt_FireBase extends House
{
	protected Particle m_AZFire;
	protected Particle m_AZFlare;

	int AZFireParticle()
	{
		return ParticleList.CAMP_SMALL_FIRE;
	}



	vector AZFireDrop()
	{
		return vector.Zero;
	}

	vector AZFireAt()
	{
		vector at = "0 0.3 0";
		if (MemoryPointExists("fire"))
			at = GetMemoryPointPos("fire");
		return at + AZFireDrop();
	}

	override void EEInit()
	{
		super.EEInit();
		if (GetGame().IsDedicatedServer())
			return;
		m_AZFire = Particle.PlayOnObject(AZFireParticle(), this, AZFireAt());
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_AZFire)
			m_AZFire.Stop();
	}


	void AZ_FireFlare()
	{
		Particle.PlayOnObject(ParticleList.CAMP_FIRE_START, this, AZFireAt());
	}
}




class AZ_Crypt_Fire_Bowl extends AZ_Crypt_FireBase
{
	protected ScriptedLightBase m_AZBowlLight;
	static const bool OWN_LIGHT = true;

	override void EEInit()
	{
		super.EEInit();
		if (OWN_LIGHT && !GetGame().IsDedicatedServer())
			m_AZBowlLight = AZ_VampCryptLights.BowlLight(this);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_AZBowlLight)
			m_AZBowlLight.Destroy();
	}
}


class AZ_Crypt_Campfire_Dead extends AZ_Crypt_FireBase
{
	override int AZFireParticle()
	{
		return ParticleList.CAMP_SMALL_SMOKE;
	}

	override void AZ_FireFlare() {}
}


class AZ_Crypt_Brazier extends AZ_Crypt_FireBase
{
	protected ScriptedLightBase m_AZLight;




	override int AZFireParticle()
	{
		return ParticleList.HOUSE_SMALL_FIRE;
	}

	override vector AZFireDrop()
	{
		return Vector(0, -0.14, 0);
	}

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			m_AZLight = AZ_VampLightGate.AtPoint(AZ_VampBrazierLight, this, "light");
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_AZLight)
			m_AZLight.Destroy();
	}

	protected bool m_AZFlaring;




	override void AZ_FireFlare()
	{
		super.AZ_FireFlare();
		AZ_VampBrazierLight bl = AZ_VampBrazierLight.Cast(m_AZLight);
		if (!bl || m_AZFlaring || bl.m_AZOff || bl.AZ_Rest() <= 0 || bl.m_Brightness <= 0)
			return;
		m_AZFlaring = true;
		bl.FadeBrightnessTo(Math.Min(bl.AZ_Rest() * 2.2, 9.0), 1.5);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_FlareEnd, 1500, false);
	}

	void AZ_FlareEnd()
	{
		m_AZFlaring = false;
		AZ_VampBrazierLight bl = AZ_VampBrazierLight.Cast(m_AZLight);
		if (bl && bl.m_Brightness > 0)
			bl.FadeBrightnessTo(bl.AZ_Rest(), 2.0);
	}
}







class AZ_Crypt_Slab_Door extends House
{
	protected bool m_Open;
	protected bool m_OpenLocal;
	protected int m_CloseAt;

	void AZ_Crypt_Slab_Door()
	{
		RegisterNetSyncVariableBool("m_Open");
	}

	override void EEInit()
	{
		super.EEInit();
		SetAnimationPhase("slab", Phase());
		m_OpenLocal = m_Open;
	}

	float Phase()
	{
		if (m_Open)
			return 1;
		return 0;
	}

	bool IsOpen()
	{
		return m_Open;
	}

	void SetOpen(bool open)
	{
		if (!GetGame().IsServer())
			return;
		m_Open = open;
		SetAnimationPhase("slab", Phase());
		SetSynchDirty();
		if (open)
		{
			m_CloseAt = GetGame().GetTime() + 600000;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CheckClose, 60000, false);
		}
	}

	protected void CheckClose()
	{
		if (!m_Open)
			return;
		if (GetGame().GetTime() < m_CloseAt || PlayerNear())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CheckClose, 60000, false);
			return;
		}
		SetOpen(false);
		Print("[AZ_Vamp] crypt door closed again");
	}

	protected bool PlayerNear()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase p : list)
		{
			if (p && vector.Distance(p.GetPosition(), GetPosition()) < 9.0)
				return true;
		}
		return false;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_Open == m_OpenLocal)
			return;
		m_OpenLocal = m_Open;
		SetAnimationPhase("slab", Phase());
		if (GetGame().IsClient())
			SEffectManager.PlaySoundOnObject("AZ_CryptSlab_SoundSet", this);
	}
}



modded class PlayerBase
{
	protected float m_AZCryptSndCheck;
	protected float m_AZCryptBreathIn;
	protected float m_AZCryptBonesIn;
	protected bool m_AZCryptRevealed;
	protected ref EffectSound m_AZCryptBed;
	protected ref EffectSound m_AZCryptRadio;

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (!GetGame().IsClient() || !IsControlledPlayer())
			return;
		if (m_AZCryptBed)
			m_AZCryptBed.SetSoundVolume(AZ_VampWhisperLines.s_DuckLevel);
		if (m_AZCryptRadio)
			m_AZCryptRadio.SetSoundVolume(AZ_VampWhisperLines.s_DuckLevel);
		m_AZCryptSndCheck -= pDt;
		m_AZCryptBreathIn -= pDt;
		m_AZCryptBonesIn -= pDt;
		if (m_AZCryptSndCheck > 0)
			return;
		m_AZCryptSndCheck = 1.0;
		AZ_CryptSounds();
	}

	protected void AZ_CryptSounds()
	{
		if (!IsAlive() || !AZ_VampCrypt.Inside(GetPosition()))
		{
			AZ_CryptSoundStop(m_AZCryptBed);
			AZ_CryptSoundStop(m_AZCryptRadio);
			m_AZCryptBed = null;
			m_AZCryptRadio = null;
			m_AZCryptBreathIn = 0;
			return;
		}
		if (!m_AZCryptBed)
			m_AZCryptBed = SEffectManager.PlaySoundOnObject("AZ_CryptBed_SoundSet", this, 2.0, 2.0, true);

		vector l = AZ_VampCrypt.HallLocal(GetPosition());
		bool upper = l[2] > -1.5;
		if (AZ_VampCrypt.EntryOn() && upper && l[0] > -5.0 && l[0] < 0.0 && l[1] > -1.5 && l[1] < 0.9)
		{



			if (m_AZCryptBreathIn <= 0)
			{
				vector me = GetPosition();
				if (Math.RandomFloat01() < 0.7)
				{
					vector over = me + Vector(Math.RandomFloat(-1.5, 1.5), Math.RandomFloat(1.6, 2.2), Math.RandomFloat(-1.5, 1.5));
					SEffectManager.PlaySound("AZ_CryptBreach_SoundSet", over);
				}
				else
				{
					vector ahead = me + AZ_VampCrypt.Fwd(AZ_VampCrypt.HALL_YAW) * Math.RandomFloat(8, 14);
					ahead[1] = me[1] + 1.0;
					SEffectManager.PlaySound("AZ_CryptFar_SoundSet", ahead);
				}
				m_AZCryptBreathIn = Math.RandomFloatInclusive(8, 15);
			}
		}
		else
			m_AZCryptBreathIn = 0;
		if (upper && l[0] >= 0.3 && !m_AZCryptRevealed)
		{
			m_AZCryptRevealed = true;
			SEffectManager.PlaySoundOnObject("AZ_CryptReveal_SoundSet", this);
			AZ_VampWhisperLines.s_QuietUntil = GetGame().GetTime() + 12000;
			AZ_VampWhisperLines.s_QuietDucked = true;
			AZ_VampWhisperLines.Duck(0.35, 1.5);
		}
		if (m_AZCryptBonesIn <= 0)
		{
			m_AZCryptBonesIn = Math.RandomFloatInclusive(20, 60);
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(GetPosition(), 20, near, null);
			foreach (Object o : near)
			{
				if (o && o.IsKindOf("AZ_VampSkull_Pile"))
				{
					SEffectManager.PlaySound("AZ_CryptBones_SoundSet", o.GetPosition());
					break;
				}
			}
		}
	}

	protected void AZ_CryptSoundStop(EffectSound snd)
	{
		if (!snd)
			return;
		snd.SoundStop();
		SEffectManager.DestroyEffect(snd);
	}
}


class ActionAZPressSkull : ActionInteractBase
{
	void ActionAZPressSkull()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Press the skull";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{


		return player && AZ_VampCrypt.AtSkull(player);
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampCrypt.PressSkull(action_data.m_Player);
	}
}

class AZ_VampAltarBasin extends AZ_VampLitDecor
{
}


modded class AZ_VampLitDecor
{
	protected float m_AZFlickerFrom;

	void AZ_Flicker()
	{
		if (!m_AZLight || m_AZFlickerFrom > 0)
			return;
		m_AZFlickerFrom = m_AZLight.m_Brightness;
		m_AZLight.SetBrightnessTo(m_AZFlickerFrom * 0.12);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_FlickerEnd, 1400, false);
	}

	void AZ_FlickerEnd()
	{
		if (m_AZLight && m_AZFlickerFrom > 0)
			m_AZLight.SetBrightnessTo(m_AZFlickerFrom);
		m_AZFlickerFrom = 0;
	}
}


modded class Weapon_Base
{
	override void EEFired(int muzzleType, int mode, string ammoType)
	{
		super.EEFired(muzzleType, mode, ammoType);
		if (!GetGame().IsServer())
			return;
		PlayerBase shooter = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (shooter && shooter.m_AZCryptGraceUntil != 0)
			AZ_VampCrypt.GraceEnd(shooter, "fired");
		if (shooter)
			shooter.m_AZGuestUntil = 0;
		if (shooter && shooter.m_AZCryptTransit)
		{
			AZ_VampCrypt.TransitEnd(shooter);
			shooter.m_AZCryptTransitForfeit = true;
		}
	}
}


modded class AZ_VampConfig
{
	bool cryptAnte = true;
	bool tombDoor = true;
	bool cryptEntrance = true;
	string altarLayout = "box";
	bool riverFlow = false;
}





class AZ_VampAltar_Drum extends House
{
	protected int m_AZEyeLevel;
	protected int m_AZEyeSeen = -1;

	void AZ_VampAltar_Drum()
	{
		RegisterNetSyncVariableInt("m_AZEyeLevel", 0, 3);
	}

	void AZ_SetEyeLevel(int level)
	{
		if (level == m_AZEyeLevel)
			return;
		m_AZEyeLevel = level;
		SetSynchDirty();
		AZ_ApplyEyes();
		Print("[AZ_Vamp] drum altar eyes " + level);
	}

	int AZ_EyeLevel()
	{
		return m_AZEyeLevel;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		AZ_ApplyEyes();
	}



	protected Object m_AZEyeOverlay;

	void AZ_ApplyEyes()
	{
		if (m_AZEyeLevel == m_AZEyeSeen)
			return;
		m_AZEyeSeen = m_AZEyeLevel;
		if (GetGame().IsDedicatedServer())
			return;
		if (m_AZEyeOverlay)
			GetGame().ObjectDelete(m_AZEyeOverlay);
		m_AZEyeOverlay = null;
		if (m_AZEyeLevel > 0)
		{
			int lv = Math.Min(m_AZEyeLevel, 3);
			m_AZEyeOverlay = GetGame().CreateObjectEx("AZ_Crypt_Countess_Drum_Eyes_" + lv, GetPosition(), ECE_LOCAL | ECE_NOLIFETIME);
			if (m_AZEyeOverlay)
			{
				vector tm[4];
				GetTransform(tm);
				m_AZEyeOverlay.SetTransform(tm);
				m_AZEyeOverlay.SetScale(GetScale());
			}
		}
		Print("[AZ_Vamp] drum altar eyes (client) overlay level " + m_AZEyeLevel + " (" + (m_AZEyeOverlay != null) + ")");
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_AZEyeOverlay)
			GetGame().ObjectDelete(m_AZEyeOverlay);
	}
}


class AZ_Crypt_Tomb_Grille extends House
{
	protected bool m_AZExitOnly;

	void AZ_Crypt_Tomb_Grille()
	{
		RegisterNetSyncVariableBool("m_AZExitOnly");
	}

	void AZ_SetExitOnly(bool on)
	{
		m_AZExitOnly = on;
		SetSynchDirty();
	}

	bool AZ_ExitOnly()
	{
		return m_AZExitOnly;
	}
}


class AZ_Crypt_Countess extends House
{
	static const string EYES_RVMAT = "azo\az_vampire_mask\data\crypt\az_crypt_countess_eyes_glow.rvmat";
	protected bool m_AZEyes;
	protected bool m_AZEyesSeen;
	protected ref array<Object> m_AZOverlays;

	void AZ_Crypt_Countess()
	{
		RegisterNetSyncVariableBool("m_AZEyes");
	}

	void AZ_SetEyes(bool on)
	{
		if (m_AZEyes == on)
			return;
		m_AZEyes = on;
		SetSynchDirty();
	}

	void AZ_SpawnHalo()
	{
		if (!m_AZEyes || !m_AZOverlays)
			return;
		Object halo = GetGame().CreateObjectEx("AZ_Crypt_Countess_Halo", GetPosition(), ECE_LOCAL | ECE_NOLIFETIME);
		if (!halo)
			return;
		vector tm[4];
		GetTransform(tm);
		halo.SetTransform(tm);
		halo.SetScale(GetScale());
		m_AZOverlays.Insert(halo);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_AZEyes == m_AZEyesSeen)
			return;
		m_AZEyesSeen = m_AZEyes;




		if (m_AZEyes && !m_AZOverlays)
		{
			m_AZOverlays = new array<Object>;
			vector tm[4];
			GetTransform(tm);
			array<string> kinds = {"AZ_Crypt_Countess_Eyes", "AZ_Crypt_Countess_Lips", "AZ_Crypt_Countess_Chalice"};
			foreach (string kind : kinds)
			{
				Object ov = GetGame().CreateObjectEx(kind, GetPosition(), ECE_LOCAL | ECE_NOLIFETIME);
				if (!ov)
					continue;
				ov.SetTransform(tm);
				ov.SetScale(GetScale());
				m_AZOverlays.Insert(ov);
			}
			Print("[AZ_Vamp] countess overlays on: " + m_AZOverlays.Count() + " of 3 at " + GetPosition());

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_SpawnHalo, 1000, false);
		}
		else if (!m_AZEyes && m_AZOverlays)
		{
			foreach (Object old : m_AZOverlays)
			{
				if (old)
					GetGame().ObjectDelete(old);
			}
			m_AZOverlays = null;
		}
	}
}
