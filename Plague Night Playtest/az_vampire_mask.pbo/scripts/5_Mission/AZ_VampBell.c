




modded class MissionGameplay
{
	protected float m_AZBellCheck;
	protected int m_AZBellHour = -1;
	protected ref EffectSound m_AZBell;
	protected int m_AZBellLastMs = -1;

	static const float BELL_FAR = 900;
	// Backstop: one midnight bell per 10 real minutes, whatever the clock does.
	static const int BELL_COOLDOWN_MS = 600000;



	static vector AZBellSource(vector from)
	{
		array<vector> spots = {Vector(6885.9, 33.0, 10976.3), Vector(6892.9, 15.2, 10946.1), Vector(6910.7, 15.1, 11004.5), Vector(8672.1, 15.0, 10088.1), Vector(5497.2, 118.1, 8176.8), Vector(1189.5, 3.3, 11837.8), Vector(4524.1, 13.8, 10403.3), Vector(4119.5, 385.2, 6624.5)};
		if (vector.Distance(spots[0], from) <= BELL_FAR)
			return spots[0];
		vector best = spots[0];
		foreach (vector s : spots)
		{
			if (vector.Distance(s, from) < vector.Distance(best, from))
				best = s;
		}
		if (vector.Distance(best, from) <= BELL_FAR)
			return best;
		vector dir = best - from;
		dir.Normalize();
		return from + dir * BELL_FAR;
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		m_AZBellCheck -= timeslice;
		if (m_AZBellCheck > 0)
			return;
		m_AZBellCheck = 1;
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		int nowMs = GetGame().GetTime();
		bool cool = m_AZBellLastMs < 0 || nowMs - m_AZBellLastMs >= BELL_COOLDOWN_MS;
		if (m_AZBellHour == 23 && h == 0 && mi < 5 && cool)
		{
			Man player = GetGame().GetPlayer();
			if (player)
			{
				m_AZBellLastMs = nowMs;
				string world;
				GetGame().GetWorldName(world);
				world.ToLower();
				if (world == "namalsk")
				{
					vector src = AZBellSource(player.GetPosition());
					m_AZBell = SEffectManager.PlaySound("AZ_VampChurchBell_SoundSet", src);
					Print("[AZ_Vamp] bell at " + src + ", " + Math.Round(vector.Distance(src, player.GetPosition())) + " m away, sound " + (m_AZBell != null));
				}
				else
					m_AZBell = SEffectManager.PlaySound("AZ_VampMidnightBell_SoundSet", player.GetPosition());
			}
		}
		m_AZBellHour = h;
	}
}
