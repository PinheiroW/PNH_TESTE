




class AZ_VampCryptSurface
{
	static const float MARGIN = 40.0;
	static const int SWEEP_MS = 12000;
	static const int LOG_MS = 60000;
	static ref array<Object> s_Tracked = new array<Object>;
	static bool s_Running;
	static int s_Deleted;
	static int s_LastLog;


	static bool OnSurfaceAbove(vector pos)
	{
		if (!GetGame().IsServer() || !AZ_VampCrypt.Site())
			return false;
		vector h = AZ_VampCrypt.HallLocal(pos);
		if (h[0] <= -AZ_VampCrypt.HALF_LEN - 3 - MARGIN || h[0] >= AZ_VampCrypt.APSE_F + 13 + MARGIN || Math.AbsFloat(h[1]) >= 14.5 + MARGIN)
			return false;
		if (AZ_VampCrypt.Inside(pos))
			return false;
		return pos[1] >= GetGame().SurfaceY(pos[0], pos[2]) - 2;
	}

	static void Track(Object o)
	{
		if (!GetGame().IsServer() || !o || !AZ_VampCrypt.Site())
			return;
		s_Tracked.Insert(o);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Check, 1000, false, o);
		if (!s_Running)
		{
			s_Running = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Sweep, SWEEP_MS, true);
		}
	}

	static void Untrack(Object o)
	{
		int i = s_Tracked.Find(o);
		if (i >= 0)
			s_Tracked.Remove(i);
	}

	static bool Purge(Object o)
	{
		EntityAI e = EntityAI.Cast(o);
		if (!e || !e.IsAlive() || e.GetHierarchyParent())
			return false;
		if (!OnSurfaceAbove(e.GetPosition()))
			return false;
		e.Delete();
		s_Deleted++;
		return true;
	}

	static void Check(Object o)
	{
		Purge(o);
	}

	static void Sweep()
	{
		int removed = 0;
		for (int i = s_Tracked.Count() - 1; i >= 0; i--)
		{
			Object o = s_Tracked[i];
			if (!o)
			{
				s_Tracked.Remove(i);
				continue;
			}
			if (Purge(o))
			{
				s_Tracked.Remove(i);
				removed++;
			}
		}
		int now = GetGame().GetTime();
		if (removed > 0 && now - s_LastLog >= LOG_MS)
		{
			s_LastLog = now;
			Print("[AZ_Vamp] crypt surface: " + removed + " infected/animals deleted above the crypt this sweep (" + s_Deleted + " in total, " + s_Tracked.Count() + " tracked)");
		}
	}
}

modded class ZombieBase
{
	override void EEInit()
	{
		super.EEInit();
		AZ_VampCryptSurface.Track(this);
	}

	override void EEDelete(EntityAI parent)
	{
		AZ_VampCryptSurface.Untrack(this);
		super.EEDelete(parent);
	}
}

modded class AnimalBase
{
	override void EEInit()
	{
		super.EEInit();
		AZ_VampCryptSurface.Track(this);
	}

	override void EEDelete(EntityAI parent)
	{
		AZ_VampCryptSurface.Untrack(this);
		super.EEDelete(parent);
	}
}
