






class AZ_VampReveal
{
	static const int TICK_MS = 2000;
	static int s_Ticks;

	static void Start()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, true);
	}

	static void Tick()
	{
		if (!GetGame().IsServer())
			return;
		s_Ticks++;
		bool saveSick = (s_Ticks % 30) == 0;
		int now = GetGame().GetTime();
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		foreach (PlayerBase p : list)
		{
			if (p.m_AZVampTurning)
				AZ_VampTurn.SickTick(p, now, saveSick);
			if (!p.m_AZVampire)
				continue;
			bool resting = false;
			if (AZ_VampEvent.Active())
			{
				AZ_VampHunger.NoHeal(p);
				resting = AZ_VampRest.Resting(p);
				float blood = p.GetHealth("", "Blood");
				AZ_VampEvidence.BeginDrain(p, blood > cfg.vampDrainFloor && !resting);
				if (blood > cfg.vampDrainFloor && !resting)
					p.AddHealth("", "Blood", -Math.Min(cfg.vampDrainPerSec * TICK_MS / 1000.0, blood - cfg.vampDrainFloor));
				AZ_VampEvidence.EndDrain(p, blood > cfg.vampDrainFloor && !resting);
			}
			Update(p);
			StarveTick(p, now, resting);
			AZ_VampTrail.Tick(p, now);
			AZ_VampPerks.Tick(p, GetGame().GetWorld().IsNight(), p.m_AZVampHungry);
			if (AZ_VampEvent.Active())
			{
				AZ_VampRepel.Tick(p, list);
				AZ_VampKin.Tick(p, list);
			}
		}
	}

	// Pure helpers are exercised offline from this source (not a copied model).
	static int StarveHunger(float blood, float hungryBlood, float drainFloor)
	{
		if (blood >= hungryBlood)
			return 0;
		return 1 + Math.Round(9 * Math.Clamp((hungryBlood - blood) / Math.Max(1, hungryBlood - drainFloor), 0, 1));
	}

	static bool StarveThreshold(int hunger, int threshold)
	{
		return hunger > 0 && hunger >= threshold;
	}

	static bool StarveGracePassed(int now, int since, int bornAt, float graceMin, float newbornMin)
	{
		return now - since >= Math.Max(0, graceMin) * 60000 && (bornAt < 0 || now - bornAt >= Math.Max(0, newbornMin) * 60000);
	}

	static float StarveDamage(float health, float rate, float floor, float seconds)
	{
		return Math.Min(Math.Max(0, rate) * Math.Max(0, seconds), Math.Max(0, health - Math.Max(1, floor)));
	}

	static int StarveRange(int hunger, bool dull, bool enabled, int starvingRange)
	{
		if (!enabled)
			return 0;
		if (dull)
			return Math.Clamp(starvingRange, 1, 1000);
		if (hunger <= 0)
			return 20;
		return Math.Round(30 + 40 * (hunger - 1) / 9.0);
	}

	static bool StarveCanWither(bool ready, bool resting, bool down, bool unconscious)
	{
		return ready && !resting && !down && !unconscious;
	}

	static void StarveReset(PlayerBase p)
	{
		p.m_AZStarveSince = -1;
		p.m_AZStarveSaid = false;
		p.m_AZStarveFloorSaid = false;
	}

	static void StarveTick(PlayerBase p, int now, bool resting)
	{
		if (!p || !p.IsAlive())
			return;
		AZ_VampConfig c = AZ_VampState.Cfg();
		bool active = AZ_VampEvent.Active() && p.m_AZVampire;
		int hunger = StarveHunger(p.GetHealth("", "Blood"), c.hungryBlood, c.vampDrainFloor);
		bool starving = active && StarveThreshold(hunger, c.starveHunger);
		if (!starving)
			StarveReset(p);
		else if (p.m_AZStarveSince < 0)
			p.m_AZStarveSince = now;
		bool ready = starving && StarveGracePassed(now, p.m_AZStarveSince, p.m_AZStarveBornAt, c.starveGraceMin, c.starveNewbornGraceMin);
		// Range replaces only the base range; existing progression multipliers stay client-side.
		int range = StarveRange(p.m_AZVampHunger, ready, active && c.hungerSensesDullOn, c.hbStarvingRangeM);
		if (range != p.m_AZHbRangeM)
		{
			p.m_AZHbRangeM = range;
			p.SetSynchDirty();
			int effectiveBase = range;
			if (range == 0)
				effectiveBase = StarveRange(p.m_AZVampHunger, false, true, c.hbStarvingRangeM);
			Print("[AZ_Vamp] hbrange " + AZ_VampState.Uid(p) + " range=" + effectiveBase + " synced=" + range + " hunger=" + hunger);
		}
		if (!StarveCanWither(ready, resting, AZ_VampDown.IsDown(p), p.IsUnconscious()))
			return;
		if (!c.hungerSensesDullOn && c.starveHealthPerSec <= 0)
			return;
		if (!p.m_AZStarveSaid)
		{
			p.m_AZStarveSaid = true;
			p.Message("You are starving. The heartbeats grow faint and your body begins to wither. Feed.", "colorImportant");
			Print("[AZ_Vamp] starve " + AZ_VampState.Uid(p) + " begin hunger=" + hunger);
		}
		float health = p.GetHealth("", "Health");
		float damage = StarveDamage(health, c.starveHealthPerSec, c.starveHealthFloor, TICK_MS / 1000.0);
		if (damage <= 0)
			return;
		p.AddHealth("", "Health", -damage);
		if (!p.m_AZStarveFloorSaid && health - damage <= Math.Max(1, c.starveHealthFloor))
		{
			p.m_AZStarveFloorSaid = true;
			p.Message("Withered with hunger. Only blood will mend you.", "colorImportant");
			Print("[AZ_Vamp] starve " + AZ_VampState.Uid(p) + " floor health=" + p.GetHealth("", "Health"));
		}
	}

	static string Wanted(PlayerBase p)
	{
		int now = GetGame().GetTime();
		// U10c fix 2 (4 Nov): a wash ends the bloody looks at once, even mid-frenzy (frenzySec 180 outlasted the wash).
		if (now < p.m_AZVampFrenzyUntil && p.m_AZVampBloodied)
			return "AZ_VampireMask_Frenzy";
		if (now < p.m_AZVampFedUntil && p.m_AZVampBloodied)
			return "AZ_VampireMask_Fed";
		float blood = p.GetHealth("", "Blood");
		float limit = AZ_VampState.Cfg().hungryBlood;
		if (p.m_AZVampHungry)
			limit += 100;
		p.m_AZVampHungry = AZ_VampEvent.Active() && blood < limit;

		int hunger = 0;
		if (p.m_AZVampHungry)
		{
			AZ_VampConfig cfg = AZ_VampState.Cfg();
			hunger = 1 + Math.Round(9 * Math.Clamp((cfg.hungryBlood - blood) / Math.Max(1, cfg.hungryBlood - cfg.vampDrainFloor), 0, 1));
		}
		if (hunger != p.m_AZVampHunger)
		{
			p.m_AZVampHunger = hunger;
			p.SetSynchDirty();
		}


		if (p.m_AZVampHungry || now < p.m_AZVampBareUntil || AZ_VampProgress.s_BloodMoon)
			return "AZ_VampireMask_Bared";
		return "AZ_VampireMask";
	}


	static void Update(PlayerBase p)
	{
		if (!p || !GetGame().IsServer() || !p.IsAlive() || !p.m_AZVampire || p.m_AZVampSwapping)
			return;
		EntityAI worn = p.FindAttachmentBySlotName("Mask");
		if (!worn)
		{
			// No mask for real (blocked by headgear, removed): clients show the maskless look, not a pending swap.
			LookPending(p, 0);
			if (GetGame().GetTime() < p.m_AZVampFaceRetryAt)
				return;
			p.m_AZVampFaceRetryAt = GetGame().GetTime() + 10000;


			p.m_AZVampSwapping = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Attach, 100, false, p, Wanted(p), 0);
			Print("[AZ_Vamp] look " + AZ_VampState.Uid(p) + " had no face: re-attach " + Wanted(p));
			return;
		}
		// The swap landed (its mask is in the slot): the pending look is no longer needed.
		LookPending(p, 0);
		if (!worn.IsKindOf("AZ_VampireMask_Base"))
			return;
		string want = Wanted(p);
		if (worn.GetType() == want)
			return;
		p.m_AZVampSwapping = true;
		// Synced before the delete: observers keep the look being swapped to while the Mask slot is empty.
		LookPending(p, AZ_VampFace.TypeKind(want));
		GetGame().ObjectDelete(worn);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Attach, 100, false, p, want, 0);
		Print("[AZ_Vamp] look " + AZ_VampState.Uid(p) + " -> " + want);
	}

	static void Attach(PlayerBase p, string type, int tries)
	{
		if (!p)
			return;
		if (!p.IsAlive())
		{
			LookPending(p, 0);
			return;
		}
		// A cure can finish while the look swap is queued.
		if (!p.m_AZVampire)
		{
			p.m_AZVampSwapping = false;
			LookPending(p, 0);
			return;
		}
		if (!p.FindAttachmentBySlotName("Mask") && p.GetInventory().CreateAttachment(type))
		{
			p.m_AZVampSwapping = false;
			return;
		}
		if (tries < 10)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Attach, 200, false, p, type, tries + 1);
		else
		{
			p.m_AZVampSwapping = false;
			LookPending(p, 0);
			string hg = "none";
			if (p.FindAttachmentBySlotName("Headgear"))
				hg = p.FindAttachmentBySlotName("Headgear").GetType();
			string mk = "none";
			if (p.FindAttachmentBySlotName("Mask"))
				mk = p.FindAttachmentBySlotName("Mask").GetType();
			Print("[AZ_Vamp] look " + AZ_VampState.Uid(p) + " FAILED to attach " + type + " (mask slot " + mk + ", headgear " + hg + ")");
		}
	}

	// Face sync: the look a server mask swap is heading to (AZ_VampFace.MASK_*), 0 when none.
	static void LookPending(PlayerBase p, int kind)
	{
		if (!p || p.m_AZVampLookPending == kind)
			return;
		p.m_AZVampLookPending = kind;
		p.SetSynchDirty();
	}

	static void BareAtWill(PlayerBase p, float sec)
	{
		if (!p || !p.m_AZVampire)
			return;
		p.m_AZVampBareUntil = Math.Max(p.m_AZVampBareUntil, GetGame().GetTime() + sec * 1000);
		Update(p);
	}

	static void Fed(PlayerBase p)
	{
		if (!p)
			return;
		StarveReset(p);
		p.m_AZVampFedUntil = GetGame().GetTime() + AZ_VampState.Cfg().fedShowSec * 1000;
		AZ_VampBlood.Set(p, true);
		AZ_VampTrail.Start(p);
		Update(p);
	}
}

modded class PlayerBase
{
	int m_AZStarveSince = -1;
	int m_AZStarveBornAt = -1;
	bool m_AZStarveSaid;
	bool m_AZStarveFloorSaid;
	int m_AZVampBareUntil;
	int m_AZVampFedUntil;
	int m_AZVampFrenzyUntil;
	int m_AZVampFaceRetryAt;
	bool m_AZVampHungry;
	bool m_AZVampSwapping;
	int m_AZVampLookPending;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableInt("m_AZVampLookPending", 0, 4);
	}
}
