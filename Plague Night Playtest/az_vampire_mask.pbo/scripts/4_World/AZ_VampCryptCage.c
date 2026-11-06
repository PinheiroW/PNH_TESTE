











class AZ_VampCryptCage
{




	static const float CACHE_F = -8.5;
	static const float CACHE_R = 5.6;
	static const float CAGE_F = -8.5;
	static const float CAGE_R = 4.85;
	static const bool GRAB_TRIGGER = true;
	static const float HALF = 1.4;
	static const float ZONE = 1.0;
	static const float DOOR_HALF = 0.45;
	static const int DOOR_MS = 6000;
	static const float APPROACH = 3.5;
	static const float LEAD_S = 0.7;
	static const float LEAD_ZONE = 0.5;
	static const float LEAD_SIDE = 1.0;
	static const int FALL_MS = 450;
	static bool s_Landed;
	static const float WINCH_DF = 0.0;
	static const float WINCH_DR = -2.2;
	static const float REACH = 1.0;
	static const int TELL_MS = 250;
	static const int HOLD_MS = 90000;
	static const int COOLDOWN_MS = 600000;
	static const int TICK_MS = 100;

	static const int UP = 0;
	static const int TELL = 1;
	static const int DOWN = 2;

	static AZ_Crypt_Trap_Cage s_Cage;
	static AZ_Crypt_Trap_Winch s_Winch;
	static int s_State;
	static int s_ReadyAt;
	static int s_Gen;
	static int s_HoldMs = HOLD_MS;
	static int s_Drops;
	static ref array<PlayerBase> s_Probe;
	static ref map<PlayerBase, vector> s_Last;
	static ref map<PlayerBase, int> s_LastT;
	static ref map<PlayerBase, vector> s_Vel;
	static int s_LastCaught;
	static float s_PBlood;
	static const int LURK_WAIT_MS = 15000;
	static int s_LurkWaitMs = LURK_WAIT_MS;
	static const int LURK_QUICK_MS = 1000;
	static int s_OpenedDrop = -1;
	static const int LINGER_MS = 35000;
	static int s_LingerMs = LINGER_MS;
	static const int LINGER_TICK_MS = 2000;
	static const float CIRCLE_F = 2.1;
	static const float CIRCLE_R = -1.85;

	static void Build()
	{
		if (AZ_VampCryptTraps.s_RoomSealed)
		{
			Print("[AZ_Vamp] crypt cage: not built (trap room sealed)");
			return;
		}
		float y = AZ_VampCryptTraps.FloorY(CAGE_F, CAGE_R);
		s_Cage = AZ_Crypt_Trap_Cage.Cast(AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Cage", AZ_VampCrypt.Hall(CAGE_F, CAGE_R, y), AZ_VampCrypt.HALL_YAW));
		float wy = AZ_VampCryptTraps.FloorY(CAGE_F + WINCH_DF, CAGE_R + WINCH_DR);
		s_Winch = AZ_Crypt_Trap_Winch.Cast(AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Winch", AZ_VampCrypt.Hall(CAGE_F + WINCH_DF, CAGE_R + WINCH_DR, wy), AZ_VampCrypt.HALL_YAW));
		AZ_VampCrypt.HallProp("AZ_VampSkeleton_Human", CAGE_F + 0.4, CAGE_R + 0.3, 0, 70);


		AZ_VampCryptLoot.Piece(AZ_VampCrypt.Hall(CACHE_F, CACHE_R, AZ_VampCryptTraps.FloorY(CACHE_F, CACHE_R) + 0.05), "cage cache");
		s_Last = new map<PlayerBase, vector>;
		s_LastT = new map<PlayerBase, int>;
		s_Vel = new map<PlayerBase, vector>;
		s_State = UP;
		s_ReadyAt = 0;
		s_Gen++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, s_Gen);
		Print("[AZ_Vamp] crypt cage built: cage " + (s_Cage != null) + " winch " + (s_Winch != null) + " floor " + y + ", on " + AZ_VampCryptTraps.On());
	}


	static vector Local(vector pos)
	{
		vector l = AZ_VampCrypt.HallLocal(pos);
		return Vector(l[0] - CAGE_F, l[1] - CAGE_R, l[2]);
	}

	static bool InZone(vector pos, float half)
	{
		vector l = Local(pos);
		return Math.AbsFloat(l[0]) < half && Math.AbsFloat(l[1]) < half && l[2] > -1.0 && l[2] < 2.5;
	}

	static void Tick(int gen)
	{
		if (gen != s_Gen)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, gen);
		if (s_State != UP || !s_Cage || !AZ_VampCryptTraps.On() || GetGame().GetTime() < s_ReadyAt || GRAB_TRIGGER)
			return;
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		if (s_Probe)
		{
			foreach (PlayerBase d : s_Probe)
			{
				if (d)
					list.Insert(d);
			}
		}
		map<PlayerBase, vector> seen = new map<PlayerBase, vector>;
		foreach (PlayerBase p : list)
		{
			if (!p || !AZ_VampCryptTraps.IsHuman(p))
				continue;
			vector pos = p.GetPosition();
			vector l = Local(pos);
			if (Math.AbsFloat(l[0]) > HALF + APPROACH || Math.AbsFloat(l[1]) > HALF + 0.5 || l[2] < -1.0 || l[2] > 2.5)
				continue;


			int now = GetGame().GetTime();
			vector v = "0 0 0";
			if (s_Vel.Contains(p))
				v = s_Vel.Get(p);
			if (s_Last && s_Last.Contains(p) && s_LastT.Contains(p) && now - s_LastT.Get(p) >= 200)
			{
				v = (pos - s_Last.Get(p)) * (1000.0 / (now - s_LastT.Get(p)));
				v[1] = 0;
				if (v.Length() > 8)
					v = "0 0 0";
				s_Vel.Set(p, v);
				seen.Set(p, pos);
				s_LastT.Set(p, now);
			}
			else if (s_Last && s_Last.Contains(p))
				seen.Set(p, s_Last.Get(p));
			else
			{
				seen.Set(p, pos);
				s_LastT.Set(p, now);
			}
			vector lead = pos + v * LEAD_S;
			vector ll = Local(lead);
			bool aimed = Math.AbsFloat(ll[0]) < LEAD_ZONE && Math.AbsFloat(ll[1]) < LEAD_SIDE && ll[2] > -1.0 && ll[2] < 2.5;
			bool still = v.Length() < 1.0 && InZone(pos, ZONE - 0.3);
			if (aimed || still)
			{
				s_Last = seen;
				Tell(p);
				return;
			}
		}
		s_Last = seen;
	}


	static void OnGrab(PlayerBase p)
	{
		if (!p || !s_Cage)
			return;
		string why = "";
		if (!AZ_VampCryptTraps.On())
			why = "traps off";
		else if (s_State != UP)
			why = "not hung";
		else if (GetGame().GetTime() < s_ReadyAt)
			why = "cooldown";
		else if (!AZ_VampCryptTraps.IsHuman(p))
			why = "not a human";
		else if (!InZone(p.GetPosition(), HALF))
			why = "not under it";
		if (why != "")
		{
			Print("[AZ_Vamp] crypt cage: " + AZ_VampState.Uid(p) + " took from the cache, no drop (" + why + ")");
			return;
		}
		Print("[AZ_Vamp] crypt cage: " + AZ_VampState.Uid(p) + " took from the cache");
		Tell(p);
	}

	static vector CacheAt()
	{
		return AZ_VampCrypt.Hall(CACHE_F, CACHE_R, 0);
	}


	static bool CanSearch(PlayerBase p, Object target)
	{
		if (!p || !target || !AZ_VampCryptTraps.IsHuman(p))
			return false;
		vector c = CacheAt();
		vector t = target.GetPosition();
		vector me = p.GetPosition();
		t[1] = c[1];
		me[1] = c[1];
		return vector.Distance(t, c) < 1.0 && vector.Distance(me, c) < 2.2;
	}

	static void Tell(PlayerBase p)
	{
		s_State = TELL;
		s_Landed = false;
		s_Cage.SetCageState(TELL);
		Print("[AZ_Vamp] crypt cage tell: " + AZ_VampState.Uid(p) + " under it");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Drop, TELL_MS, false, s_Gen);
	}

	static void Drop(int gen)
	{
		if (gen != s_Gen || s_State != TELL || !s_Cage)
			return;
		int now = GetGame().GetTime();
		s_State = DOWN;
		s_Drops++;
		s_ReadyAt = now + COOLDOWN_MS;
		s_Cage.SetCageState(DOWN);
		if (s_Winch)
			s_Winch.SetDown(true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Landed, FALL_MS, false, gen);
	}


	static void Landed(int gen)
	{
		if (gen != s_Gen || s_State != DOWN)
			return;
		s_Landed = true;
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		if (s_Probe)
		{
			foreach (PlayerBase d : s_Probe)
			{
				if (d)
					list.Insert(d);
			}
		}
		string caught = "";
		int told = 0;
		PlayerBase first;
		s_LastCaught = 0;
		foreach (PlayerBase p : list)
		{
			if (!p)
				continue;
			if (InZone(p.GetPosition(), HALF) && AZ_VampCryptTraps.IsHuman(p))
			{
				caught = caught + AZ_VampState.Uid(p) + " ";
				s_LastCaught++;
				if (!first)
					first = p;
				if (p.GetIdentity())
					p.Message("Iron bars slam down around you.", "colorImportant");
			}
			else if ((p.m_AZVampire || AZ_VampState.IsVampire(p)) && AZ_VampCrypt.Inside(p.GetPosition()) && p.GetIdentity())
			{
				p.Message("Someone's caught... the cage by the crawl has fallen.", "colorImportant");
				told++;
			}
		}
		if (AZ_VampCryptTraps.s_Alarm)
			AZ_VampCryptTraps.s_Alarm.Ring();
		Print("[AZ_Vamp] crypt cage dropped, caught " + caught + "| " + told + " vampires told, auto-release in " + (s_HoldMs / 1000) + " s");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AutoRise, s_HoldMs, false, s_Gen, s_Drops);
		if (first)
		{
			int wait = s_LurkWaitMs;
			if (AZ_VampCryptLurkers.PlayerVampireInside() == "")
				wait = Math.Min(wait, LURK_QUICK_MS);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LurkerCheck, wait, false, first, s_Gen, s_Drops);
		}
	}



	static void LurkerCheck(PlayerBase p, int gen, int drop)
	{
		if (gen != s_Gen || drop != s_Drops || s_State != DOWN)
			return;
		string who = "?";
		if (p)
			who = AZ_VampState.Uid(p);
		if (s_OpenedDrop == drop)
		{
			Print("[AZ_Vamp] crypt cage: " + who + " still caught, the door was opened, no lurker");
			return;
		}
		if (!p || !p.IsAlive() || !HoldsHuman(p) || p.m_AZVampHeld)
		{
			Print("[AZ_Vamp] crypt cage: no lurker, " + who + " is no longer a free human inside");
			return;
		}
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase v : list)
		{
			if (v && v.IsAlive() && (v.m_AZVampire || AZ_VampState.IsVampire(v)) && AZ_VampCrypt.Inside(v.GetPosition()))
			{
				Print("[AZ_Vamp] crypt cage: " + who + " still caught, a vampire player is in the crypt (" + AZ_VampState.Uid(v) + "), no lurker");
				return;
			}
		}
		Print("[AZ_Vamp] crypt cage: " + who + " caught " + (s_LurkWaitMs / 1000) + " s, nobody answered: a lurker comes");
		AZ_VampCryptLurkers.CageAmbush(p);
	}

	static bool HoldsHuman(PlayerBase p)
	{
		return p && InZone(p.GetPosition(), HALF) && AZ_VampCryptTraps.IsHuman(p);
	}


	static vector DoorOutside()
	{
		return AZ_VampCrypt.Hall(CAGE_F - HALF - 0.7, CAGE_R, 0);
	}

	static vector DoorInside()
	{
		return AZ_VampCrypt.Hall(CAGE_F - HALF + 0.5, CAGE_R, 0);
	}

	static vector InwardDir()
	{
		vector d = AZ_VampCrypt.Hall(1, 0, 0) - AZ_VampCrypt.Hall(0, 0, 0);
		d[1] = 0;
		return d.Normalized();
	}

	static void AutoRise(int gen, int drop)
	{
		if (gen == s_Gen && drop == s_Drops && s_State == DOWN)
			Rise("auto-release");
	}

	static void Rise(string why)
	{
		if (s_State != DOWN || !s_Cage)
			return;
		s_State = UP;
		s_Cage.SetCageState(UP);
		s_Cage.SetDoorOpen(false);
		if (s_Winch)
			s_Winch.SetDown(false);
		Print("[AZ_Vamp] crypt cage up: " + why + ", re-armed in " + Math.Max(0, (s_ReadyAt - GetGame().GetTime()) / 1000) + " s");
	}




	static bool CanOpen(PlayerBase p, Object target)
	{
		AZ_Crypt_Trap_Cage c = AZ_Crypt_Trap_Cage.Cast(target);
		if (!p || !c || c.GetCageState() != DOWN || c.IsDoorOpen() || !p.m_AZVampire)
			return false;
		vector l = Local(p.GetPosition());
		return l[0] < -HALF && l[0] > -HALF - REACH && Math.AbsFloat(l[1]) < DOOR_HALF + 0.4;
	}


	static void OpenDoor(string who, bool hold = false)
	{
		if (!s_Cage || s_State != DOWN)
			return;
		s_Cage.SetDoorOpen(true);
		s_OpenedDrop = s_Drops;
		Print("[AZ_Vamp] crypt cage door opened by " + who + ", left open " + hold);
		if (!hold)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CloseDoor, DOOR_MS, false, s_Drops);
	}

	static bool IsDoorOpen()
	{
		return s_Cage && s_Cage.IsDoorOpen();
	}


	static vector CirclePoint(float t)
	{
		float a = Math.PI * Math.AbsFloat(1.0 - 2.0 * t);
		return AZ_VampCrypt.Hall(CAGE_F + CIRCLE_F * Math.Cos(a), CAGE_R + CIRCLE_R * Math.Sin(a), 0);
	}


	static void WatchLinger(PlayerBase p)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LingerTick, LINGER_TICK_MS, false, p, s_Gen, s_Drops, GetGame().GetTime());
	}

	static void LingerTick(PlayerBase p, int gen, int drop, int from)
	{
		if (!p || !p.IsAlive() || gen != s_Gen || drop != s_Drops || s_State != DOWN || !AZ_VampCryptLurkers.Hunted(p))
			return;
		bool near = InZone(p.GetPosition(), HALF + 1.0) && AZ_VampCryptTraps.IsHuman(p);
		if (near && GetGame().GetTime() - from >= s_LingerMs)
		{
			Print("[AZ_Vamp] crypt cage: " + AZ_VampState.Uid(p) + " stayed at the cage " + (s_LingerMs / 1000) + " s after the taste: the hunt");
			AZ_VampCryptLurkers.CageHunt(p);
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LingerTick, LINGER_TICK_MS, false, p, gen, drop, from);
	}

	static void CloseDoor(int drop)
	{
		if (s_Cage && drop == s_Drops)
			s_Cage.SetDoorOpen(false);
	}

	static bool CanWinch(PlayerBase p, Object target)
	{
		AZ_Crypt_Trap_Winch w = AZ_Crypt_Trap_Winch.Cast(target);
		if (!p || !w || !w.IsDown() || p.m_AZVampire || p.m_AZVampTurning)
			return false;
		if (InZone(p.GetPosition(), HALF + 0.2))
			return false;
		return vector.Distance(p.GetPosition(), w.GetPosition()) < REACH + 0.4;
	}


	static void ForceArm()
	{
		s_Gen++;
		s_State = UP;
		s_ReadyAt = 0;
		if (s_Cage)
			s_Cage.SetCageState(UP);
		if (s_Winch)
			s_Winch.SetDown(false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, s_Gen);
	}

	static void ForceDown()
	{
		if (!s_Cage)
			return;
		s_Gen++;
		s_State = DOWN;
		s_ReadyAt = GetGame().GetTime() + COOLDOWN_MS;
		s_Cage.SetCageState(DOWN);
		if (s_Winch)
			s_Winch.SetDown(true);
	}


	static bool BarsBlock()
	{
		vector from = AZ_VampCrypt.Hall(CAGE_F + HALF + 1.0, CAGE_R, 1.0);
		vector to = AZ_VampCrypt.Hall(CAGE_F, CAGE_R, 1.0);
		vector pos, dir;
		int comp;
		set<Object> hits = new set<Object>;
		DayZPhysics.RaycastRV(from, to, pos, dir, comp, hits, null, null, false, false, ObjIntersectGeom);
		foreach (Object o : hits)
		{
			if (o == s_Cage)
				return true;
		}
		return false;
	}

	static PlayerBase Dummy(float dr, int seed)
	{
		vector at = AZ_VampCrypt.Hall(CAGE_F, CAGE_R + dr, 0.05);
		PlayerBase d = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Male(seed), at, false, true));
		if (!d)
			return null;
		AZ_VampDress.Survivor(d, seed);
		d.SetPosition(at);
		if (!s_Probe)
			s_Probe = new array<PlayerBase>;
		s_Probe.Insert(d);
		return d;
	}

	static void Put(PlayerBase d, float dr)
	{
		if (d)
			d.SetPosition(AZ_VampCrypt.Hall(CAGE_F, CAGE_R + dr, 0.05));
	}

	static string Verdict(bool ok)
	{
		if (ok)
			return "PASS";
		return "FAIL";
	}

	static void Probe()
	{
		if (AZ_VampCryptTraps.s_RoomSealed)
		{
			Print("[AZ_Vamp] cage probe: SKIP (entrance off)");
			Print("[AZ_Vamp] cage probe: done");
			return;
		}
		if (!s_Cage)
		{
			Print("[AZ_Vamp] cage probe: FAIL no cage");
			return;
		}
		ForceArm();
		s_HoldMs = 4000;
		PlayerBase vamp = Dummy(-3.0, 4);
		PlayerBase human = Dummy(-3.5, 5);
		if (!vamp || !human)
		{
			Print("[AZ_Vamp] cage probe: FAIL spawn");
			return;
		}
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		Print("[AZ_Vamp] cage probe: start, bars block while hung " + BarsBlock() + " (expect false)");
		Put(vamp, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 1500, false, 1, vamp, human);
	}


	static void ProbeStep(int step, PlayerBase vamp, PlayerBase human)
	{
		int next = step + 1;
		int wait = 1500;
		bool ok;
		switch (step)
		{
			case 1:
				ok = s_State == UP;
				Print("[AZ_Vamp] cage probe: vampire under the cage -> state " + s_State + " " + Verdict(ok) + " (expect it stays hung)");
				Put(vamp, -3.0);
				Put(human, 0);
				wait = 1000;
				break;
			case 2:
				ok = s_State == UP;
				Print("[AZ_Vamp] cage probe: human walked in under it, 1 s -> state " + s_State + " " + Verdict(ok) + " (expect it stays hung: only the grab drops it)");
				OnGrab(human);
				wait = 1200;
				break;
			case 3:
				ok = s_State == DOWN && BarsBlock();
				Print("[AZ_Vamp] cage probe: 1.5 s -> state " + s_State + " bars block " + BarsBlock() + " " + Verdict(ok) + " (expect down, bars hit at 1 m)");
				ok = !CanWinch(human, s_Winch) && CanOpen(vamp, s_Cage) == false;
				Print("[AZ_Vamp] cage probe: from inside, winch " + CanWinch(human, s_Winch) + "; vampire 3 m off, open " + CanOpen(vamp, s_Cage) + " " + Verdict(ok) + " (expect neither)");
				PutAt(vamp, -HALF - 0.6, 0);
				ok = CanOpen(vamp, s_Cage);
				Print("[AZ_Vamp] cage probe: vampire at the door, open " + ok + " " + Verdict(ok) + " (expect yes)");
				OpenDoor("probe");
				ok = s_Cage.IsDoorOpen() && s_State == DOWN;
				Print("[AZ_Vamp] cage probe: door opened -> door open " + s_Cage.IsDoorOpen() + " cage state " + s_State + " " + Verdict(ok) + " (expect open, cage still down)");
				ProbeFeedFit(vamp, human);
				wait = 1000;
				break;
			case 31:
				s_Cage.SetDoorOpen(false);
				Rise("probe winch");
				wait = 1000;
				next = 4;
				break;
			case 4:
				ok = s_State == UP && !BarsBlock();
				Print("[AZ_Vamp] cage probe: winched -> state " + s_State + " bars block " + BarsBlock() + " " + Verdict(ok) + " (expect up, clear)");
				Put(vamp, -3.0);
				s_ReadyAt = 0;
				OnGrab(human);
				wait = 1500;
				break;
			case 5:
				ok = s_State == DOWN;
				Print("[AZ_Vamp] cage probe: second drop on the human -> state " + s_State + " " + Verdict(ok) + " (expect down)");
				wait = 5000;
				break;
			case 6:
				ok = s_State == UP;
				Print("[AZ_Vamp] cage probe: after the 4 s probe hold -> state " + s_State + " " + Verdict(ok) + " (expect auto-release)");
				wait = 1500;
				break;
			case 7:
				ok = s_State == UP;
				OnGrab(human);
				ok = s_State == UP;
				Print("[AZ_Vamp] cage probe: human under it grabs again inside the cooldown -> state " + s_State + " " + Verdict(ok) + " (expect it stays hung)");

				s_ReadyAt = 0;
				Put(human, -(HALF + 0.5));
				OnGrab(human);
				wait = 1000;
				next = 91;
				break;
			case 8:

				s_ReadyAt = 0;
				Put(human, 0);
				s_JogDR = 0.2;
				human.SetPosition(AZ_VampCrypt.Hall(CAGE_F - 4.0, CAGE_R + s_JogDR, 0.05));
				s_Last.Clear();
				s_LastT.Clear();
				s_Vel.Clear();
				s_Landed = false;
				s_LastCaught = 0;
				Jog(human, CAGE_F - 4.0, s_Gen);
				wait = 3500;
				break;
			case 9:
				ok = s_LastCaught > 0 && s_State == DOWN;
				Print("[AZ_Vamp] cage probe: jogger at 3.5 m/s -> state " + s_State + " caught " + s_LastCaught + " at hall " + AZ_VampCrypt.HallLocal(human.GetPosition()) + " " + Verdict(ok) + " (expect the bars land on them)");

				ForceArm();
				s_ReadyAt = 0;
				s_Last.Clear();
				s_LastT.Clear();
				s_Vel.Clear();
				s_Landed = false;
				s_LastCaught = 0;
				s_JogDR = HALF + 0.5;
				human.SetPosition(AZ_VampCrypt.Hall(CAGE_F - 4.0, CAGE_R + s_JogDR, 0.05));
				Jog(human, CAGE_F - 4.0, s_Gen);
				wait = 3500;
				next = 91;
				break;
			case 91:
				ok = s_State == UP;
				Print("[AZ_Vamp] cage probe: a grab from outside the bars -> state " + s_State + " " + Verdict(ok) + " (expect it stays hung)");
				ForceArm();
				s_ReadyAt = 0;
				Put(human, 0);
				AZ_VampState.Cfg().cryptTraps = false;
				OnGrab(human);
				wait = 1500;
				next = 10;
				break;
			case 10:
				ok = s_State == UP;
				Print("[AZ_Vamp] cage probe: kill switch, human under it -> state " + s_State + " " + Verdict(ok) + " (expect it stays hung)");
				AZ_VampState.Cfg().cryptTraps = true;


				if (vamp)
					GetGame().ObjectDelete(vamp);
				AZ_VampCryptLurkers.Reset();
				AZ_VampCryptLurkers.RefillBeds();
				ForceArm();
				s_ReadyAt = 0;
				s_HoldMs = 60000;
				s_LurkWaitMs = 2000;
				Put(human, 0);
				human.SetHealth("", "Shock", 100);
				Tell(human);
				s_LingerMs = 3000;
				wait = 12700;
				next = 11;
				break;
			case 11:
				int near = 0;
				array<Object> objs = new array<Object>;
				GetGame().GetObjectsAtPosition(human.GetPosition(), 1.5, objs, null);
				foreach (Object o : objs)
				{
					PlayerBase pb = PlayerBase.Cast(o);
					if (pb && pb != human && pb.IsAlive())
						near++;
				}
				bool doorOpened = s_OpenedDrop == s_Drops;
				ok = doorOpened && human.m_AZVampHeld && near >= 1 && s_State == DOWN;
				Print("[AZ_Vamp] cage probe: held, nobody answered -> door opened " + doorOpened + " held " + human.m_AZVampHeld + " feeders near " + near + " state " + s_State + " " + Verdict(ok) + " (expect a lurker circled, opened the door and takes a taste inside)");
				s_PBlood = human.GetHealth("", "Blood");
				wait = 3000;
				next = 12;
				break;
			case 12:
				ok = !human.m_AZVampHeld && human.GetHealth("", "Shock") > 25 && IsDoorOpen() && s_State == DOWN && AZ_VampCryptLurkers.Hunted(human);
				Print("[AZ_Vamp] cage probe: after the taste -> held " + human.m_AZVampHeld + " blood " + human.GetHealth("", "Blood") + " shock " + human.GetHealth("", "Shock") + " door open " + IsDoorOpen() + " state " + s_State + " hunted " + AZ_VampCryptLurkers.Hunted(human) + " " + Verdict(ok) + " (expect let go standing, the door left open, the cage still down)");
				AZ_VampCryptLurkers.s_ReadyAt = 0;
				wait = 13000;
				next = 13;
				break;
			case 13:
				ok = s_State == UP && !human.m_AZVampHeld && human.GetHealth("", "Shock") < 25 && !human.m_AZVampTurning;
				Print("[AZ_Vamp] cage probe: stayed in the open cage -> the hunt: state " + s_State + " held " + human.m_AZVampHeld + " blood " + human.GetHealth("", "Blood") + " shock " + human.GetHealth("", "Shock") + " turning " + human.m_AZVampTurning + " " + Verdict(ok) + " (expect the full feed, down, not sick, then the cage up)");
				s_LingerMs = LINGER_MS;
				s_LurkWaitMs = LURK_WAIT_MS;
				AZ_VampCryptLurkers.Reset();
				Put(human, 3.5);
				s_HoldMs = HOLD_MS;
				foreach (PlayerBase d : s_Probe)
				{
					if (d)
						GetGame().ObjectDelete(d);
				}
				s_Probe = null;
				ForceArm();
				Print("[AZ_Vamp] cage probe: done");
				return;
		}
		if (step == 3)
			next = 31;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, wait, false, next, vamp, human);
	}




	static void ProbeFeedFit(PlayerBase vamp, PlayerBase human)
	{
		PlayerBase vamp2 = Dummy(3.2, 6);
		if (!vamp2)
			return;
		AZ_VampState.SetVampire(vamp2, true);
		AZ_VampBind.Bind(vamp2);
		vector dir = AZ_VampCrypt.Hall(1, 0, 0) - AZ_VampCrypt.Hall(0, 0, 0);
		dir[1] = 0;
		dir.Normalize();
		human.SetDirection(dir);
		vector hp = human.GetPosition();
		vamp.SetPosition(hp - dir * AZ_VampFeed.ROOT_GAP);
		vamp2.SetPosition(hp + dir * (AZ_VampFeed.ROOT_GAP + AZ_VampState.Cfg().frontGapMale));
		bool ok = true;
		string where = "";
		array<PlayerBase> three = new array<PlayerBase>;
		three.Insert(human);
		three.Insert(vamp);
		three.Insert(vamp2);
		foreach (PlayerBase d : three)
		{
			vector l = Local(d.GetPosition());
			bool clear = Math.AbsFloat(l[0]) < HALF - 0.3 && Math.AbsFloat(l[1]) < HALF - 0.3;
			ok = ok && clear;
			where = where + " <" + l[0] + ", " + l[1] + ">";
		}
		Print("[AZ_Vamp] cage probe: human + 2 vampires at the shared-feed spots, hall offsets" + where + " " + Verdict(ok) + " (expect all 0.3 m clear of the bars)");
	}

	static void PutAt(PlayerBase d, float df, float dr)
	{
		if (d)
			d.SetPosition(AZ_VampCrypt.Hall(CAGE_F + df, CAGE_R + dr, 0.05));
	}


	static float s_JogDR = 0.2;

	static void Jog(PlayerBase d, float f, int gen)
	{
		if (!d || gen != s_Gen || s_Landed || f > CAGE_F + 4.0)
			return;
		d.SetPosition(AZ_VampCrypt.Hall(f, CAGE_R + s_JogDR, 0.05));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Jog, 100, false, d, f + 0.35, gen);
	}
}


class AZ_Crypt_Trap_Cage extends House
{
	protected int m_CageState;
	protected int m_CageStateLocal;

	void AZ_Crypt_Trap_Cage()
	{
		RegisterNetSyncVariableInt("m_CageState", 0, 3);
		RegisterNetSyncVariableBool("m_DoorOpen");
	}




	protected ScriptedLightBase m_AZChainLight;

	void AZ_LightTick()
	{
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		bool want = false;
		if (me)
		{
			vector l = AZ_VampCrypt.HallLocal(me.GetPosition());
			want = l[0] < -1.0 && Math.AbsFloat(l[1]) < 6.25 && l[2] > -2.0 && l[2] < 6.0;
		}
		if (want && !m_AZChainLight)
			m_AZChainLight = AZ_VampLightGate.Light(AZ_VampCageChainLight, AZ_VampCrypt.Hall(AZ_VampCryptCage.CAGE_F, AZ_VampCryptCage.CAGE_R, 2.6));
		else if (!want && m_AZChainLight)
		{
			m_AZChainLight.Destroy();
			m_AZChainLight = null;
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_LightTick);
		if (m_AZChainLight)
			m_AZChainLight.Destroy();
	}

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_LightTick, 500, true);
		SetAnimationPhase("drop", Phase());
		m_CageStateLocal = m_CageState;
	}

	int GetCageState()
	{
		return m_CageState;
	}

	float Phase()
	{
		if (m_CageState == AZ_VampCryptCage.DOWN)
			return 1;
		return 0;
	}

	protected bool m_DoorOpen;
	protected bool m_DoorOpenLocal;

	bool IsDoorOpen()
	{
		return m_DoorOpen;
	}

	float DoorPhase()
	{
		if (m_DoorOpen)
			return 1;
		return 0;
	}

	void SetDoorOpen(bool open)
	{
		if (!GetGame().IsServer() || open == m_DoorOpen)
			return;
		m_DoorOpen = open;
		SetAnimationPhase("door", DoorPhase());
		SetSynchDirty();
	}

	void SetCageState(int state)
	{
		if (!GetGame().IsServer() || state == m_CageState)
			return;
		m_CageState = state;
		SetAnimationPhase("drop", Phase());
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_DoorOpen != m_DoorOpenLocal)
		{
			m_DoorOpenLocal = m_DoorOpen;
			SetAnimationPhase("door", DoorPhase());
			if (GetGame().IsClient())
				SEffectManager.PlaySound("beartrapOpen_SoundSet", GetPosition() + "0 1 0");
		}
		if (m_CageState == m_CageStateLocal)
			return;
		int was = m_CageStateLocal;
		m_CageStateLocal = m_CageState;
		SetAnimationPhase("drop", Phase());
		if (!GetGame().IsClient())
			return;
		vector floor = GetPosition() + "0 0.05 0";
		if (m_CageState == AZ_VampCryptCage.TELL)
		{
			SEffectManager.PlaySound("AZ_CryptSlab_SoundSet", GetPosition() + "0 4 0");
			Particle.PlayInWorld(ParticleList.DUST_GROUND, floor);
		}
		else if (m_CageState == AZ_VampCryptCage.DOWN)
		{
			SEffectManager.PlaySound("beartrapClose_SoundSet", floor);
			Particle.PlayInWorld(ParticleList.DUST_CLOUD, floor);
		}
		else if (was == AZ_VampCryptCage.DOWN)
			SEffectManager.PlaySound("AZ_CryptSlab_SoundSet", GetPosition() + "0 2 0");
	}
}



class AZ_Crypt_Trap_Winch extends House
{
	protected bool m_Down;

	void AZ_Crypt_Trap_Winch()
	{
		RegisterNetSyncVariableBool("m_Down");
	}

	bool IsDown()
	{
		return m_Down;
	}

	void SetDown(bool down)
	{
		if (!GetGame().IsServer() || down == m_Down)
			return;
		m_Down = down;
		SetSynchDirty();
	}
}

class ActionAZCageOpenCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(3);
	}
}


class ActionAZCageOpen : ActionContinuousBase
{
	void ActionAZCageOpen()
	{
		m_CallbackClass = ActionAZCageOpenCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Open the cage";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return target && AZ_VampCryptCage.CanOpen(player, target.GetObject());
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptCage.OpenDoor(AZ_VampState.Uid(action_data.m_Player));
	}
}

class ActionAZCacheSearchCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(2);
	}
}


class ActionAZCacheSearch : ActionContinuousBase
{
	void ActionAZCacheSearch()
	{
		m_CallbackClass = ActionAZCacheSearchCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Search the cache";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return target && AZ_VampCryptCage.CanSearch(player, target.GetObject());
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptCage.OnGrab(action_data.m_Player);
	}
}

class ActionAZCageWinchCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(5);
	}
}


class ActionAZCageWinch : ActionContinuousBase
{
	void ActionAZCageWinch()
	{
		m_CallbackClass = ActionAZCageWinchCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Winch the cage up";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return target && AZ_VampCryptCage.CanWinch(player, target.GetObject());
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptCage.Rise("winched by " + AZ_VampState.Uid(action_data.m_Player));
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCageOpen);
		actions.Insert(ActionAZCageWinch);
		actions.Insert(ActionAZCacheSearch);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCageOpen, InputActionMap);
		AddAction(ActionAZCageWinch, InputActionMap);
		AddAction(ActionAZCacheSearch, InputActionMap);
	}
}
