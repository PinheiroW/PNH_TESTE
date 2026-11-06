


class AZ_VampSunDebug
{
	static bool s_On;
	static int s_Last = -1;
	static float s_DotTimer;

	static void Toggle()
	{
		s_On = !s_On;
		s_Last = -1;
		GetGame().Chat("[vamp] sun debug " + s_On, "colorAction");
	}
}

modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (!AZ_VampSunDebug.s_On)
			return;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		vector from, to;
		bool exposed = AZ_VampSun.Exposed(p, from, to);
		if (exposed)
		{

			Debug.DrawLine(from, to, 0xFFFF8C1A, ShapeFlags.ONCE | ShapeFlags.NOZBUFFER);
			AZ_VampSunDebug.s_DotTimer -= timeslice;
			if (AZ_VampSunDebug.s_DotTimer <= 0)
			{
				AZ_VampSunDebug.s_DotTimer = 0.4;
				vector dir = (to - from).Normalized();
				for (int i = 1; i <= 12; i++)
				{
					Particle dot = Particle.PlayInWorld(ParticleList.DEBUG_DOT, from + dir * (i * 1.5));
					if (dot)
						GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(dot.Stop, 450, false);
				}
			}
		}
		int now = exposed;
		if (now != AZ_VampSunDebug.s_Last)
		{
			AZ_VampSunDebug.s_Last = now;
			string state = "shade";
			if (exposed)
				state = "IN THE SUN";
			if (!AZ_VampSun.SunUp())
				state = "night / sun down";
			GetGame().Chat("[vamp] " + state, "colorAction");
		}
	}
}
