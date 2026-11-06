









class AZ_VampGuest
{
	static const int GUEST_MS = 20000;
	static const float CAMP_M = 12.0;
	static const int CAMP_WARN_MS = 60000;
	static const int CAMP_VISIT_MS = 90000;
	static int s_LastGuestAt;


	static void Landed(PlayerBase p)
	{
		if (!p || p.m_AZVampire)
			return;
		p.m_AZGuestUntil = GetGame().GetTime() + GUEST_MS;
		s_LastGuestAt = GetGame().GetTime();
	}

	static bool IsGuest(PlayerBase p)
	{
		return p && p.m_AZGuestUntil > GetGame().GetTime();
	}


	static void Struck(PlayerBase who, PlayerBase by)
	{
		if (!who || !by || by == who)
			return;
		by.m_AZGuestUntil = 0;
		if (!IsGuest(who) || by.m_AZElder)
			return;
		if (Answer(by, "struck a guest"))
			Print("[AZ_Vamp] guest wrath " + AZ_VampState.Uid(by) + " (struck " + AZ_VampState.Uid(who) + ")");
	}


	static bool Answer(PlayerBase by, string what)
	{
		if (!by || by.m_AZElder || !by.IsAlive() || by.IsUnconscious() || by.m_AZVampHeld)
			return false;
		string uid = AZ_VampState.Uid(by);
		int now = GetGame().GetTime();
		if (AZ_VampRitual.s_ProtectAt.Contains(uid) && now - AZ_VampRitual.s_ProtectAt.Get(uid) < AZ_VampRitual.PROTECT_COOLDOWN_MS)
			return false;
		AZ_VampRitual.s_ProtectAt.Set(uid, now);
		vector dir = by.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector from = by.GetPosition() - dir * 3.0;
		PlayerBase e = AZ_VampCryptLurkers.Dress(from, 0, 11);
		if (!e)
			return false;
		e.SetDirection(dir);
		GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(from), true);
		AZ_VampElderWrath.Hit(e, by);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampRitual.ProtectGone, AZ_VampRitual.PROTECT_GONE_MS, false, e);
		Print("[AZ_Vamp] the household answers: " + uid + " " + what);
		return true;
	}


	static void CampTick(array<PlayerBase> list)
	{
		if (AZ_VampCrypt.EntryOn())
			return;
		array<vector> slots = AZ_VampCryptLedge.Slots();
		vector mid = (slots[1] + slots[4]) * 0.5;
		int now = GetGame().GetTime();
		foreach (PlayerBase p : list)
		{
			if (!p)
				continue;
			bool skip = !p.IsAlive() || p.m_AZVampire || p.m_AZElder || IsGuest(p) || AZ_VampRitual.Protecting(p) || AZ_VampRitual.StillKneeling(p);
			if (skip || vector.Distance(p.GetPosition(), mid) > CAMP_M)
			{
				p.m_AZCampSince = 0;
				p.m_AZCampStage = 0;
				continue;
			}
			if (p.m_AZCampSince == 0 || s_LastGuestAt > p.m_AZCampSince)
			{
				p.m_AZCampSince = now;
				p.m_AZCampStage = 0;
				continue;
			}
			int held = now - p.m_AZCampSince;
			if (p.m_AZCampStage == 0 && held >= CAMP_WARN_MS)
			{
				p.m_AZCampStage = 1;
				if (p.GetIdentity())
					GetGame().RPCSingleParam(p, AZ_VampCrypt.RPC_WHISPER, new Param1<int>(1), true, p.GetIdentity());
				Print("[AZ_Vamp] camper notice " + AZ_VampState.Uid(p) + " " + (held / 1000));
			}
			else if (p.m_AZCampStage == 1 && held >= CAMP_VISIT_MS)
			{
				p.m_AZCampStage = 2;
				Print("[AZ_Vamp] camper notice " + AZ_VampState.Uid(p) + " " + (held / 1000) + " (an elder visits)");
				Answer(p, "waited at the arrivals");
			}
		}
	}
}
