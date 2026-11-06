




class AZ_VampMist
{
	static const int EVERY_MS = 60000;
	static int s_NextAt;
	static bool s_On;
	static int s_Map = -1;


	static void Tick()
	{
		if (s_Map < 0)
		{
			string world;
			GetGame().GetWorldName(world);
			world.ToLower();
			s_Map = 0;
			if (world == "namalsk")
				s_Map = 1;
			Print("[AZ_Vamp] night mist: world " + world + ", on Namalsk " + s_Map);
		}
		if (s_Map != 1)
			return;
		float target = AZ_VampState.Cfg().nightMist;
		bool want = target > 0 && AZ_VampEvent.Active() && GetGame().GetWorld().IsNight();
		int now = GetGame().GetTime();
		if (want == s_On && now < s_NextAt)
			return;
		s_NextAt = now + EVERY_MS;
		Fog fog = GetGame().GetWeather().GetFog();
		if (want)
		{
			target = Math.Clamp(target, 0.02, 0.3);
			fog.SetLimits(0, Math.Min(1, target + 0.12));
			if (!s_On)
				Print("[AZ_Vamp] night mist on, target " + target);
			s_On = true;
			if (fog.GetForecast() < target - 0.03)
				fog.Set(Math.RandomFloatInclusive(target - 0.03, target + 0.06), 600, 1800);
		}
		else if (s_On)
		{
			s_On = false;
			fog.SetLimits(0, 1);
			fog.Set(Math.Min(fog.GetActual(), 0.03), 900, 1800);
			Print("[AZ_Vamp] night mist off");
		}
	}
}
