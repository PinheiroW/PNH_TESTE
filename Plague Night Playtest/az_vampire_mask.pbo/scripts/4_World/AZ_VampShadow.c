






class AZ_VampShadowState
{
	int biteSerial;
	PlayerBase victim;
	int chain;
	int chainUntil;
	int coolUntil;
	int busyUntil;
	int compelUntil;
	int strikeUntil;
	int pathWhyAt;
	bool compelling;
	bool animWaiting;
	ref array<PlayerBase> bitten = new array<PlayerBase>;
}

class AZ_VampShadow
{
	static const int RPC_SHADOW = 4702188;
	// Owner-only bite transition freeze (AZ-2510); RPC_SHADOW stays Param1<vector> for all its other senders.
	static const int RPC_SHADOW_FREEZE = 4702623;
	static const int RPC_STREAK = 4702249;
	static const int STREAK_PUFFS = 6;
	static const float TASTE_BITE_SEC = 1.5;
	static const int STREAK_MS = 350;
	static const int RPC_JBAR = 4702194;
	static bool s_InChain;
	static const int SJ_SHADOW_BITE = 147;
	static const int AI_CHAIN_WAIT_MS = 3000;
	static const int AI_CHAIN_POLL_MS = 50;
	static ref map<PlayerBase, ref AZ_VampShadowState> s_State = new map<PlayerBase, ref AZ_VampShadowState>;

	static AZ_VampShadowState State(PlayerBase p)
	{
		AZ_VampShadowState st = s_State.Get(p);
		if (!st)
		{
			st = new AZ_VampShadowState();
			s_State.Insert(p, st);
		}
		return st;
	}


	static void Abort(PlayerBase vamp)
	{
		AZ_VampShadowState st = s_State.Get(vamp);
		if (!st)
			return;
		PlayerBase held = st.victim;
		st.victim = null;
		st.biteSerial++;
		st.chain = 0;
		st.chainUntil = 0;
		st.busyUntil = 0;
		st.compelUntil = 0;
		st.compelling = false;
		st.animWaiting = false;
		st.bitten.Clear();
		st.coolUntil = GetGame().GetTime() + AZ_VampState.Cfg().shadowCooldownSec * 1000;
		vamp.m_AZShadowPinned = false;
		if (held && held.m_AZVampHeld && held.m_AZVampFeeder == vamp)
			AZ_VampFeed.Release(vamp, held);
		BiteAnim(vamp, 2, 0, 0);
	}

	// AZ-2627 / AZ-2510: hold the vampire where it landed for the bite. The owner's own movement prediction and the
	// 200 ms before the clip could walk it off the victim (the 1-2 m gap seen on streams); the server re-snaps it, as
	// AZ_VampPinTick does for the victim, and the tick lets go the moment the hold ends
	// ms: how long at most; serial: the chain step it belongs to (a taste has no hold, so the step itself keeps it, J02)
	static void Pin(PlayerBase vamp, vector land, vector dir, int ms, int serial, bool levelHeight = false)
	{
		int now = GetGame().GetTime();
		vamp.m_AZShadowPinPos = land;
		vamp.m_AZShadowPinLevel = levelHeight;
		vamp.m_AZShadowPinDir = dir;
		vamp.m_AZShadowPinned = true;
		vamp.m_AZShadowPinFixes = 0;
		vamp.m_AZShadowPinAt = now + 500;		// let the owner's first snap land
		vamp.m_AZShadowPinUntil = now + ms;
		vamp.m_AZShadowPinSerial = serial;
	}

	// J03: the record goes with the entity (final logout, deletion); the cooldowns live on by UID (AZ_VampState.Checkpoint)
	static void Forget(PlayerBase p)
	{
		if (!p || !s_State.Contains(p))
			return;
		Abort(p);
		s_State.Remove(p);
	}

	// J01: back from a relog with the cooldowns it left with; the owner's J bar shows the longer one
	static void RestoreCooldowns(PlayerBase p, int strikeMs, int frenzyMs)
	{
		if (!p || (strikeMs <= 0 && frenzyMs <= 0))
			return;
		AZ_VampShadowState st = State(p);
		int now = GetGame().GetTime();
		if (strikeMs > 0)
			st.strikeUntil = Math.Max(st.strikeUntil, now + strikeMs);
		if (frenzyMs > 0)
			st.coolUntil = Math.Max(st.coolUntil, now + frenzyMs);
		if (p.GetIdentity())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SendBar, 3000, false, p, Math.Max(strikeMs, frenzyMs) - 3000);
	}

	static void SendBar(PlayerBase p, int ms)
	{
		if (p && p.GetIdentity() && ms > 0)
			GetGame().RPCSingleParam(p, RPC_JBAR, new Param1<int>(ms), true, p.GetIdentity());
	}

	static void Released(PlayerBase vamp, PlayerBase victim)
	{
		AZ_VampShadowState st = s_State.Get(vamp);
		if (st && st.victim == victim)
			Abort(vamp);
	}


	static bool TryStep(PlayerBase vamp)
	{
		if (AZ_VampDown.RefuseBite(vamp))
			return false;
		if (vamp && GetGame().GetTime() < vamp.m_AZFeedCancelUntil)
			return false;

		if (!vamp || vamp.m_AZVampHeld || !vamp.IsAlive() || vamp.IsUnconscious() || vamp.IsInVehicle())
			return false;
		AZ_VampConfig cfg = AZ_VampState.Cfg();


		if (!vamp.m_AZBloodMoon && cfg.shadowStrike && AZ_VampEvent.Active())
			return Strike(vamp);
		if (!vamp.m_AZBloodMoon || !AZ_VampEvent.Active())
		{


			int t = GetGame().GetTime();
			if (AZ_VampEvent.Active() && t >= vamp.m_AZShadowWhyAt && FindTarget(vamp, cfg.shadowRange, 0.64, AZ_VampReach.START_RANGE, new array<PlayerBase>))
			{
				vamp.m_AZShadowWhyAt = t + 30000;
				vamp.Message("The moon is too pale for the shadows.", "colorAction");
			}
			return false;
		}
		AZ_VampShadowState st = State(vamp);
		int now = GetGame().GetTime();
		if (st.victim || now < st.busyUntil)
			return true;
		if (vamp.GetActionManager() && vamp.GetActionManager().GetRunningAction())
			return true;
		if (vamp.GetItemInHands() && !st.compelling && now >= st.compelUntil)
		{

			if (now >= vamp.m_AZShadowWhyAt && FindTarget(vamp, cfg.shadowRange, 0.64, AZ_VampReach.START_RANGE, new array<PlayerBase>))
			{
				vamp.m_AZShadowWhyAt = now + 10000;
				vamp.Message("Free your hands to feed.", "colorAction");
			}
			return false;
		}
		bool chaining = st.chain > 0 && now < st.chainUntil;
		if (chaining && st.compelling)
			return CompelStep(vamp);
		if (!chaining)
		{
			st.chain = 0;
			st.bitten.Clear();
			st.compelling = false;
			if (now < st.compelUntil && cfg.compelSec > 0)
			{


				st.compelling = true;
				st.compelUntil = 0;
				return CompelStep(vamp);
			}
			else if (now < st.coolUntil)
			{

				if (now >= vamp.m_AZShadowWhyAt && FindTarget(vamp, cfg.shadowRange, 0.64, AZ_VampReach.START_RANGE, new array<PlayerBase>))
				{
					vamp.m_AZShadowWhyAt = now + 10000;
					vamp.Message("Your strength is still returning (" + Math.Ceil((st.coolUntil - now) / 1000.0) + " s).", "colorAction");
				}
				return false;
			}
		}
		float cone = 0.64;
		if (chaining)
			cone = -1;
		float minD = AZ_VampReach.START_RANGE;
		PlayerBase victim = FindTarget(vamp, cfg.shadowRange, cone, minD, st.bitten, false);
		if (!victim)
			return false;
		vector from = vamp.GetPosition();
		vector to = victim.GetPosition();
		vector dir = to - from;
		dir[1] = 0;
		dir.Normalize();
		vector land = to - dir * AZ_VampFeed.ROOT_GAP;
		bool rear = AZ_VampRear.Available();
		if (rear)
		{

			dir = victim.GetDirection();
			dir[1] = 0;
			dir.Normalize();
			land = to - dir * AZ_VampFeed.ROOT_GAP;
		}
		land[1] = GetGame().SurfaceY(land[0], land[2]);
		if (land[1] == GetGame().SurfaceY(to[0], to[2]) && (to[1] > land[1] + 0.3 || to[1] < land[1] - 2))
			land[1] = to[1];
		vector rearLand = land;
		bool rearLevel = AZ_VampReach.LevelBite(victim, rearLand);
		if (rear && (!rearLevel || Blocked(victim, rearLand)))
		{

			rear = false;
			dir = to - from;
			dir[1] = 0;
			dir.Normalize();
			land = to - dir * AZ_VampFeed.ROOT_GAP;
			land[1] = GetGame().SurfaceY(land[0], land[2]);
			if (land[1] == GetGame().SurfaceY(to[0], to[2]) && (to[1] > land[1] + 0.3 || to[1] < land[1] - 2))
				land[1] = to[1];
		}
		// Validate the final computed landing before any move/FX.
		if (!AZ_VampReach.LevelBite(victim, land) || !JumpPath(vamp, victim, land))
		{
			NoPath(vamp);
			return true;
		}
		GetGame().RPCSingleParam(vamp, RPC_SHADOW, new Param1<vector>(from), true);
		GetGame().RPCSingleParam(vamp, RPC_SHADOW_FREEZE, new Param1<bool>(true), true);


		if (st.chain == 0)
			GetGame().RPCSingleParam(vamp, AZ_VampSound.RPC_KISS, new Param1<bool>(true), true);
		// Server placement plus the owner transform, so the bite lines up on every client.
		AZ_VampSnap.To(vamp, land, dir);
		st.chain++;
		st.victim = victim;
		st.animWaiting = false;
		st.biteSerial++;
		st.bitten.Insert(victim);


		bool taste = victim.GetHealth("", "Blood") <= cfg.bloodFloor + 50;
		float biteSec = Math.Max(cfg.chainMinBiteSec, cfg.chainBiteSec);		// AZ-2627: the chain's own floor, not the strike's 6 s
		if (taste)
			biteSec = TASTE_BITE_SEC;
		st.busyUntil = now + biteSec * 1000;
		Pin(vamp, land, dir, biteSec * 1000 + 500, st.biteSerial, GetGame().SurfaceY(land[0], land[2]) != GetGame().SurfaceY(to[0], to[2]));		// the taste too (J02)
		victim.m_AZVampRear = rear;
		int rearInt = 0;
		if (rear)
			rearInt = 1;
		if (taste)
		{
			AZ_VampFeed.ReleaseHint(vamp);
			victim.AZ_VampMarkBitten();
			victim.Message("Fangs at your neck, then gone. Your blood is too thin for it.", "colorImportant");
		}
		else
		{
			AZ_VampFeed.HoldTimed(vamp, victim, biteSec, cfg.chainMinBiteSec);
			victim.AZ_VampPin(victim.GetPosition(), victim.GetDirection());
		}
		AZ_VampReveal.BareAtWill(vamp, biteSec + 1);


		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ChainAnim, 200, false, vamp, victim, biteSec * 1000 + 1800, rearInt, st.biteSerial);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(EndBite, biteSec * 1000, false, vamp, victim, taste, st.biteSerial);
		Print("[AZ_Vamp] shadow step " + st.chain + " " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " " + Math.Round(vector.Distance(from, to)) + " m");
		return true;
	}






	static bool Strike(PlayerBase vamp)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!AZ_VampState.IsNightNow())
			return false;
		AZ_VampShadowState st = State(vamp);
		int now = GetGame().GetTime();
		if (now < st.busyUntil)
			return true;
		if (vamp.GetActionManager() && vamp.GetActionManager().GetRunningAction())
			return true;
		PlayerBase victim = FindTarget(vamp, cfg.strikeRange, 0.966, 1.2, null);
		if (!victim)
			return false;
		string why;
		if (now < st.strikeUntil)
			why = "Your strength is still returning (" + Math.Ceil((st.strikeUntil - now) / 1000.0) + " s).";
		else if (vamp.GetItemInHands())
			why = "Free your hands to strike.";
		else if (!vamp.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH))
			why = "Crouch. Strike unseen.";
		else if (vamp.GetHealth("", "Blood") > cfg.fullBlood)
			why = "You are full. The hunger will come back.";
		else if (victim.GetHealth("", "Blood") <= cfg.bloodFloor + 50)
			why = "Their blood is too thin.";
		else if (!FacingAway(vamp, victim))
			why = "They face you.";
		else if (LightNear(victim.GetPosition(), cfg.strikeLightM))
			why = "Fire burns near them.";
		else if (Watched(vamp, victim))
			why = "They would see you.";
		if (why != "")
		{
			if (now >= vamp.m_AZShadowWhyAt)
			{
				vamp.m_AZShadowWhyAt = now + 3000;
				vamp.Message(why, "colorAction");
			}
			return true;
		}
		vector from = vamp.GetPosition();
		vector to = victim.GetPosition();
		vector dir = victim.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector land = to - dir * AZ_VampFeed.ROOT_GAP;
		land[1] = GetGame().SurfaceY(land[0], land[2]);
		if (land[1] == GetGame().SurfaceY(to[0], to[2]) && (to[1] > land[1] + 0.3 || to[1] < land[1] - 2))
			land[1] = to[1];
		if (!AZ_VampReach.LevelBite(victim, land))
		{
			NoPath(vamp);
			return true;
		}
		if (Blocked(victim, land))
		{
			vamp.Message("No room behind them.", "colorAction");
			return true;
		}
		GetGame().RPCSingleParam(vamp, RPC_SHADOW, new Param1<vector>(from), true);
		GetGame().RPCSingleParam(vamp, RPC_SHADOW_FREEZE, new Param1<bool>(true), true);
		GetGame().RPCSingleParam(vamp, AZ_VampSound.RPC_KISS, new Param1<bool>(true), true);
		// Server placement plus the owner transform, so the bite lines up on every client.
		AZ_VampSnap.To(vamp, land, dir);
		float biteSec = Math.Max(AZ_VampFeed.MIN_J_FEED_SEC, cfg.strikeBiteSec);
		st.busyUntil = now + biteSec * 1000;
		st.strikeUntil = now + cfg.strikeCooldownSec * 1000;
		st.biteSerial++;
		Pin(vamp, land, dir, biteSec * 1000 + 500, st.biteSerial, GetGame().SurfaceY(land[0], land[2]) != GetGame().SurfaceY(to[0], to[2]));
		victim.m_AZVampRear = true;
		AZ_VampFeed.HoldTimed(vamp, victim, biteSec);
		victim.AZ_VampPin(victim.GetPosition(), victim.GetDirection());
		AZ_VampReveal.BareAtWill(vamp, biteSec + 1);

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StrikeAnim, 200, false, vamp, victim, biteSec * 1000 + 1800, st.biteSerial);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(EndStrike, biteSec * 1000, false, vamp, victim, st.biteSerial);
		if (vamp.GetIdentity())
			GetGame().RPCSingleParam(vamp, RPC_JBAR, new Param1<int>(cfg.strikeCooldownSec * 1000), true, vamp.GetIdentity());
		Print("[AZ_Vamp] shadow strike " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " " + Math.Round(vector.Distance(from, to)) + " m");
		return true;
	}


	static void StrikeAnim(PlayerBase vamp, PlayerBase victim, int ms, int serial)
	{
		AZ_VampShadowState st = s_State.Get(vamp);
		if (!st || st.biteSerial != serial)
			return;
		if (vamp && victim && vamp.m_AZVampTimedVictim == victim && victim.m_AZVampHeld && victim.m_AZVampFeeder == vamp)
			BiteAnim(vamp, 1, ms, 1);
	}

	static void EndStrike(PlayerBase vamp, PlayerBase victim, int serial)
	{
		AZ_VampShadowState st = s_State.Get(vamp);
		if (!st || st.biteSerial != serial)
			return;
		if (!vamp || vamp.m_AZVampTimedVictim != victim)
			return;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (vamp)
			State(vamp).busyUntil = 0;
		if (vamp && vamp.IsAlive() && !vamp.IsUnconscious() && victim && victim.IsAlive() && victim.m_AZVampHeld && victim.m_AZVampTimedFeed && victim.m_AZVampFeeder == vamp)
		{
			float take = Math.Max(0, victim.GetHealth("", "Blood") - cfg.bloodFloor);
			AZ_VampEvidence.Begin("shadow_strike_drain", vamp, victim, false);
			victim.AddHealth("", "Blood", -take);
			vamp.AddHealth("", "Blood", take * cfg.feedGain);
			AZ_VampEvidence.End("shadow_strike_drain", vamp, victim);
			AZ_VampFeed.Collapse(vamp, victim);
		}
		else
			AZ_VampFeed.Release(vamp, victim);
		if (vamp)
			BiteAnim(vamp, 2, 0, 1);
	}


	static bool FacingAway(PlayerBase vamp, PlayerBase victim)
	{
		vector look = victim.GetDirection();
		look[1] = 0;
		look.Normalize();
		vector away = victim.GetPosition() - vamp.GetPosition();
		away[1] = 0;
		away.Normalize();
		return vector.Dot(look, away) > 0.2;
	}


	static bool LightNear(vector pos, float r)
	{
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(pos, r, objs, proxies);
		foreach (Object o : objs)
		{
			PlayerBase h = PlayerBase.Cast(o);
			if (h)
			{
				if (!h.m_AZVampire && h.IsAlive() && AZ_VampRepel.LightReach(h, 1) > 0)
					return true;
				continue;
			}
			FireplaceBase fire = FireplaceBase.Cast(o);
			if (fire && fire.IsBurning())
				return true;
			ItemBase it = ItemBase.Cast(o);
			if (it && it.HasEnergyManager() && it.GetCompEM().IsWorking() && (Torch.Cast(it) || Roadflare.Cast(it) || Flashlight.Cast(it) || PortableGasLamp.Cast(it)))
				return true;
		}
		return false;
	}


	static bool Watched(PlayerBase vamp, PlayerBase victim)
	{
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(vamp.GetPosition(), 30, objs, proxies);
		foreach (Object o : objs)
		{
			PlayerBase h = PlayerBase.Cast(o);
			if (!h || h == vamp || h == victim || h.m_AZVampire || !h.IsAlive() || h.IsUnconscious())
				continue;
			vector look = h.GetDirection();
			look[1] = 0;
			look.Normalize();
			vector toVamp = vamp.GetPosition() - h.GetPosition();
			toVamp[1] = 0;
			toVamp.Normalize();
			if (vector.Dot(look, toVamp) > 0.5 && Sees(h, vamp))
				return true;
		}
		return false;
	}

	static const float COMPEL_BIND_M = 3.0;
	static const int COMPEL_STEP_MS = 1400;
	static const int STARE_MS = 900;




	static bool CompelStep(PlayerBase vamp)
	{

		if (!vamp || vamp.m_AZVampHeld || !vamp.IsAlive() || vamp.IsUnconscious() || vamp.IsInVehicle())
			return false;
		if (!vamp.m_AZBloodMoon || !AZ_VampEvent.Active())
			return false;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		AZ_VampShadowState st = State(vamp);
		if (!st.compelling || st.chain >= cfg.compelMax)
			return false;
		vector from = vamp.GetPosition();
		PlayerBase victim;
		vector to, face, land;
		bool ok = false;
		for (int tries = 0; tries < 6 && !ok; tries++)
		{
			victim = FindCompelTarget(vamp, cfg.shadowRange, st.bitten);
			if (!victim)
				return false;		// nobody left in range: not a path problem, so no "No clear path." (AZ-2627)
			to = victim.GetPosition();
			face = victim.GetDirection();
			face[1] = 0;
			face.Normalize();
			land = to + face * 0.9;
			if (Blocked(victim, land))
			{
				face = from - to;
				face[1] = 0;
				face.Normalize();
				land = to + face * 0.9;
			}


			ok = Sees(vamp, victim) && !Blocked(victim, land) && Clear(from + "0 1.1 0", land + "0 1.1 0", vamp, victim, ObjIntersectGeom);
			if (!ok)
				st.bitten.Insert(victim);
		}
		if (!ok)
		{
			NoPath(vamp);
			return false;
		}
		land[1] = GetGame().SurfaceY(land[0], land[2]);
		if (to[1] > land[1] + 0.3 || to[1] < land[1] - 2)
			land[1] = to[1];
		vector look = to - land;
		look[1] = 0;
		look.Normalize();
		// Validate the final computed landing before any move/FX.
		if (!JumpPath(vamp, victim, land))
		{
			NoPath(vamp);
			return false;
		}
		GetGame().RPCSingleParam(vamp, RPC_SHADOW, new Param1<vector>(from), true);
		AZ_VampSnap.AllowMove(vamp);
		vamp.SetPosition(land);
		vamp.SetDirection(look);
		AZ_VampReveal.BareAtWill(vamp, 3);
		if (!victim.IsUnconscious())
			victim.Message("Its eyes find yours.", "colorImportant");
		int now = GetGame().GetTime();
		st.chain++;
		st.bitten.Insert(victim);
		st.busyUntil = now + COMPEL_STEP_MS;
		st.biteSerial++;
		AZ_VampFeed.ReleaseHint(vamp);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Compel, STARE_MS, false, vamp, victim, st.biteSerial);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CompelNext, COMPEL_STEP_MS, false, vamp, st.biteSerial);
		Print("[AZ_Vamp] compel step " + st.chain + " " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim));
		return true;
	}


	// Blood-moon moves only: fail closed across floors, ladders and solid geometry (AZ-2575). AZ-2627: in the open the
	// ground itself, the people in the way (the one just drained lies in line with the next) and the loot at the feet
	// are not walls; the view ray above still has to reach the victim, so a hill or a wall between them still refuses
	// Which JumpPath check refused last (logged by NoPath, so refusal rates can be measured from live logs).
	static string s_PathWhy = "";
	static bool PathWhy(string why)
	{
		s_PathWhy = why;
		return false;
	}

	static bool JumpPath(PlayerBase vamp, PlayerBase victim, vector land)
	{
		if (!GetGame().IsServer() || !vamp || !victim)
			return false;
		if (vamp.GetCommand_Ladder() || victim.GetCommand_Ladder())
			return PathWhy("ladder");
		vector from = vamp.GetPosition();
		vector to = victim.GetPosition();
		// Retain the original feed target visibility requirement.
		if (!Clear(from + "0 1.6 0", to + "0 1.2 0", vamp, victim, ObjIntersectView))
			return PathWhy("sight");
		// Open terrain may slope; keep the height cap when either player is off terrain.
		bool fromOffTerrain = Math.AbsFloat(from[1] - GetGame().SurfaceY(from[0], from[2])) > 0.4;
		bool toOffTerrain = Math.AbsFloat(to[1] - GetGame().SurfaceY(to[0], to[2])) > 0.4;
		if ((fromOffTerrain || toOffTerrain) && (Math.AbsFloat(to[1] - from[1]) > 1.5 || Math.AbsFloat(land[1] - from[1]) > 1.5))
			return PathWhy("height");
		// both on the terrain: the rays may climb over the ground's own rises (a floor or a roof is never "open")
		float chest = -1;
		float feet = -1;
		if (!fromOffTerrain && !toOffTerrain)
		{
			chest = 1.1;
			feet = AZ_VampState.Cfg().jumpFeetRayM;
		}
		float feetY = AZ_VampState.Cfg().jumpFeetRayM;
		if (!JumpClear(from + "0 1.1 0", to + "0 1.1 0", vamp, victim, 0.25, chest))
			return PathWhy("chest");
		if (!JumpClear(from + Vector(0, feetY, 0), to + Vector(0, feetY, 0), vamp, victim, 0.15, feet))
			return PathWhy("feet");
		if (!JumpClear(from + "0 1.1 0", land + "0 1.1 0", vamp, victim, 0.25, chest))
			return PathWhy("chest_land");
		if (!JumpClear(from + Vector(0, feetY, 0), land + Vector(0, feetY, 0), vamp, victim, 0.15, feet))
			return PathWhy("feet_land");
		// Sweep the standing space too: a clear target is not a clear landing.
		if (!JumpClear(land + "0 0.35 0", land + "0 1.55 0", vamp, victim, 0.3, -1))
			return PathWhy("landing");
		return true;
	}

	// ground >= 0: the ray runs that high over open terrain, so a hit on the ground's surface is climbed over (the cast
	// goes on from just past it, lifted back to that height); ground < 0: the terrain and static geometry are walls.
	// People in the ray are stepped past like Clear() does; anything else solid refuses. At most 6 casts
	static bool JumpClear(vector a, vector b, PlayerBase vamp, PlayerBase victim, float radius, float ground)
	{
		vector from = a;
		Object skip = vamp;
		vector dir = b - a;
		dir.Normalize();
		for (int n = 0; n < 6; n++)
		{
			vector hit, contactDir;
			int component;
			set<Object> results = new set<Object>;
			// All objects prevents the victim from hiding a wall/ceiling in the same cast.
			if (!DayZPhysics.RaycastRV(from, b, hit, contactDir, component, results, null, skip, false, false, ObjIntersectGeom, radius, CollisionFlags.ALLOBJECTS))
				return true;
			if (results.Count() == 0)
			{
				// no object: the terrain or a map building. The ground's surface is at terrain height; a wall is not
				if (ground < 0 || Math.AbsFloat(hit[1] - GetGame().SurfaceY(hit[0], hit[2])) > 0.2)
					return false;
				from = hit + dir * 0.3;
				from[1] = GetGame().SurfaceY(from[0], from[2]) + ground;
				if (vector.Distance(from, b) < 0.3)
					return true;
				continue;
			}
			PlayerBase other = null;
			foreach (Object obj : results)
			{
				if (obj == vamp || obj == victim)
					continue;
				PlayerBase pb = PlayerBase.Cast(obj);
				if (!pb)
					return false;
				other = pb;
			}
			if (!other)
				return true;		// only the two of them in the set
			vector step = b - hit;
			if (step.Length() < 0.05)
				return true;
			from = hit;
			skip = other;
		}
		return false;
	}

	static void NoPath(PlayerBase vamp)
	{
		if (!GetGame().IsServer() || !vamp)
			return;
		AZ_VampShadowState st = State(vamp);
		int now = GetGame().GetTime();
		if (now < st.pathWhyAt)
			return;
		st.pathWhyAt = now + 3000;
		vamp.Message("No clear path.", "colorAction");
		Print("[AZ_Vamp] shadow nopath " + AZ_VampState.Uid(vamp) + " reason=" + s_PathWhy);
	}

	static bool Sees(PlayerBase vamp, PlayerBase victim)
	{
		vector eye = vamp.GetPosition() + "0 1.6 0";
		vector body = victim.GetPosition() + "0 1.1 0";
		if (victim.IsUnconscious())
			body = victim.GetPosition() + "0 0.3 0";
		return Clear(eye, body, vamp, victim, ObjIntersectView) && Clear(eye, body, vamp, victim, ObjIntersectGeom);
	}



	static bool Clear(vector a, vector b, Object ignore, Object victim, int layer)
	{



		vector from = a;
		Object skip = ignore;
		for (int n = 0; n < 6; n++)
		{
			vector hit, nrm;
			int comp;
			set<Object> res = new set<Object>;
			if (!DayZPhysics.RaycastRV(from, b, hit, nrm, comp, res, null, skip, false, false, layer, 0.1))
				return true;
			if (res.Count() == 0)
				return false;
			PlayerBase other = null;
			foreach (Object o : res)
			{
				if (o == victim)
					return true;
				PlayerBase pb = PlayerBase.Cast(o);
				if (!pb)
					return false;
				other = pb;
			}


			vector step = b - hit;
			if (step.Length() < 0.05)
				return true;
			from = hit;
			skip = other;
		}
		return false;
	}



	static bool CompelProtected(PlayerBase h)
	{
		if (!h)
			return true;
		return AZ_VampCrypt.IsGuest(h) || AZ_VampState.InSpawnGrace(h) || h.m_AZOnAltar || AZ_VampRitual.s_Lying == h || AZ_VampRitual.s_Kneelers.Find(h) >= 0;
	}


	static PlayerBase FindCompelTarget(PlayerBase vamp, float range, array<PlayerBase> skip)
	{
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(vamp.GetPosition(), range, objs, proxies);
		PlayerBase best;
		float bestD = range + 1;
		foreach (Object o : objs)
		{
			PlayerBase h = PlayerBase.Cast(o);
			if (!h || h == vamp || !h.IsAlive() || h.m_AZVampire || h.m_AZVampHeld || h.m_AZVampCompelled || h.IsRestrained() || CompelProtected(h))
				continue;
			if (skip && skip.Find(h) >= 0)
				continue;
			float d = vector.Distance(vamp.GetPosition(), h.GetPosition());
			if (d < bestD)
			{
				bestD = d;
				best = h;
			}
		}
		return best;
	}


	static void Compel(PlayerBase vamp, PlayerBase victim, int serial)
	{
		AZ_VampShadowState st = s_State.Get(vamp);
		if (!st || st.biteSerial != serial || !st.compelling)
			return;
		if (!victim || !victim.IsAlive() || victim.m_AZVampCompelled || victim.IsRestrained())
			return;
		if (!vamp || !vamp.IsAlive() || vamp.IsUnconscious() || !vamp.m_AZBloodMoon || !AZ_VampEvent.Active())
			return;


		if (victim.m_AZVampHeld || CompelProtected(victim) || vector.Distance(vamp.GetPosition(), victim.GetPosition()) > COMPEL_BIND_M || !Sees(vamp, victim))
		{
			Print("[AZ_Vamp] compel: " + AZ_VampState.Uid(victim) + " got away before the bind (range, sight or protected)");
			return;
		}
		victim.m_AZVampCompelled = true;
		victim.SetSynchDirty();
		if (!AZ_VampTie.Bind(victim, AZ_VampTie.COMPEL_BONDS))
			Print("[AZ_Vamp] compel: no bonds for " + AZ_VampState.Uid(victim) + ", held still only");
		victim.m_AZCompelFreeAt = 0;
		if (victim.IsUnconscious())
		{

			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Uncompel, 3000, false, victim);
			return;
		}
		victim.Message("Your hands bind themselves. You cannot move.", "colorImportant");
		victim.m_AZCompelFreeAt = GetGame().GetTime() + AZ_VampState.Cfg().compelSec * 1000;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Uncompel, AZ_VampState.Cfg().compelSec * 1000, false, victim);
	}

	static void Uncompel(PlayerBase victim)
	{
		if (!victim || !victim.m_AZVampCompelled)
			return;
		if (victim.IsAlive() && victim.IsUnconscious())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Uncompel, 3000, false, victim);
			return;
		}
		if (victim.IsAlive() && victim.m_AZCompelFreeAt == 0)
		{

			victim.m_AZCompelFreeAt = GetGame().GetTime() + AZ_VampState.Cfg().compelSec * 1000;
			victim.Message("You wake bound. Its will still holds you.", "colorImportant");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Uncompel, AZ_VampState.Cfg().compelSec * 1000, false, victim);
			return;
		}
		victim.m_AZVampCompelled = false;
		victim.SetSynchDirty();
		AZ_VampTie.Unbind(victim);
		victim.Message("The compulsion fades. Your limbs are your own again.", "colorAction");
	}


	static void CompelNext(PlayerBase vamp, int serial)
	{
		if (!vamp)
			return;
		AZ_VampShadowState st = State(vamp);
		if (st.biteSerial != serial || !st.compelling)
			return;
		st.busyUntil = 0;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		st.chainUntil = GetGame().GetTime() + cfg.chainWindowSec * 1000;
		if (st.chain >= cfg.compelMax || !CompelStep(vamp))
		{
			st.chain = 0;
			st.bitten.Clear();
			st.compelling = false;
			vamp.Message("They stand where you left them.", "colorAction");
		}
	}




	static bool Blocked(PlayerBase victim, vector land, Object also = null)
	{
		vector a = victim.GetPosition() + "0 1.1 0";
		vector b = land + "0 1.1 0";
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (!DayZPhysics.RaycastRV(a, b, hit, nrm, comp, res, null, victim, false, false, ObjIntersectGeom, 0.25))
			return false;
		if (!also || res.Count() == 0)
			return true;
		foreach (Object o : res)
		{
			if (o != also)
				return true;
		}
		return false;
	}


	static void EndBite(PlayerBase vamp, PlayerBase victim, bool taste = false, int serial = 0)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!vamp)
		{
			if (victim && !victim.m_AZVampFeeder)
				AZ_VampFeed.Release(null, victim);
			return;
		}
		AZ_VampShadowState st = State(vamp);
		if (st.biteSerial != serial || st.victim != victim)
			return;
		if (!vamp.IsAlive() || vamp.IsUnconscious() || AZ_VampDown.Weak(vamp) || vamp.IsInVehicle() || !vamp.m_AZVampire || !vamp.m_AZBloodMoon || !AZ_VampEvent.Active() || vamp.GetItemInHands() || (vamp.GetActionManager() && vamp.GetActionManager().GetRunningAction()) || !victim || !victim.IsAlive() || (!taste && (!victim.m_AZVampHeld || victim.m_AZVampFeeder != vamp)))
		{
			Abort(vamp);
			return;
		}
		st.victim = null;

		if (taste)
		{

		}
		else if (vamp && victim && victim.IsAlive())
		{
			float take = Math.Max(0, victim.GetHealth("", "Blood") - cfg.bloodFloor);
			AZ_VampEvidence.Begin("shadow_chain_drain", vamp, victim, false);
			victim.AddHealth("", "Blood", -take);
			vamp.AddHealth("", "Blood", take * cfg.feedGain);
			AZ_VampEvidence.End("shadow_chain_drain", vamp, victim);
			s_InChain = true;
			AZ_VampFeed.Collapse(vamp, victim);
			s_InChain = false;
		}
		else
			AZ_VampFeed.Release(vamp, victim);
		if (!vamp)
			return;
		int now = GetGame().GetTime();
		st.busyUntil = 0;
		if (st.chain >= cfg.chainMax)
		{
			st.chain = 0;
			st.bitten.Clear();
			st.coolUntil = now + cfg.shadowCooldownSec * 1000;
			Exhaust(vamp);
		}
		else
		{
			st.chainUntil = now + cfg.chainWindowSec * 1000;
			st.coolUntil = now + cfg.shadowCooldownSec * 1000;

			if (cfg.chainAuto)
			{
				AZ_VampVictimCB running = AZ_VampVictimCB.Cast(vamp.GetCommand_Action());
				if (!vamp.GetIdentity() && (!running || !running.AZ_CanContinueDrink()))
				{
					BiteAnim(vamp, 2, 0, 0);
					st.busyUntil = now + AI_CHAIN_WAIT_MS;
					AIChainAdvance(vamp, st.biteSerial, st.busyUntil);
					return;
				}
				TryStep(vamp);
				// TryStep can return true on a blocked path without starting a feed.
				if (st.victim)
					return;
				st.chain = 0;
				st.bitten.Clear();
				Exhaust(vamp);
			}
			else if (FindTarget(vamp, cfg.shadowRange, -1, AZ_VampReach.START_RANGE, st.bitten))
			{
				// Keep the manual chain alive only for its existing input window.
				st.animWaiting = true;
				BiteAnim(vamp, 1, cfg.chainWindowSec * 1000 + 1800, 0);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(EndChainAnim, cfg.chainWindowSec * 1000, false, vamp, st.biteSerial);
				vamp.Message("Another heart, racing... (Fangs: {AZ_FANGS})", "colorAction");
				return;
			}
		}
		BiteAnim(vamp, 2, 0, 0);
	}

	static void EndChainAnim(PlayerBase vamp, int serial)
	{
		AZ_VampShadowState st = s_State.Get(vamp);
		if (vamp && st && st.biteSerial == serial && !st.victim)
		{
			st.animWaiting = false;
			BiteAnim(vamp, 2, 0, 0);
		}
	}


	static void AIChainAdvance(PlayerBase vamp, int serial, int deadline)
	{
		if (!vamp)
			return;
		AZ_VampShadowState st = s_State.Get(vamp);
		if (!st || st.biteSerial != serial || st.victim || st.chain <= 0)
			return;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (vamp.GetIdentity() || !vamp.IsAlive() || vamp.IsUnconscious() || vamp.IsInVehicle() || vamp.m_AZVampHeld || !vamp.m_AZVampire || !vamp.m_AZBloodMoon || !AZ_VampEvent.Active() || !cfg.chainAuto || st.compelling || vamp.GetItemInHands() || (vamp.GetActionManager() && vamp.GetActionManager().GetRunningAction()))
		{
			Abort(vamp);
			return;
		}
		int now = GetGame().GetTime();
		if (now >= deadline)
		{
			Print("[AZ_Vamp] AI chain stopped: previous bite command did not release");
			Abort(vamp);
			return;
		}
		if (vamp.GetCurrentCommandID() != DayZPlayerConstants.COMMANDID_MOVE || vamp.GetCommand_Action() || vamp.GetCommandModifier_Action())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AIChainAdvance, AI_CHAIN_POLL_MS, false, vamp, serial, deadline);
			return;
		}
		st.busyUntil = 0;
		st.chainUntil = now + cfg.chainWindowSec * 1000;
		if (!TryStep(vamp))
		{
			st.chain = 0;
			st.bitten.Clear();
			Exhaust(vamp);
		}
	}


	static void Exhaust(PlayerBase vamp)
	{
		GetGame().RPCSingleParam(vamp, AZ_VampSound.RPC_SIGH, new Param1<bool>(true), true);

		vamp.m_AZVampFedUntil = GetGame().GetTime() + AZ_VampState.Cfg().fedShowSec * 3000;
		vamp.m_AZVampFrenzyUntil = GetGame().GetTime() + AZ_VampState.Cfg().frenzySec * 1000;
		AZ_VampBlood.Set(vamp, true);
		State(vamp).compelUntil = GetGame().GetTime() + AZ_VampState.Cfg().compelWindowSec * 1000;
		if (vamp.m_AZBloodMoon && AZ_VampState.Cfg().compelSec > 0)
			AZ_VampTie.Hint(vamp, "compel", "Press Fangs ({AZ_FANGS}) again: your will binds the rest where they stand.");
		AZ_VampTrail.Start(vamp);
		AZ_VampReveal.Update(vamp);
		if (vamp.GetStaminaHandler())
			vamp.GetStaminaHandler().SetStamina(0);


		for (int i = 0; i < 4; i++)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DayZPlayerSyncJunctures.SendKuruRequest, i * 1500, false, vamp, 1.0);
		vamp.Message("The frenzy ebbs. You are blood-drunk and spent.", "colorAction");
		if (vamp.GetIdentity())
			GetGame().RPCSingleParam(vamp, RPC_JBAR, new Param1<int>(AZ_VampState.Cfg().shadowCooldownSec * 1000), true, vamp.GetIdentity());
	}

	static void ChainAnim(PlayerBase vamp, PlayerBase victim, int ms, int rear, int serial)
	{
		if (!vamp)
			return;
		AZ_VampShadowState st = s_State.Get(vamp);
		if (st && st.biteSerial == serial && st.victim == victim && vamp.IsAlive() && !vamp.IsUnconscious())
			BiteAnim(vamp, 1, ms, rear);
	}



	static void BiteAnim(PlayerBase vamp, int what, int ms, int rear)
	{
		if (!vamp)
			return;
		if (what == 1 && !AZ_VampFeedAnim.Available())
		{
			Print("[AZ_Vamp] shadow bite anim skipped: AZ_VampireAnims_Feed missing");
			return;
		}
		if (!vamp.GetIdentity())
		{

			if (what == 1)
				AZ_VampFeedAnim.DummyFeedStart(vamp, rear == 1);
			else
				AZ_VampFeedAnim.DummyFeedEnd(vamp);
			return;
		}
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(what);
		ctx.Write(ms);
		ctx.Write(rear);
		vamp.SendSyncJuncture(SJ_SHADOW_BITE, ctx);
	}


	static PlayerBase FindTarget(PlayerBase vamp, float range, float cone, float minD, array<PlayerBase> skip, bool requireSight = true)
	{
		vector eye = vamp.GetPosition() + "0 1.6 0";
		vector fwd = vamp.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(vamp.GetPosition(), range, objs, proxies);
		PlayerBase best;
		float bestD = range + 1;
		foreach (Object o : objs)
		{
			PlayerBase h = PlayerBase.Cast(o);
			if (!h || h == vamp || !h.IsAlive() || h.m_AZVampire || h.IsUnconscious() || h.m_AZVampHeld || h.IsInVehicle())
				continue;
			if (AZ_VampState.IsImmune(h) || h.m_AZVampTurning || h.m_AZVampCompelled || h.IsRestrained())
				continue;
			if (!AZ_VampState.BiteReady(h, vamp.m_AZBloodMoon) || AZ_VampCrypt.IsGuest(h))
				continue;
			if (skip && skip.Find(h) >= 0)
				continue;
			vector to = h.GetPosition() - vamp.GetPosition();
			float d = to.Length();
			if (d < minD || d > range || d >= bestD)
				continue;
			vector flat = to;
			flat[1] = 0;
			flat.Normalize();
			if (vector.Dot(flat, fwd) < cone)
				continue;
			vector chest = h.GetPosition() + "0 1.2 0";
			if (requireSight && !Clear(eye, chest, vamp, h, ObjIntersectView))
				continue;
			best = h;
			bestD = d;
		}
		return best;
	}
}

modded class PlayerBase
{
	override void EEKilled(Object killer)
	{
		m_AZShadowInputUntil = 0;
		if (GetGame().IsServer())
			AZ_VampShadow.Abort(this);
		super.EEKilled(killer);
	}

	override void EEDelete(EntityAI parent)
	{
		m_AZShadowInputUntil = 0;
		if (GetGame().IsServer())
			AZ_VampShadow.Forget(this);		// J03: no record outlives its entity
		super.EEDelete(parent);
	}

	int m_AZShadowAnimReq;
	int m_AZShadowAnimMs;
	int m_AZShadowAnimLogAt;
	int m_AZJCoolMs;
	int m_AZJReadyAt;
	int m_AZShadowAnimRear;
	int m_AZShadowAnimStaleUntil;

	// AZ-2510: owner input only, between an accepted bite step and its running clip.
	int m_AZShadowInputUntil;

	bool AZ_ShadowInputFrozen()
	{
		if (!GetGame().IsClient() || !IsControlledPlayer())
			return false;
		if (!IsAlive() || IsUnconscious() || GetGame().GetTime() >= m_AZShadowInputUntil)
		{
			m_AZShadowInputUntil = 0;
			return false;
		}
		AZ_VampVictimCB running = AZ_VampVictimCB.Cast(GetCommand_Action());
		if (running && running.AZ_CanContinueDrink())
		{
			m_AZShadowInputUntil = 0;
			return false;
		}
		return m_AZShadowInputUntil > 0;
	}

	void AZ_ShadowFreezeInput()
	{
		if (!AZ_ShadowInputFrozen())
			return;
		HumanInputController shadowInput = GetInputController();
		if (!shadowInput)
			return;
		shadowInput.OverrideMovementSpeed(HumanInputControllerOverrideType.ONE_FRAME, 0);
		shadowInput.OverrideMovementAngle(HumanInputControllerOverrideType.ONE_FRAME, 0);
		shadowInput.OverrideRaise(HumanInputControllerOverrideType.ONE_FRAME, false);
		shadowInput.OverrideMeleeEvade(HumanInputControllerOverrideType.ONE_FRAME, false);
		shadowInput.OverrideAimChangeX(HumanInputControllerOverrideType.ONE_FRAME, 0);
		shadowInput.OverrideAimChangeY(HumanInputControllerOverrideType.ONE_FRAME, 0);
	}

	override bool CanJump()
	{
		if (AZ_ShadowInputFrozen())
			return false;
		return super.CanJump();
	}

	override bool CanClimb(int climbType, SHumanCommandClimbResult climbRes)
	{
		if (AZ_ShadowInputFrozen())
			return false;
		return super.CanClimb(climbType, climbRes);
	}

	override void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		super.OnSyncJuncture(pJunctureID, pCtx);
		if (pJunctureID != AZ_VampShadow.SJ_SHADOW_BITE)
			return;
		int what, ms, rear;
		if (pCtx.Read(what) && pCtx.Read(ms))
		{
			if (what == 2)
				m_AZShadowInputUntil = 0;
			m_AZShadowAnimReq = what;
			m_AZShadowAnimStaleUntil = 0;
			m_AZShadowAnimMs = ms;
			m_AZShadowAnimRear = 0;
			if (pCtx.Read(rear))
				m_AZShadowAnimRear = rear;
			Print("[AZ_Vamp] shadow bite juncture " + what + " ms " + ms + " server " + GetGame().IsServer());
		}
	}

	// AZ-2627: the vampire's own pin for a shadow bite (AZ_VampShadow.Pin); server only
	vector m_AZShadowPinPos;
	bool m_AZShadowPinLevel;
	vector m_AZShadowPinDir;
	bool m_AZShadowPinned;
	int m_AZShadowPinFixes;
	int m_AZShadowPinAt;
	int m_AZShadowPinUntil;
	int m_AZShadowPinSerial;

	// re-snap the vampire to its landing while its bite runs: the strike's timed hold, or the chain step it was pinned
	// for (a taste has no hold, J02); released the moment neither is there, or the deadline passes
	void AZ_ShadowPinTick()
	{
		int now = GetGame().GetTime();
		PlayerBase held = m_AZVampTimedVictim;
		bool holdBite = held && held.m_AZVampHeld && held.m_AZVampFeeder == this;
		AZ_VampShadowState st = AZ_VampShadow.s_State.Get(this);
		bool chainBite = st && st.victim && st.biteSerial == m_AZShadowPinSerial;
		if (!IsAlive() || now >= m_AZShadowPinUntil || (!holdBite && !chainBite))
		{
			if (m_AZShadowPinFixes > 0)
				Print("[AZ_Vamp] shadow pin released " + AZ_VampState.Uid(this) + " after " + m_AZShadowPinFixes + " fixes");
			m_AZShadowPinned = false;
			return;
		}
		if (now < m_AZShadowPinAt)
			return;
		vector off = GetPosition() - m_AZShadowPinPos;
		off[1] = 0;
		if (off.Length() > AZ_VampFeedAnim.SERVER_PIN_TOLERANCE || (m_AZShadowPinLevel && Math.AbsFloat(GetPosition()[1] - m_AZShadowPinPos[1]) > 0.02))
		{
			vector to = m_AZShadowPinPos;
			if (!m_AZShadowPinLevel)
				to[1] = GetPosition()[1];
			AZ_VampSnap.To(this, to, m_AZShadowPinDir);		// server placement and the owner transform again
			m_AZShadowPinFixes++;
			m_AZShadowPinAt = now + 500;
		}
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		// Apply before vanilla command processing can consume movement/jump/raise input.
		AZ_ShadowFreezeInput();
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (GetGame().IsServer())
		{
			AZ_VampShadowState pending = AZ_VampShadow.s_State.Get(this);
			if (pending && (pending.victim || pending.animWaiting) && (!IsAlive() || IsUnconscious() || IsInVehicle() || !m_AZVampire || !m_AZBloodMoon || !AZ_VampEvent.Active() || GetItemInHands() || (GetActionManager() && GetActionManager().GetRunningAction())))
				AZ_VampShadow.Abort(this);
			if (m_AZShadowPinned)
				AZ_ShadowPinTick();
		}
		if (m_AZShadowAnimReq == 0)
			return;
		AZ_VampVictimCB cb = AZ_VampVictimCB.Cast(GetCommand_Action());
		if (m_AZShadowAnimReq == 2)
		{
			if (cb)
			{
				cb.m_AZDrinker = false;
				cb.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			}
			m_AZShadowAnimReq = 0;
			return;
		}
		if (cb)
		{
			if (cb.AZ_CanContinueDrink())
			{
				cb.Init(m_AZShadowAnimMs);
				m_AZShadowInputUntil = 0;
				m_AZShadowAnimReq = 0;
				return;
			}
			// A real stale/end-phase clip gets one hard interrupt, never END every frame.
			if (m_AZShadowAnimStaleUntil == 0)
			{
				AZShadowAnimLog("old clip still on, ending it");
				cb.m_AZDrinker = false;
				cb.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
				m_AZShadowAnimStaleUntil = GetGame().GetTime() + 500;
			}
			else if (GetGame().GetTime() >= m_AZShadowAnimStaleUntil)
			{
				AZShadowAnimLog("stale clip did not release; dropping start");
				m_AZShadowInputUntil = 0;
				m_AZShadowAnimReq = 0;
			}
			return;
		}
		HumanCommandActionCallback other = GetCommand_Action();
		if (!other)
			other = GetCommandModifier_Action();
		if (other)
		{
			AZShadowAnimLog("interrupting " + other.ClassName());
			other.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
			return;
		}
		if (pCurrentCommandID != DayZPlayerConstants.COMMANDID_MOVE)
		{
			AZShadowAnimLog("waits, cmd " + pCurrentCommandID);
			return;
		}
		m_AZShadowAnimReq = 0;
		int cmd = AZ_VampFeedAnim.CMD_VAMP;
		// AZ-2627: ERECT only, also for the crouched strike: with a single stance in the mask the engine stands the
		// player up before the clip (a prone or crouched vampire never bites from below). The rear clip is the strike's
		// (Strike sets m_AZVampRear) and the chain's when the rear pack is present
		int stances = DayZPlayerConstants.STANCEMASK_ERECT;
		if (m_AZShadowAnimRear == 1)
			cmd = AZ_VampRear.CMD_VAMP;
		cb = AZ_VampVictimCB.Cast(StartCommand_Action(cmd, AZ_VampVictimCB, stances));
		if (cb)
		{
			cb.m_AZDrinker = true;
			cb.Init(m_AZShadowAnimMs);
			m_AZShadowInputUntil = 0;
		}
		Print("[AZ_Vamp] shadow bite clip start " + (cb != null) + " server " + GetGame().IsServer() + " cmd " + pCurrentCommandID);
	}


	protected void AZShadowAnimLog(string what)
	{
		if (m_AZShadowAnimLogAt > GetGame().GetTime())
			return;
		m_AZShadowAnimLogAt = GetGame().GetTime() + 500;
		Print("[AZ_Vamp] shadow bite clip " + what + " server " + GetGame().IsServer());
	}



	void AZ_Streak(vector from, vector to)
	{
		vector a = from + "0 1.0 0";
		vector b = to + "0 1.0 0";


		AZ_DashSound("AZ_ElderVanish_SoundSet", a);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_DashSound, AZ_VampShadow.STREAK_MS, false, "AZ_ElderAppear_SoundSet", b);
		if (GetGame().ConfigIsExisting("CfgSoundSets AZ_VampSwish_SoundSet"))
		{
			EffectSound w = SEffectManager.PlaySound("AZ_VampSwish_SoundSet", (a + b) * 0.5);
			if (w)
				w.SetSoundAutodestroy(true);
		}
		for (int i = 0; i < AZ_VampShadow.STREAK_PUFFS; i++)
		{
			float k = i * 1.0 / (AZ_VampShadow.STREAK_PUFFS - 1);
			int delay = k * AZ_VampShadow.STREAK_MS;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_StreakPuff, delay, false, a + (b - a) * k);
		}
	}

	void AZ_StreakPuff(vector at)
	{
		Particle.PlayInWorld(ParticleList.AZ_VAMP_ASH_BURST, at);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == AZ_VampShadow.RPC_JBAR && GetGame().IsClient())
		{
			Param1<int> cool;
			if (ctx.Read(cool))
			{
				m_AZJCoolMs = cool.param1;
				m_AZJReadyAt = GetGame().GetTime() + cool.param1;
			}
			return;
		}
		if (rpc_type == AZ_VampShadow.RPC_STREAK && GetGame().IsClient())
		{
			Param2<vector, vector> st = new Param2<vector, vector>(vector.Zero, vector.Zero);
			if (ctx.Read(st))
				AZ_Streak(st.param1, st.param2);
			return;
		}
		if (rpc_type == AZ_VampShadow.RPC_SHADOW_FREEZE && GetGame().IsClient())
		{
			Param1<bool> fz;
			if (ctx.Read(fz) && fz.param1 && IsControlledPlayer() && IsAlive() && !IsUnconscious())
			{
				m_AZShadowInputUntil = GetGame().GetTime() + 1500;
				AZ_ShadowFreezeInput();
			}
			return;
		}
		if (rpc_type != AZ_VampShadow.RPC_SHADOW || !GetGame().IsClient())
			return;
		Param1<vector> p;
		if (!ctx.Read(p))
			return;
		Particle.PlayInWorld(ParticleList.AZ_VAMP_ASH_BURST, p.param1 + "0 1.0 0");
	}

	void AZ_DashSound(string soundSet, vector at)
	{
		if (!GetGame().ConfigIsExisting("CfgSoundSets " + soundSet))
			return;
		EffectSound s = SEffectManager.PlaySound(soundSet, at);
		if (s)
			s.SetSoundAutodestroy(true);
	}
}
