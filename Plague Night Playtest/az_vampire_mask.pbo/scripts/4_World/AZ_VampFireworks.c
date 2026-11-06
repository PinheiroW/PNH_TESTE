



class AZ_VampFireworks
{
	static ref array<Object> s_Launchers;
	static bool s_OpeningDone;

	static void Dusk(bool opening)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (cfg.fireworksDelaySec <= 0 || !cfg.halloween)
			return;
		bool hw = AZ_VampProgress.RealHalloween();
		if (!hw && (!opening || s_OpeningDone))
			return;
		if (!hw)
			s_OpeningDone = true;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Launch, cfg.fireworksDelaySec * 1000, false);
		Print("[AZ_Vamp] fireworks tonight in " + cfg.fireworksDelaySec + " s");
	}

	static void Launch()
	{
		if (!AZ_VampLandmarks.s_Fires || !GetGame().GetWorld().IsNight())
			return;
		Clear();
		s_Launchers = new array<Object>;
		foreach (Fireplace fire : AZ_VampLandmarks.s_Fires)
		{
			if (!fire)
				continue;
			vector at;
			if (!AZ_VampProps.OpenSpot(fire.GetPosition(), 5, 10, 1.5, at))
				continue;
			FireworksLauncher fl = FireworksLauncher.Cast(GetGame().CreateObjectEx("FireworksLauncher", at, ECE_PLACE_ON_SURFACE | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD));
			if (!fl)
				continue;
			fl.SetOrientation(vector.Zero);
			fl.OnIgnitedThis(null);
			s_Launchers.Insert(fl);
		}
		Print("[AZ_Vamp] fireworks lit at " + s_Launchers.Count() + " village fires");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Clear);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Clear, 300000, false);
	}

	static void Clear()
	{
		if (!s_Launchers)
			return;
		foreach (Object o : s_Launchers)
		{
			if (o)
				GetGame().ObjectDelete(o);
		}
		s_Launchers = null;
	}
}
