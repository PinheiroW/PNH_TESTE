class AZ_VampHunger
{
	static bool NoHeal(PlayerBase p)
	{
		if (!p || !p.m_AZVampire)
			return false;
		AZ_VampConfig c = AZ_VampState.Cfg();
		bool hungry = p.GetHealth("", "Blood") < c.hungryBlood;
		if (!hungry || !c.hungerNoHealOn)
			p.m_AZHungerNoHealSaid = false;
		if (!c.hungerNoHealOn || !hungry)
			return false;
		if (!p.m_AZHungerNoHealSaid && GetGame().IsServer())
		{
			p.m_AZHungerNoHealSaid = true;
			p.Message("The shade will not mend you while you hunger.", "colorAction");
			Print("[AZ_Vamp] hunger noheal " + AZ_VampState.Uid(p) + " blood=" + p.GetHealth("", "Blood"));
		}
		return true;
	}
}





class AZ_VampRest
{
	// Keep night and direct-sun regeneration unchanged; only shade by day is slower.
	static float DayHealFactor(PlayerBase p)
	{
		if (GetGame().GetWorld().IsNight() || AZ_VampSun.Exposed(p))
			return 1;
		return Math.Clamp(AZ_VampState.Cfg().dayShadeHealFactor, 0, 1);
	}

	static bool Resting(PlayerBase p)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (cfg.restSettleSec <= 0 || !p.IsAlive() || GetGame().GetWorld().IsNight())
		{
			p.m_AZRestSince = 0;
			return false;
		}
		int now = GetGame().GetTime();
		vector here = p.GetPosition();
		bool still = vector.Distance(here, p.m_AZRestAt) < 0.3;
		p.m_AZRestAt = here;
		if (!still || !Roofed(p) || AZ_VampSun.Exposed(p))
		{
			if (p.m_AZRestSince > 0 && now - p.m_AZRestSince > cfg.restSettleSec * 1000)
				p.Message("You stir. The day is still out there.", "colorAction");
			p.m_AZRestSince = 0;
			return false;
		}
		if (p.m_AZRestSince == 0)
			p.m_AZRestSince = now;
		if (now - p.m_AZRestSince < cfg.restSettleSec * 1000)
			return false;
		if (now - p.m_AZRestSince < cfg.restSettleSec * 1000 + AZ_VampReveal.TICK_MS)
			p.Message("You rest in the dark like the dead. The hunger sleeps with you.", "colorFriendly");
		// Low blood can rest safely, but Health recovery requires feeding.
		if (!AZ_VampHunger.NoHeal(p) && p.GetHealth("", "Blood") >= cfg.hungryBlood)
			p.AddHealth("", "Health", cfg.restHealPerSec * DayHealFactor(p) * AZ_VampReveal.TICK_MS / 1000.0);
		return true;
	}


	static bool Roofed(PlayerBase p)
	{
		vector from = p.GetPosition() + "0 1.0 0";
		vector to = from + "0 12 0";
		vector hit, normal;
		int comp;
		set<Object> results = new set<Object>;
		return DayZPhysics.RaycastRV(from, to, hit, normal, comp, results, null, p, false, false, ObjIntersectView);
	}
}

modded class PlayerBase
{
	bool m_AZHungerNoHealSaid;
	int m_AZRestSince;
	vector m_AZRestAt;
}
