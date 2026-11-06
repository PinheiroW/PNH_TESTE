








modded class AZ_VampConfig
{
	bool eldersSmoke = false;
}

modded class PlayerBase
{
	bool m_AZElder;
	int m_AZElderWrathAt;

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (!m_AZElder || !GetGame().IsServer() || !IsAlive() || !source)
			return;
		Print("[AZ_Vamp] elder hit by " + source.GetType() + " type " + damageType + " ammo " + ammo);
		PlayerBase by = PlayerBase.Cast(source.GetHierarchyRootPlayer());
		if (!by)
			by = PlayerBase.Cast(source);
		if (by && by != this)
			AZ_VampElderWrath.Hit(this, by);
	}
}

class AZ_VampElderWrath
{
	static const int BITE_MS = 5000;
	static const float BLOOD_FLOOR = 3000;
	static const int KILL_FROM = 3;
	static const int COOLDOWN_MS = 12000;
	static const float REACH_M = 30.0;
	static ref map<string, int> s_Offences;
	static int s_Night;
	// F01: the bite's delayed end must still own its victim when it fires: the elder alive and awake, the hold still
	// its own (an ally's rescue releases it), and no newer wrath bite on that victim since
	static int s_BiteSerial;
	static ref map<string, int> s_BiteSerialOf;

	static int NextBite(PlayerBase by)
	{
		if (!s_BiteSerialOf)
			s_BiteSerialOf = new map<string, int>;
		s_BiteSerial++;
		s_BiteSerialOf.Set(AZ_VampState.Uid(by), s_BiteSerial);
		return s_BiteSerial;
	}

	static bool BiteOwned(PlayerBase e, PlayerBase by, int serial)
	{
		if (!e || !e.IsAlive() || e.IsUnconscious() || !by || !by.m_AZVampHeld || (by.m_AZVampFeeder != e && by.m_AZVampFeeder2 != e))
			return false;
		return s_BiteSerialOf && s_BiteSerialOf.Get(AZ_VampState.Uid(by)) == serial;
	}

	static void Hit(PlayerBase e, PlayerBase by)
	{
		if (by.m_AZElder || !by.IsAlive() || by.IsUnconscious() || by.m_AZVampHeld)
			return;
		int now = GetGame().GetTime();
		if (now < e.m_AZElderWrathAt || Busy(e))
			return;
		if (vector.Distance(e.GetPosition(), by.GetPosition()) > REACH_M)
			return;
		e.m_AZElderWrathAt = now + COOLDOWN_MS;
		int n = Offence(AZ_VampState.Uid(by));
		Print("[AZ_Vamp] elder wrath " + AZ_VampState.Uid(by) + " offence " + n + " on " + e.GetType() + " at hall " + AZ_VampCrypt.HallLocal(e.GetPosition()));
		vector home = e.GetPosition();
		vector dir = by.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector behind = by.GetPosition() - dir * AZ_VampFeed.ROOT_GAP;
		GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_STREAK, new Param2<vector, vector>(home, behind), true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Bite, AZ_VampCryptLurkers.SWISH_MS, false, e, by, n, home);
	}

	static int Offence(string uid)
	{
		int night = AZ_VampCryptExit.NightKey();
		if (!s_Offences || night != s_Night)
		{
			s_Offences = new map<string, int>;
			s_Night = night;
		}
		int n = 1;
		if (s_Offences.Contains(uid))
			n = s_Offences.Get(uid) + 1;
		s_Offences.Set(uid, n);
		return n;
	}


	static bool Busy(PlayerBase e)
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase p : list)
		{
			if (p && p.m_AZVampHeld && (p.m_AZVampFeeder == e || p.m_AZVampFeeder2 == e))
				return true;
		}
		return false;
	}

	static void Bite(PlayerBase e, PlayerBase by, int n, vector home)
	{
		if (!e || !e.IsAlive() || !by || !by.IsAlive() || by.m_AZVampHeld)
			return;
		e.SetHealth("", "Blood", 2500);
		ActionAZVampBite.SnapJoin(e, by, true);
		GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(e.GetPosition()), true);
		AZ_VampFeed.Hold(e, by);
		AZ_VampFeedAnim.DummyFeedStart(e, true);
		by.Message("You struck one of the old ones.", "colorImportant");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BiteEnd, BITE_MS, false, e, by, n, home, NextBite(by));
	}

	static void BiteEnd(PlayerBase e, PlayerBase by, int n, vector home, int serial = 0)
	{
		if (by && by.IsAlive() && !BiteOwned(e, by, serial))
			Print("[AZ_Vamp] elder wrath on " + AZ_VampState.Uid(by) + " was broken before its end: no punishment");
		else if (by && by.IsAlive())
		{
			AZ_VampEvidence.Begin("elder_wrath_ko_or_lethal_drain", e, by, false);
			by.m_AZVampCollapsed = true;
			by.SetHealth("", "Shock", 0);
			DayZPlayerSyncJunctures.SendPlayerUnconsciousness(by, true);
			if (n >= KILL_FROM)
				by.SetHealth("", "Blood", 0);
			else if (by.GetHealth("", "Blood") < BLOOD_FLOOR)
				by.SetHealth("", "Blood", BLOOD_FLOOR);
			AZ_VampEvidence.End("elder_wrath_ko_or_lethal_drain", e, by);
			AZ_VampFeed.Release(e, by);
			Print("[AZ_Vamp] elder wrath done with " + AZ_VampState.Uid(by) + ": offence " + n + " blood " + by.GetHealth("", "Blood") + " unconscious " + by.IsUnconscious() + " alive " + by.IsAlive());
		}
		if (!e)
			return;
		AZ_VampFeedAnim.DummyFeedEnd(e);
		e.SetHealth("", "Blood", 5000);
		if (e.IsAlive())
			AZ_VampCryptLurkers.Dash(e, home, home);
	}


	static void Smoke(PlayerBase e)
	{
		if (e)
			e.SetAllowDamage(!AZ_VampState.Cfg().eldersSmoke);
	}

	static string Toggle(string arg)
	{
		if (arg == "on")
			AZ_VampState.Cfg().eldersSmoke = true;
		else if (arg == "off")
			AZ_VampState.Cfg().eldersSmoke = false;
		array<PlayerBase> all = new array<PlayerBase>;
		if (AZ_VampCryptLurkers.s_River)
			all.InsertAll(AZ_VampCryptLurkers.s_River);
		if (AZ_VampCryptLurkers.s_Audience)
			all.InsertAll(AZ_VampCryptLurkers.s_Audience);
		foreach (PlayerBase e : all)
			Smoke(e);
		Print("[AZ_Vamp] elders smoke " + AZ_VampState.Cfg().eldersSmoke + " (" + all.Count() + " standing)");
		return "elders smoke " + AZ_VampState.Cfg().eldersSmoke + ": the ritual's elders are " + Says(AZ_VampState.Cfg().eldersSmoke) + " (usage: /vamp eldersmoke on|off)";
	}

	static string Says(bool smoke)
	{
		if (smoke)
			return "untouchable smoke";
		return "attackable, and they bite back";
	}
}
