



class AZ_VampWolves
{
	static const int PACKS = 0;
	static const int PACK_SIZE = 3;
	static ref array<Object> s_Wolves = new array<Object>;
	static bool s_Was;
	static int s_NextTry;
	static const int RETRY_MS = 120000;



	static const int MAX_LIVE = 9;
	static const float A1_KEEP_OUT = 450.0;
	static const vector A1_AT = "3777 152 8244";
	static int s_PacksTonight;
	static int s_A1Packs;

	static void Watch()
	{
		bool on = AZ_VampProgress.s_BloodMoon && AZ_VampEvent.Active();
		if (on != s_Was)
		{
			s_Was = on;
			s_NextTry = 0;
			s_PacksTonight = 0;
			s_A1Packs = 0;
			if (!on)
			{
				Clear();
				return;
			}
		}


		if (!on || GetGame().GetTime() < s_NextTry)
			return;
		s_NextTry = GetGame().GetTime() + RETRY_MS;
		Spawn();
	}

	static void Spawn()
	{
		int alive = 0;
		for (int i = s_Wolves.Count() - 1; i >= 0; i--)
		{
			EntityAI e = EntityAI.Cast(s_Wolves[i]);
			if (e && e.IsAlive())
				alive++;
			else
				s_Wolves.Remove(i);
		}
		int want = PACKS - s_PacksTonight;
		if (want <= 0 || alive + PACK_SIZE > MAX_LIVE)
			return;
		array<PlayerBase> everyone;
		AZ_VampState.OnlinePlayers(everyone);
		array<PlayerBase> humans = new array<PlayerBase>;
		foreach (PlayerBase p : everyone)
		{
			if (p && p.IsAlive() && !p.m_AZVampire && !InSpawnZone(p.GetPosition()))
				humans.Insert(p);
		}
		if (humans.Count() == 0)
		{
			Print("[AZ_Vamp] blood moon wolves: no living human outside the spawn zone to place a pack near (" + everyone.Count() + " online), retry in " + (RETRY_MS / 1000) + " s");
			return;
		}
		int packs = 0;
		for (int t = 0; t < want * 4 && packs < want; t++)
		{
			PlayerBase near = humans.GetRandomElement();
			float a = Math.RandomFloat(0, Math.PI2);
			float r = Math.RandomFloat(180, 260);
			vector c = near.GetPosition() + Vector(Math.Cos(a) * r, 0, Math.Sin(a) * r);
			if (GetGame().SurfaceIsSea(c[0], c[2]) || GetGame().SurfaceIsPond(c[0], c[2]) || InSpawnZone(c))
				continue;
			for (int w = 0; w < PACK_SIZE; w++)
			{
				vector wp = c + Vector(Math.RandomFloat(-4, 4), 0, Math.RandomFloat(-4, 4));
				wp[1] = GetGame().SurfaceY(wp[0], wp[2]);
				Object wolf = GetGame().CreateObjectEx("Animal_CanisLupus_Grey", wp, ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_CREATEPHYSICS);
				if (wolf)
					s_Wolves.Insert(wolf);
			}
			packs++;
			s_PacksTonight++;
		}
		Print("[AZ_Vamp] blood moon wolves: " + packs + " new packs (" + s_PacksTonight + "/" + PACKS + " tonight, " + s_A1Packs + " near A1), " + s_Wolves.Count() + " wolves out");
	}


	static bool InSpawnZone(vector p)
	{
		return p[2] >= AZ_VampState.Cfg().spawnZoneMinZ - 300;
	}

	static void Clear()
	{
		int n = 0;
		foreach (Object w : s_Wolves)
		{
			EntityAI e = EntityAI.Cast(w);
			if (e && e.IsAlive())
			{
				GetGame().ObjectDelete(e);
				n++;
			}
		}
		s_Wolves.Clear();
		Print("[AZ_Vamp] blood moon over: " + n + " wolves gone");
	}
}
