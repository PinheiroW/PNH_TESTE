






















modded class AZ_VampConfig
{
	bool cryptTraps = true;
}

class AZ_VampCryptTraps
{
	static const int TICK_MS = 250;



	static const float SPIKE_F = 5.2;
	static const float SPIKE_R = 3.6;
	static const float SPIKE_Y = 0.9;
	static const float FAKE_F = -5.65;
	static const float FAKE_R = -0.5;
	static const int FAKE_CD_MS = 15000;
	static AZ_Crypt_Trap_Spikes s_Fake;
	static int s_FakeReadyAt;
	static int s_FakeClicks;
	static const float SPIKE_HF = 0.55;
	static const float SPIKE_HR = 0.6;
	static const float ALARM_F = -12.5;
	static const float ALARM_R = -1.05;
	static const float ALARM_HF = 0.6;
	static const float ALARM_HR = 0.5;
	static const int ALARM_COOLDOWN_MS = 90000;
	static const float CW_F = -12.3;
	static const float CW_R = -2.7;
	static const float CW_H0 = 0.2;
	static const float CW_SLOPE = 0.30614;
	static const float RUBBLE_F = -12.0;
	static const float RUBBLE_R = -4.89;
	static const float TRIG_F = -10.3;
	static const float TRIG_HF = 0.3;
	static const float TRIG_HR = 0.75;
	static const float BALL_F0 = -5.882;
	static const float BALL_Y0 = 2.635;
	static const float BALL_F1 = -12.532;
	static const float BALL_Y1 = 0.599;
	static const int RUMBLE_MS = 1000;
	static const int ROLL_MS = 2500;
	static const int ROLL_TICK_MS = 100;
	static const int BALL_BACK_MS = 60000;
	static const float BALL_HIT = 0.85;
	static const float BALL_DMG = 50;
	static const float BALL_FLOOR = 15;

	static const int B_READY = 0;
	static const int B_RUMBLE = 1;
	static const int B_ROLL = 2;
	static const int B_GONE = 3;
	static const int SPRING_MS = 600;
	static const int UP_MS = 8000;
	static const int REARM_MS = 45000;
	static const int WEDGE_MS = 600000;
	static const float SPIKE_DMG = 25;
	static const float SPIKE_FLOOR = 10;
	static const float LEG_CHANCE = 0.5;

	static const int ARMED = 0;
	static const int CLICKED = 1;
	static const int UP = 2;
	static const int DOWN = 3;

	static bool s_Built;
	static int s_Gen;
	static int s_SpikeState;
	static int s_SpikeAt;
	static int s_SpikeClickAt;
	static int s_WedgedUntil;
	static AZ_Crypt_Trap_Spikes s_Spikes;
	static AZ_Crypt_Trap_Alarm s_Alarm;
	static AZ_Crypt_Trap_Boulder s_Ball;
	static int s_BallState;
	static int s_RollAt;
	static float s_RollF;
	static int s_RollGen;
	static ref array<PlayerBase> s_RollHit;
	static int s_AlarmReadyAt;
	static int s_AlarmRings;
	static ref array<PlayerBase> s_Probe;

	static bool On()
	{
		return s_Built && AZ_VampState.Cfg() && AZ_VampState.Cfg().cryptTraps;
	}







	static bool s_RoomSealed;

	static void SealRoom(bool shut)
	{
		s_RoomSealed = shut;
		Print("[AZ_Vamp] crypt traps: trap room and arrival gallery sealed " + shut);
	}



	static void ClearSealedLoot()
	{
		array<vector> at = new array<vector>;
		float lf = LURE_F - 0.35;
		at.Insert(AZ_VampCrypt.Hall(lf, CW_R, Crown(lf)));
		at.Insert(AZ_VampCrypt.Hall(AZ_VampCryptCage.CACHE_F, AZ_VampCryptCage.CACHE_R, FloorY(AZ_VampCryptCage.CACHE_F, AZ_VampCryptCage.CACHE_R)));
		int gone = 0;
		foreach (vector c : at)
		{
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition3D(c, 2.0, near, null);
			foreach (Object o : near)
			{
				ItemBase it = ItemBase.Cast(o);
				if (!it || it.GetHierarchyParent())
					continue;
				Print("[AZ_Vamp] crypt traps (room sealed): removed a persisted " + it.GetType() + " at " + it.GetPosition());
				GetGame().ObjectDelete(it);
				gone++;
			}
		}
		Print("[AZ_Vamp] crypt traps (room sealed): " + gone + " persisted finds removed from the lure and the cage cache");
	}

	static void Build()
	{
		if (s_Built || !AZ_VampCrypt.s_Built)
			return;
		s_Built = true;
		s_SpikeState = ARMED;


		float spikeY = FloorYAt(SPIKE_F, SPIKE_R, SPIKE_Y);
		s_Fake = AZ_Crypt_Trap_Spikes.Cast(AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Spikes", AZ_VampCrypt.Hall(FAKE_F, FAKE_R, FloorY(FAKE_F, FAKE_R)), AZ_VampCrypt.HALL_YAW));
		float alarmY = FloorY(ALARM_F, ALARM_R);
		Object o = AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Spikes", AZ_VampCrypt.Hall(SPIKE_F, SPIKE_R, spikeY), AZ_VampCrypt.HALL_YAW);
		s_Spikes = AZ_Crypt_Trap_Spikes.Cast(o);
		float skullF = SPIKE_F - SPIKE_HF - 0.25;
		float skullR = SPIKE_R + SPIKE_HR + 0.3;
		AZ_VampCrypt.HallProp("AZ_VampSkull_Human", skullF, skullR, FloorYAt(skullF, skullR, SPIKE_Y), 180);
		if (s_RoomSealed)
		{
			if (s_Fake)
				GetGame().ObjectDelete(s_Fake);
			s_Fake = null;
			s_Gen++;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, s_Gen);
			Print("[AZ_Vamp] crypt traps built (room sealed): only the hall's spike plate, at " + AZ_VampCrypt.Hall(SPIKE_F, SPIKE_R, 0));
			AZ_VampCryptLoot.SideCaches();
			ClearSealedLoot();
			return;
		}
		o = AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Alarm", AZ_VampCrypt.Hall(ALARM_F, ALARM_R, alarmY), AZ_VampCrypt.HALL_YAW);
		s_Alarm = AZ_Crypt_Trap_Alarm.Cast(o);
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Rubble", AZ_VampCrypt.Hall(RUBBLE_F, RUBBLE_R, 0), AZ_VampCrypt.HALL_YAW);
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Causeway", AZ_VampCrypt.Hall(CW_F, CW_R, 0), AZ_VampCrypt.HALL_YAW);
		Route();
		SpawnBall();
		AZ_VampCryptLoot.SideCaches();
		s_Gen++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, s_Gen);
		Print("[AZ_Vamp] crypt traps built: spikes " + (s_Spikes != null) + " at " + AZ_VampCrypt.Hall(SPIKE_F, SPIKE_R, 0) + ", boulder " + (s_Ball != null) + " at " + AZ_VampCrypt.Hall(BALL_F0, CW_R, BALL_Y0) + ", alarm " + (s_Alarm != null) + " at " + AZ_VampCrypt.Hall(ALARM_F, ALARM_R, 0) + ", on " + On() + ", floor under spikes " + spikeY + " alarm " + alarmY);
	}







	static const string LURE_LOOT = "BloodBagFull,Morphine";
	static const float LURE_F = -8.2;

	static void Route()
	{


		float lf = LURE_F;

		AZ_VampCrypt.HallProp("AZ_VampSkull_Human", lf, CW_R - 0.1, Crown(lf), 200);
		array<string> loot = new array<string>;
		LURE_LOOT.Split(",", loot);

		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(AZ_VampCrypt.Hall(lf, CW_R, Crown(lf)), 1.5, near, null);
		for (int k = 0; k < loot.Count(); k++)
		{
			bool there = false;
			foreach (Object o : near)
			{
				if (o && o.GetType() == loot[k])
					there = true;
			}
			if (there)
				continue;
			float f = lf + 0.2 - k * 0.4;
			GetGame().CreateObjectEx(loot[k], AZ_VampCrypt.Hall(f, CW_R + 0.25, Crown(f) + 0.05), ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY);
		}

		float pf = lf - 0.7;
		AZ_VampCryptLoot.Piece(AZ_VampCrypt.Hall(pf, CW_R + 0.2, Crown(pf) + 0.06), "boulder lure");
		AZ_VampCrypt.HallProp("AZ_VampSkull_Human", AZ_VampCryptCage.CAGE_F - 0.5, AZ_VampCryptCage.CAGE_R - 0.6, 0, 40);
		AZ_VampCrypt.HallProp("AZ_VampSkull_Human", AZ_VampCryptCage.CAGE_F + 0.9, AZ_VampCryptCage.CAGE_R + 0.5, 0, 300);
		AZ_VampCrypt.HallProp("AZ_VampSkeleton_Human", -12.9, CW_R + 0.1, 0, 35);
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Hook", AZ_VampCrypt.Hall(ALARM_F + 0.7, ALARM_R, 0), AZ_VampCrypt.HALL_YAW);
		AZ_VampCrypt.HallProp("AZ_VampSkull_Human", -12.6, CW_R - 0.6, 0, 120);
		AZ_VampCrypt.HallProp("AZ_VampSkull_Human", -13.1, CW_R + 0.7, 0, 260);
		Print("[AZ_Vamp] crypt traps routing: no berm, lure at f " + lf + " on the crown " + Crown(lf));
	}


	static void FloorDiag(float f, float r)
	{
		array<int> kinds = {ObjIntersectView, ObjIntersectGeom, ObjIntersectFire};
		array<string> names = {"view", "geom", "fire"};
		for (int k = 0; k < 3; k++)
		{
			vector pos, dir;
			int comp;
			set<Object> hits = new set<Object>;
			bool hit = DayZPhysics.RaycastRV(AZ_VampCrypt.Hall(f, r, 2.0), AZ_VampCrypt.Hall(f, r, -2.0), pos, dir, comp, hits, null, null, true, false, kinds[k]);
			string what = "";
			foreach (Object o : hits)
			{
				if (o)
					what = what + o.GetType() + " ";
			}
			Print("[AZ_Vamp] crypt trap floor diag " + names[k] + " at f " + f + " r " + r + ": hit " + hit + " height " + AZ_VampCrypt.HallLocal(pos)[2] + " objects " + what);
		}
	}



	static float FloorYAt(float f, float r, float base)
	{
		vector pos, dir;
		int comp;
		if (!DayZPhysics.RaycastRV(AZ_VampCrypt.Hall(f, r, base + 1.0), AZ_VampCrypt.Hall(f, r, base - 1.0), pos, dir, comp, null, null, null, false, false, ObjIntersectView))
			return base;
		return Math.Clamp(AZ_VampCrypt.HallLocal(pos)[2], base - 0.2, base + 0.4);
	}

	static float FloorY(float f, float r)
	{
		vector pos, dir;
		int comp;
		if (!DayZPhysics.RaycastRV(AZ_VampCrypt.Hall(f, r, 1.0), AZ_VampCrypt.Hall(f, r, -1.0), pos, dir, comp, null, null, null, false, false, ObjIntersectView))
			return 0;
		float y = AZ_VampCrypt.HallLocal(pos)[2];
		return Math.Clamp(y, 0, 0.4);
	}


	static bool OnBox(vector pos, float f, float r, float hf, float hr)
	{
		vector l = AZ_VampCrypt.HallLocal(pos);
		return Math.AbsFloat(l[0] - f) <= hf && Math.AbsFloat(l[1] - r) <= hr && l[2] > -1.5 && l[2] < 2.0;
	}

	static bool OnPlate(vector pos)
	{
		return OnBox(pos, SPIKE_F, SPIKE_R, SPIKE_HF, SPIKE_HR);
	}

	static bool OnAlarm(vector pos)
	{
		return OnBox(pos, ALARM_F, ALARM_R, ALARM_HF, ALARM_HR);
	}

	static bool IsHuman(PlayerBase p)
	{
		return p && p.IsAlive() && !p.m_AZVampire && !p.m_AZVampTurning && !AZ_VampState.IsVampire(p);
	}

	static void Tick(int gen)
	{
		if (gen != s_Gen)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, gen);
		int now = GetGame().GetTime();
		if (s_SpikeState == UP && now >= s_SpikeAt)
			Sink(now);
		else if (s_SpikeState == DOWN && now >= s_SpikeAt)
			s_SpikeState = ARMED;
		if (!On())
			return;
		bool spikes = s_SpikeState == ARMED && now >= s_WedgedUntil;
		bool alarm = now >= s_AlarmReadyAt && s_Alarm != null;
		bool ball = s_BallState == B_READY && s_Ball != null;
		if (!spikes && !alarm && !ball)
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
		foreach (PlayerBase p : list)
		{
			if (now >= s_FakeReadyAt && CheckFake(p, now))
				s_FakeReadyAt = now + FAKE_CD_MS;
			if (alarm && CheckAlarm(p, now))
				alarm = false;
			if (spikes && CheckSpikes(p, now))
				spikes = false;
			if (ball && CheckBall(p, now))
				ball = false;
		}
	}



	static bool CheckFake(PlayerBase p, int now)
	{
		if (!s_Fake || !p || !OnBox(p.GetPosition(), FAKE_F, FAKE_R, SPIKE_HF, SPIKE_HR) || !IsHuman(p))
			return false;
		s_FakeClicks++;
		s_Fake.SetTrapState(CLICKED);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_Fake.SetTrapState, 1500, false, ARMED);
		Print("[AZ_Vamp] crypt trap fake plate clicked under " + AZ_VampState.Uid(p));
		return true;
	}


	static bool CheckAlarm(PlayerBase p, int now)
	{
		if (!p || !OnAlarm(p.GetPosition()) || !IsHuman(p))
			return false;
		s_AlarmReadyAt = now + ALARM_COOLDOWN_MS;
		s_AlarmRings++;
		if (s_Alarm)
			s_Alarm.Ring();
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		int told = 0;
		foreach (PlayerBase v : list)
		{
			if (v && (v.m_AZVampire || AZ_VampState.IsVampire(v)) && AZ_VampCrypt.Inside(v.GetPosition()))
			{
				v.Message("Something in the bunker stepped on the stone.", "colorImportant");
				told++;
			}
		}
		Print("[AZ_Vamp] crypt trap alarm fired on " + AZ_VampState.Uid(p) + ", " + told + " vampires inside told");
		return true;
	}


	static bool CheckSpikes(PlayerBase p, int now)
	{
		if (!p || !OnPlate(p.GetPosition()) || p.m_AZCryptGraceUntil != 0 || !IsHuman(p))
			return false;
		s_SpikeState = CLICKED;
		s_SpikeClickAt = now;
		if (s_Spikes)
			s_Spikes.SetTrapState(CLICKED);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Spring, SPRING_MS, false, s_Gen);
		Print("[AZ_Vamp] crypt trap spikes click under " + AZ_VampState.Uid(p));
		return true;
	}

	static void Spring(int gen)
	{
		if (gen != s_Gen || s_SpikeState != CLICKED)
			return;
		int now = GetGame().GetTime();
		s_SpikeState = UP;
		s_SpikeAt = now + UP_MS;
		if (s_Spikes)
			s_Spikes.SetTrapState(UP);
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
		int hit = 0;
		foreach (PlayerBase p : list)
		{
			if (!IsHuman(p) || p.m_AZCryptGraceUntil != 0 || !OnPlate(p.GetPosition()))
				continue;
			Wound(p);
			hit++;
		}
		if (hit == 0)
			Print("[AZ_Vamp] crypt trap spikes came up on nobody");
	}

	static void Wound(PlayerBase p)
	{
		AZ_VampEvidence.Begin("crypt_spikes", null, p, false);
		float h = p.GetHealth("", "Health");
		float dmg = Math.Min(SPIKE_DMG, h - SPIKE_FLOOR);
		if (dmg > 0)
			p.AddHealth("", "Health", -dmg);
		string foot = "RightFoot";
		if (Math.RandomFloat01() < 0.5)
			foot = "LeftFoot";
		bool bled = false;
		if (p.GetBleedingManagerServer())
			bled = p.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection(foot);
		bool broke = Math.RandomFloat01() < LEG_CHANCE;
		if (broke)
		{
			string leg = "RightLeg";
			if (foot == "LeftFoot")
				leg = "LeftLeg";
			p.SetHealth(leg, "Health", 0);
			if (p.GetModifiersManager())
			{
				if (p.GetModifiersManager().IsModifierActive(eModifiers.MDF_BROKEN_LEGS))
					p.GetModifiersManager().DeactivateModifier(eModifiers.MDF_BROKEN_LEGS);
				p.GetModifiersManager().ActivateModifier(eModifiers.MDF_BROKEN_LEGS);
			}
		}
		p.m_AZLastHitAt = GetGame().GetTime();
		AZ_VampEvidence.End("crypt_spikes", null, p);
		if (p.GetIdentity())
			p.Message("Iron spikes tear up through the floor.", "colorImportant");
		bool mdf = p.GetModifiersManager() && p.GetModifiersManager().IsModifierActive(eModifiers.MDF_BROKEN_LEGS);
		Print("[AZ_Vamp] crypt trap spikes fired on " + AZ_VampState.Uid(p) + ": health " + h + " -> " + p.GetHealth("", "Health") + ", bleed " + foot + " " + bled + ", leg roll " + broke + " (legs R " + p.GetHealth("RightLeg", "Health") + " L " + p.GetHealth("LeftLeg", "Health") + ", fracture modifier " + mdf + ")");
		AZ_VampCryptLurkers.OnTrapFail(p, "spikes");
	}

	static void Sink(int now)
	{
		s_SpikeState = DOWN;
		s_SpikeAt = s_SpikeClickAt + REARM_MS;
		if (s_SpikeAt < now)
			s_SpikeAt = now;
		if (s_Spikes)
			s_Spikes.SetTrapState(ARMED);
	}


	static void ForceArm()
	{
		s_Gen++;
		s_SpikeState = ARMED;
		s_SpikeAt = 0;
		s_WedgedUntil = 0;
		s_AlarmReadyAt = 0;
		s_RollGen++;
		if (s_Built && !s_RoomSealed && (!s_Ball || s_BallState != B_READY))
			SpawnBall();
		if (s_Spikes)
			s_Spikes.SetTrapState(ARMED);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, s_Gen);
	}


	static float Crown(float f)
	{
		return CW_H0 + (f - CW_F) * CW_SLOPE;
	}

	static void SpawnBall()
	{
		if (s_Ball)
			GetGame().ObjectDelete(s_Ball);
		Object o = AZ_VampCrypt.PutCrypt("AZ_Crypt_Trap_Boulder", AZ_VampCrypt.Hall(BALL_F0, CW_R, BALL_Y0), AZ_VampCrypt.HALL_YAW);
		s_Ball = AZ_Crypt_Trap_Boulder.Cast(o);
		s_BallState = B_READY;
	}

	static bool CheckBall(PlayerBase p, int now)
	{
		if (!p || !OnBox(p.GetPosition(), TRIG_F, CW_R, TRIG_HF, TRIG_HR) || !IsHuman(p))
			return false;
		vector l = AZ_VampCrypt.HallLocal(p.GetPosition());
		if (l[2] < Crown(TRIG_F) - 0.4)
			return false;
		Release(AZ_VampState.Uid(p));
		return true;
	}


	static void Release(string who)
	{
		if (!s_Ball || s_BallState != B_READY)
			return;
		s_BallState = B_RUMBLE;
		s_Ball.SetBallState(B_RUMBLE);
		s_RollGen++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Roll, RUMBLE_MS, false, s_RollGen);
		Print("[AZ_Vamp] crypt trap boulder released by " + who);
	}

	static void Roll(int gen)
	{
		if (gen != s_RollGen || !s_Ball)
			return;
		s_BallState = B_ROLL;
		s_RollAt = GetGame().GetTime();
		s_RollF = BALL_F0;
		s_RollHit = new array<PlayerBase>;
		s_Ball.SetBallState(B_ROLL);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RollTick, ROLL_TICK_MS, false, gen);
	}


	static float BallF(int t)
	{
		float tt = t;
		float k = Math.Clamp(tt / ROLL_MS, 0, 1);
		return BALL_F0 + (BALL_F1 - BALL_F0) * k;
	}

	static float BallY(float f)
	{
		return BALL_Y0 + (BALL_Y1 - BALL_Y0) * (f - BALL_F0) / (BALL_F1 - BALL_F0);
	}

	static void RollTick(int gen)
	{
		if (gen != s_RollGen || s_BallState != B_ROLL)
			return;
		int t = GetGame().GetTime() - s_RollAt;
		float f = BallF(t);
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
		foreach (PlayerBase p : list)
		{


			if (!p || !p.IsAlive() || p.m_AZCryptGraceUntil != 0 || s_RollHit.Find(p) >= 0)
				continue;
			vector l = AZ_VampCrypt.HallLocal(p.GetPosition());

			float nf = Math.Clamp(l[0], f, s_RollF);
			float dr = l[1] - CW_R;
			float df = l[0] - nf;
			if (dr * dr + df * df > BALL_HIT * BALL_HIT || Math.AbsFloat(l[2] + 0.9 - BallY(nf)) > 1.2)
				continue;
			s_RollHit.Insert(p);
			Crush(p, nf);
		}
		s_RollF = f;
		if (t >= ROLL_MS)
		{
			s_BallState = B_GONE;
			s_Ball.SetBallState(B_GONE);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BallGone, 1500, false, gen);
			Print("[AZ_Vamp] crypt trap boulder ran out at " + AZ_VampCrypt.Hall(f, CW_R, BallY(f)) + ", " + s_RollHit.Count() + " hit");
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RollTick, ROLL_TICK_MS, false, gen);
	}

	static void BallGone(int gen)
	{
		if (gen != s_RollGen)
			return;
		if (s_Ball)
			GetGame().ObjectDelete(s_Ball);
		s_Ball = null;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BallBack, BALL_BACK_MS, false, gen);
	}

	static void BallBack(int gen)
	{
		if (gen != s_RollGen)
			return;
		SpawnBall();
	}

	static void Crush(PlayerBase p, float atF)
	{
		AZ_VampEvidence.Begin("crypt_boulder", null, p, false);
		float h = p.GetHealth("", "Health");
		float dmg = Math.Min(BALL_DMG, h - BALL_FLOOR);
		if (dmg > 0)
			p.AddHealth("", "Health", -dmg);
		float shock = p.GetHealth("", "Shock");
		p.AddHealth("", "Shock", -shock);
		AZ_VampEvidence.End("crypt_boulder", null, p);
		p.m_AZLastHitAt = GetGame().GetTime();
		if (p.GetIdentity())
			p.Message("The stone ball runs you down.", "colorImportant");
		Print("[AZ_Vamp] crypt trap boulder hit " + AZ_VampState.Uid(p) + " at f " + atF + ": health " + h + " -> " + p.GetHealth("", "Health") + ", shock " + shock + " -> " + p.GetHealth("", "Shock"));
		if (IsHuman(p))
			AZ_VampCryptLurkers.OnTrapFail(p, "boulder");
	}


	static bool CanWedge(PlayerBase p)
	{
		if (!p || !IsHuman(p) || !AZ_VampCrypt.Site())
			return false;
		ToolBase tool = ToolBase.Cast(p.GetItemInHands());
		if (!tool)
			return false;
		string type = tool.GetType();
		if (type.IndexOf("Knife") < 0 && !tool.IsKindOf("Screwdriver"))
			return false;
		vector l = AZ_VampCrypt.HallLocal(p.GetPosition());
		return Math.AbsFloat(l[0] - SPIKE_F) <= SPIKE_HF + 0.8 && Math.AbsFloat(l[1] - SPIKE_R) <= SPIKE_HR + 0.5 && l[2] > -1.5 && l[2] < 2.0;
	}

	static void Wedge(PlayerBase p)
	{
		if (!CanWedge(p))
			return;
		s_WedgedUntil = GetGame().GetTime() + WEDGE_MS;
		p.Message("You jam the plate. It will hold a while.", "colorAction");
		Print("[AZ_Vamp] crypt trap spikes wedged by " + AZ_VampState.Uid(p));
	}



	static vector SealedSpot(int i)
	{
		float f = SPIKE_F + 2.5 + 0.9 * i;
		return AZ_VampCrypt.Hall(f, SPIKE_R, FloorYAt(f, SPIKE_R, SPIKE_Y) + 0.05);
	}

	static PlayerBase Dummy(float f)
	{
		vector at = AZ_VampCrypt.Hall(f, 2.5, 0.05);
		if (s_RoomSealed)
		{
			int k = 0;
			if (s_Probe)
				k = s_Probe.Count();
			at = SealedSpot(k);
		}
		int seed = 0;
		if (s_Probe)
			seed = s_Probe.Count();
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


	static string HitAt(vector from, vector to)
	{
		vector pos, dir;
		int comp;
		set<Object> objs = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, to, pos, dir, comp, objs, null, null, true, false, ObjIntersectGeom))
			return "none";
		string what = "?";
		if (objs.Count() > 0 && objs[0])
			what = objs[0].GetType();
		vector l = AZ_VampCrypt.HallLocal(pos);
		return string.Format("f %1 r %2 y %3 %4", Math.Round(l[0] * 100) / 100, Math.Round(l[1] * 100) / 100, Math.Round(l[2] * 100) / 100, what);
	}

	static void Survey()
	{
		float f;
		float r;
		string line;

		for (f = -18.5; f <= -4.9; f += 1.5)
		{
			line = "[AZ_Vamp] survey f " + f + ": north@1.0 " + HitAt(AZ_VampCrypt.Hall(f, 0, 1.0), AZ_VampCrypt.Hall(f, -9, 1.0));
			line += " | south@1.0 " + HitAt(AZ_VampCrypt.Hall(f, 0, 1.0), AZ_VampCrypt.Hall(f, 9, 1.0));
			line += " | north@0.3 " + HitAt(AZ_VampCrypt.Hall(f, 0, 0.3), AZ_VampCrypt.Hall(f, -9, 0.3));
			line += " | up " + HitAt(AZ_VampCrypt.Hall(f, -2, 0.3), AZ_VampCrypt.Hall(f, -2, 9));
			Print(line);
		}

		for (r = -6.0; r <= 6.01; r += 0.5)
		{
			line = "[AZ_Vamp] survey r " + r + ": fwd@1.0 from -14 " + HitAt(AZ_VampCrypt.Hall(-14, r, 1.0), AZ_VampCrypt.Hall(-3, r, 1.0));
			line += " | fwd@0.3 from -12 " + HitAt(AZ_VampCrypt.Hall(-12, r, 0.3), AZ_VampCrypt.Hall(-3, r, 0.3));
			line += " | back@0.3 from -5.2 " + HitAt(AZ_VampCrypt.Hall(-5.2, r, 0.3), AZ_VampCrypt.Hall(-13, r, 0.3));
			line += " | floor " + HitAt(AZ_VampCrypt.Hall(-8, r, 1.0), AZ_VampCrypt.Hall(-8, r, -2));
			Print(line);
		}
		Print("[AZ_Vamp] survey done");
	}

	static void Probe()
	{
		if (s_RoomSealed)
			Print("[AZ_Vamp] trap probe: entrance off -> the alarm and boulder checks SKIP (entrance off); the hall's spike plate and the lurkers are probed");
		else
			Survey();
		if (!s_Built)
		{
			Print("[AZ_Vamp] trap probe: FAIL traps not built (crypt built " + AZ_VampCrypt.s_Built + ")");
			return;
		}
		ForceArm();
		AZ_VampCryptLurkers.Reset();
		AZ_VampCryptLurkers.s_ProbeElder = true;
		PlayerBase vamp = Dummy(-9.0);
		PlayerBase human = Dummy(-9.5);
		PlayerBase off = Dummy(-10.0);
		PlayerBase grace = Dummy(-10.5);
		if (!vamp || !human || !off || !grace)
		{
			Print("[AZ_Vamp] trap probe: FAIL spawn");
			return;
		}
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		Print("[AZ_Vamp] trap probe: start, vampire " + AZ_VampState.IsVampire(vamp) + " synced " + vamp.m_AZVampire + ", spikes static " + (s_Spikes != null));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 3000, false, 1, vamp, human, off, grace, 0.0);
	}

	static void ProbeStand(PlayerBase d)
	{
		ProbeAt(d, SPIKE_F, SPIKE_R, SPIKE_Y);
	}

	static void ProbeAt(PlayerBase d, float f, float r, float y = 0)
	{
		vector at = AZ_VampCrypt.Hall(f, r, y + 0.05);
		d.SetPosition(at);
		Print("[AZ_Vamp] trap probe: placed " + AZ_VampState.Uid(d) + " at hall " + AZ_VampCrypt.HallLocal(d.GetPosition()) + " on spikes " + OnPlate(d.GetPosition()) + " on alarm " + OnAlarm(d.GetPosition()));
	}

	static float s_PH1;
	static float s_PH2;
	static float s_PH3;

	static void ProbeOnRun(PlayerBase d, float f)
	{
		d.SetPosition(AZ_VampCrypt.Hall(f, CW_R, Crown(f) + 0.05));
		Print("[AZ_Vamp] trap probe: placed " + AZ_VampState.Uid(d) + " on the run at hall " + AZ_VampCrypt.HallLocal(d.GetPosition()));
	}

	static void ProbeAway(PlayerBase d)
	{
		if (!d)
			return;
		if (s_RoomSealed)
		{
			int k = 0;
			if (s_Probe)
				k = Math.Max(0, s_Probe.Find(d));
			d.SetPosition(SealedSpot(k));
			return;
		}
		d.SetPosition(AZ_VampCrypt.Hall(-10, 2.5, 0.05));
	}

	static string Verdict(bool ok)
	{
		if (ok)
			return "PASS";
		return "FAIL";
	}



	static void ProbeStep(int step, PlayerBase vamp, PlayerBase human, PlayerBase off, PlayerBase grace, float before)
	{
		int next = step + 1;
		float hb = before;
		bool ok;
		switch (step)
		{
			case 1:
				if (s_RoomSealed)
				{
					Print("[AZ_Vamp] trap probe: alarm plate SKIP (entrance off)");
					hb = vamp.GetHealth("", "Health");
					ProbeStand(vamp);
					next = 2;
					break;
				}
				hb = s_AlarmRings;
				ProbeAt(vamp, ALARM_F, ALARM_R);
				next = 11;
				break;
			case 11:
				ok = s_AlarmRings == before;
				Print("[AZ_Vamp] trap probe: vampire on the alarm plate -> rings " + before + " -> " + s_AlarmRings + " " + Verdict(ok) + " (expect no bell)");
				ProbeAway(vamp);
				hb = s_AlarmRings;
				ProbeAt(human, ALARM_F, ALARM_R);
				next = 12;
				break;
			case 12:
				ok = s_AlarmRings == before + 1;
				Print("[AZ_Vamp] trap probe: human on the alarm plate -> rings " + before + " -> " + s_AlarmRings + " " + Verdict(ok) + " (expect one bell)");
				ProbeAway(human);
				hb = vamp.GetHealth("", "Health");
				ProbeStand(vamp);
				next = 2;
				break;
			case 2:
				ok = s_SpikeState == ARMED && vamp.GetHealth("", "Health") >= before;
				Print("[AZ_Vamp] trap probe: vampire on the plate -> state " + s_SpikeState + " health " + before + " -> " + vamp.GetHealth("", "Health") + " " + Verdict(ok) + " (expect no click, no damage)");
				ProbeAway(vamp);
				hb = human.GetHealth("", "Health");
				ProbeStand(human);
				break;
			case 3:
				ok = s_SpikeState == UP && human.GetHealth("", "Health") < before;
				int bleeds = 0;
				if (human.GetBleedingManagerServer())
					bleeds = human.GetBleedingManagerServer().GetBleedingSourcesCount();
				int legs = human.GetBrokenLegs();
				Print("[AZ_Vamp] trap probe: human on the plate -> state " + s_SpikeState + " health " + before + " -> " + human.GetHealth("", "Health") + " bleeding sources " + bleeds + " legs " + legs + " " + Verdict(ok) + " (expect spikes up, damage)");
				ProbeAway(human);
				AZ_VampState.Cfg().cryptTraps = false;
				ForceArm();
				hb = off.GetHealth("", "Health");
				ProbeStand(off);
				break;
			case 4:
				ok = s_SpikeState == ARMED && off.GetHealth("", "Health") >= before;
				Print("[AZ_Vamp] trap probe: kill switch (cryptTraps false), human on the plate -> state " + s_SpikeState + " health " + before + " -> " + off.GetHealth("", "Health") + " " + Verdict(ok) + " (expect nothing)");
				ProbeAway(off);
				AZ_VampState.Cfg().cryptTraps = true;
				grace.m_AZCryptGraceUntil = GetGame().GetTime() + 10000;
				hb = grace.GetHealth("", "Health");
				ProbeStand(grace);
				break;
			case 5:
				ok = s_SpikeState == ARMED && grace.GetHealth("", "Health") >= before;
				Print("[AZ_Vamp] trap probe: human in arrival grace on the plate -> state " + s_SpikeState + " health " + before + " -> " + grace.GetHealth("", "Health") + " " + Verdict(ok) + " (expect the plate to wait)");
				grace.m_AZCryptGraceUntil = 0;
				ForceArm();
				if (s_RoomSealed)
				{
					Print("[AZ_Vamp] trap probe: boulder SKIP (entrance off)");
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 8000, false, 6, vamp, human, off, grace, 0.0);
					return;
				}


				s_PH1 = off.GetHealth("", "Health");
				s_PH2 = vamp.GetHealth("", "Health");
				s_PH3 = grace.GetHealth("", "Health");
				ProbeOnRun(vamp, -11.6);
				ProbeAt(grace, -9.5, -4.9);
				ProbeOnRun(off, TRIG_F);
				Print("[AZ_Vamp] trap probe: boulder set, ball " + (s_Ball != null) + " state " + s_BallState);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 8000, false, 6, vamp, human, off, grace, 0.0);
				return;
			case 6:
				if (!s_RoomSealed)
				{
					ok = off.GetHealth("", "Health") < s_PH1 && off.GetHealth("", "Shock") < 25;
					Print("[AZ_Vamp] trap probe: boulder, human on the trigger and the run -> health " + s_PH1 + " -> " + off.GetHealth("", "Health") + " shock " + off.GetHealth("", "Shock") + " " + Verdict(ok) + " (expect hit, knocked down: shock near 0)");
					ok = vamp.GetHealth("", "Health") < s_PH2;
					Print("[AZ_Vamp] trap probe: boulder, vampire on the run -> health " + s_PH2 + " -> " + vamp.GetHealth("", "Health") + " " + Verdict(ok) + " (expect hit too: LD_CAMPING_ROUTE 3, no trap-immune perch)");
					ok = grace.GetHealth("", "Health") >= s_PH3;
					Print("[AZ_Vamp] trap probe: boulder, human in the nook -> health " + s_PH3 + " -> " + grace.GetHealth("", "Health") + " at hall " + AZ_VampCrypt.HallLocal(grace.GetPosition()) + " " + Verdict(ok) + " (expect untouched)");
				}
				int sleepers = 0;
				foreach (PlayerBase sl : AZ_VampCryptLurkers.s_Sleepers)
				{
					if (sl)
						sleepers++;
				}

				ok = human.IsAlive() && human.GetHealth("", "Shock") > 25 && !human.m_AZVampTurning && !human.m_AZVampHeld && human.GetHealth("", "Blood") < 4800 && AZ_VampCryptLurkers.Hunted(human);
				Print("[AZ_Vamp] trap probe: lurker on the spiked human (first time) -> alive " + human.IsAlive() + " shock " + human.GetHealth("", "Shock") + " turning " + human.m_AZVampTurning + " held " + human.m_AZVampHeld + " blood " + human.GetHealth("", "Blood") + " hunted " + AZ_VampCryptLurkers.Hunted(human) + " sleepers left " + sleepers + " " + Verdict(ok) + " (expect a taste of ~400 ml, let go standing, NOT sick)");
				ok = !off.m_AZVampHeld && !off.m_AZVampTurning;
				Print("[AZ_Vamp] trap probe: lurker cooldown, boulder victim -> held " + off.m_AZVampHeld + " turning " + off.m_AZVampTurning + " " + Verdict(ok) + " (expect no second ambush inside the cooldown)");

				AZ_VampCryptLurkers.Reset();
				s_PH1 = human.GetHealth("", "Blood");
				AZ_VampCryptLurkers.OnTrapFail(human, "probe hunt");
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 9000, false, 61, vamp, human, off, grace, 0.0);
				return;
			case 61:
				ok = human.IsAlive() && human.GetHealth("", "Shock") < 25 && !human.m_AZVampTurning && !human.m_AZVampHeld && human.GetHealth("", "Blood") < s_PH1;
				Print("[AZ_Vamp] trap probe: lurker on the same human again (the hunt) -> shock " + human.GetHealth("", "Shock") + " turning " + human.m_AZVampTurning + " held " + human.m_AZVampHeld + " blood " + s_PH1 + " -> " + human.GetHealth("", "Blood") + " " + Verdict(ok) + " (expect the full feed: drained and down, NOT sick, released)");
				ForceArm();
				AZ_VampState.Cfg().cryptTraps = false;
				if (!s_RoomSealed)
					ProbeOnRun(human, TRIG_F);
				next = 7;
				break;
			case 7:
				if (!s_RoomSealed)
				{
					ok = s_BallState == B_READY;
					Print("[AZ_Vamp] trap probe: kill switch, human on the boulder trigger -> ball state " + s_BallState + " " + Verdict(ok) + " (expect it stays in the cradle)");
				}
				AZ_VampState.Cfg().cryptTraps = true;
				AZ_VampCryptLurkers.Reset();
				AZ_VampState.Cfg().cryptLurkers = false;
				AZ_VampCryptLurkers.OnTrapFail(grace, "probe");
				ok = !AZ_VampCryptLurkers.s_Busy;
				Print("[AZ_Vamp] trap probe: lurker kill switch (cryptLurkers false), a human fails a trap -> ambush " + AZ_VampCryptLurkers.s_Busy + " " + Verdict(ok) + " (expect none)");
				AZ_VampState.Cfg().cryptLurkers = true;
				AZ_VampCryptLurkers.s_ProbeElder = false;
				AZ_VampCryptLurkers.Reset();
				foreach (PlayerBase d : s_Probe)
				{
					if (d)
						GetGame().ObjectDelete(d);
				}
				s_Probe = null;
				ForceArm();
				Print("[AZ_Vamp] trap probe: done");
				return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 1500, false, next, vamp, human, off, grace, hb);
	}
}



class AZ_Crypt_Trap_Spikes extends House
{
	protected int m_TrapState;
	protected int m_TrapStateLocal;

	void AZ_Crypt_Trap_Spikes()
	{
		RegisterNetSyncVariableInt("m_TrapState", 0, 3);
	}

	override void EEInit()
	{
		super.EEInit();
		SetAnimationPhase("spikes", Phase());
		m_TrapStateLocal = m_TrapState;
	}

	float Phase()
	{
		if (m_TrapState == AZ_VampCryptTraps.UP)
			return 1;
		return 0;
	}

	void SetTrapState(int state)
	{
		if (!GetGame().IsServer() || state == m_TrapState)
			return;
		m_TrapState = state;
		SetAnimationPhase("spikes", Phase());
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_TrapState == m_TrapStateLocal)
			return;
		int was = m_TrapStateLocal;
		m_TrapStateLocal = m_TrapState;
		SetAnimationPhase("spikes", Phase());
		if (!GetGame().IsClient())
			return;
		vector at = GetPosition() + "0 0.05 0";
		if (m_TrapState == AZ_VampCryptTraps.CLICKED)
		{
			SEffectManager.PlaySound("landmineActivate_SoundSet", at);
			Particle.PlayInWorld(ParticleList.DUST_GROUND, at);
		}
		else if (m_TrapState == AZ_VampCryptTraps.UP)
			SEffectManager.PlaySound("beartrapClose_SoundSet", at);
		else if (was == AZ_VampCryptTraps.UP)
			SEffectManager.PlaySound("beartrapOpen_SoundSet", at);
	}
}


class ActionAZWedgePlateCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(8);
	}
}

class ActionAZWedgePlate : ActionContinuousBase
{
	void ActionAZWedgePlate()
	{
		m_CallbackClass = ActionAZWedgePlateCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Wedge the plate";
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
		return AZ_VampCryptTraps.CanWedge(player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptTraps.Wedge(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZWedgePlate);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZWedgePlate, InputActionMap);
	}
}



class AZ_Crypt_Trap_Alarm extends House
{
	protected int m_Rings;
	protected int m_RingsLocal = -1;

	void AZ_Crypt_Trap_Alarm()
	{
		RegisterNetSyncVariableInt("m_Rings", 0, 255);
	}

	void Ring()
	{
		if (!GetGame().IsServer())
			return;
		m_Rings = (m_Rings + 1) % 256;
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		int was = m_RingsLocal;
		m_RingsLocal = m_Rings;
		if (was < 0 || was == m_Rings || !GetGame().IsClient())
			return;
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (!me || !AZ_VampCrypt.Inside(me.GetPosition()))
			return;
		SEffectManager.PlaySound("AZ_CryptAlarmBell_SoundSet", AZ_VampCrypt.Hall(10, 0, 1));
	}
}




class AZ_Crypt_Trap_Boulder extends House
{
	protected int m_BState;
	protected int m_BStateLocal;

	void AZ_Crypt_Trap_Boulder()
	{
		RegisterNetSyncVariableInt("m_BState", 0, 3);
	}

	override void EEInit()
	{
		super.EEInit();
		SetAnimationPhase("roll", Phase());
		m_BStateLocal = m_BState;
	}

	float Phase()
	{
		if (m_BState >= AZ_VampCryptTraps.B_ROLL)
			return 1;
		return 0;
	}

	void SetBallState(int state)
	{
		if (!GetGame().IsServer() || state == m_BState)
			return;
		m_BState = state;
		SetAnimationPhase("roll", Phase());
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_BState == m_BStateLocal)
			return;
		m_BStateLocal = m_BState;
		SetAnimationPhase("roll", Phase());
		if (!GetGame().IsClient())
			return;
		if (m_BState == AZ_VampCryptTraps.B_RUMBLE)
		{
			SEffectManager.PlaySound("AZ_CryptSlab_SoundSet", GetPosition());
			Particle.PlayInWorld(ParticleList.DUST_GROUND, GetPosition() - "0 0.4 0");
		}
		else if (m_BState == AZ_VampCryptTraps.B_ROLL)
			SEffectManager.PlaySound("AZ_CryptSlab_SoundSet", GetPosition());
		else if (m_BState == AZ_VampCryptTraps.B_GONE)
		{
			vector end = AZ_VampCrypt.Hall(AZ_VampCryptTraps.BALL_F1, AZ_VampCryptTraps.CW_R, AZ_VampCryptTraps.BALL_Y1);
			SEffectManager.PlaySound("AZ_CryptBones_SoundSet", end);
			Particle.PlayInWorld(ParticleList.DUST_CLOUD, end);
		}
	}
}
