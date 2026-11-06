



class AZ_VampTrail
{
	static ref array<Object> s_Drops = new array<Object>;

	static void Start(PlayerBase p)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!p || cfg.trailSec <= 0)
			return;
		p.m_AZTrailUntil = GetGame().GetTime() + cfg.trailSec * 1000;
		p.m_AZTrailLast = p.GetPosition();
	}


	static void Tick(PlayerBase p, int now)
	{
		if (now >= p.m_AZTrailUntil)
			return;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		vector pos = p.GetPosition();
		if (vector.Distance(pos, p.m_AZTrailLast) < cfg.trailStepM || p.IsInVehicle())
			return;
		p.m_AZTrailLast = pos;
		vector at = pos + Vector(Math.RandomFloat(-0.3, 0.3), 0, Math.RandomFloat(-0.3, 0.3));
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		if (pos[1] > at[1] + 1.0 || pos[1] < at[1] - 2)
			at[1] = pos[1];
		else
		{


			vector n = GetGame().SurfaceGetNormal(at[0], at[2]);
			float slope = Math.Sqrt(Math.Max(0, 1 - n[1] * n[1])) / Math.Max(n[1], 0.2);
			at[1] = at[1] + 0.03 + slope * 0.16;
		}
		Object o = GetGame().CreateObjectEx("AZ_VampDecal_BloodDrops", at, ECE_KEEPHEIGHT | ECE_NOLIFETIME);
		if (!o)
			return;
		o.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
		s_Drops.Insert(o);
		while (s_Drops.Count() > cfg.trailCap)
		{
			GetGame().ObjectDelete(s_Drops[0]);
			s_Drops.Remove(0);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Fade, cfg.trailLifeSec * 1000, false, o);
	}




	static void Scorch(PlayerBase p, int now)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!p || cfg.scorchStepSec <= 0 || now < p.m_AZScorchAt || p.IsInVehicle())
			return;
		vector pos = p.GetPosition();
		if (p.m_AZScorchAt > 0 && vector.Distance(pos, p.m_AZScorchLast) < cfg.scorchStepM)
			return;
		p.m_AZScorchAt = now + cfg.scorchStepSec * 1000;
		p.m_AZScorchLast = pos;
		vector at = pos;
		float ground = GetGame().SurfaceY(at[0], at[2]);
		if (pos[1] > ground + 1.0)
			at[1] = pos[1] + 0.02;
		else
		{
			vector n = GetGame().SurfaceGetNormal(at[0], at[2]);
			float slope = Math.Sqrt(Math.Max(0, 1 - n[1] * n[1])) / Math.Max(n[1], 0.2);
			at[1] = ground + 0.035 + slope * 0.35;
		}
		Object o = GetGame().CreateObjectEx("AZ_VampDecal_Scorch", at, ECE_KEEPHEIGHT | ECE_NOLIFETIME);
		if (!o)
			return;
		o.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
		s_Drops.Insert(o);
		while (s_Drops.Count() > cfg.trailCap)
		{
			GetGame().ObjectDelete(s_Drops[0]);
			s_Drops.Remove(0);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Fade, cfg.trailLifeSec * 1000, false, o);
	}

	static void Fade(Object o)
	{
		int i = s_Drops.Find(o);
		if (i < 0)
			return;
		s_Drops.Remove(i);
		GetGame().ObjectDelete(o);
	}
}

modded class PlayerBase
{
	int m_AZTrailUntil;
	vector m_AZTrailLast;
	int m_AZScorchAt;
	vector m_AZScorchLast;
}
