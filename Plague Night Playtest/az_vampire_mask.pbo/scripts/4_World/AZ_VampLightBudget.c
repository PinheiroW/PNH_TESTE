





class AZ_VampLightBudget
{
	static const float ON_M = 30;
	static const float KEEP_M = 35;

	static const int MAX_LIT = 6;
	static const int RPC = 4702360;
	static int s_Cap = 4;
	static const float POOL_GAP_M = 12;
	static ref array<ScriptedLightBase> s_Sources;
	static int s_OtherCount;

	static void Track(ScriptedLightBase source)
	{
		if (!s_Sources)
			s_Sources = new array<ScriptedLightBase>;
		s_Sources.Insert(source);
	}

	static int OtherLights(vector camera)
	{
		int count = 0;
		if (!s_Sources)
			return count;
		for (int si = s_Sources.Count() - 1; si >= 0; si--)
		{
			ScriptedLightBase source = s_Sources[si];
			if (!source)
			{
				s_Sources.Remove(si);
				continue;
			}
			if (!source.IsEnabled())
				continue;
			EntityAI owner = EntityAI.Cast(source.m_Parent);
			if (owner && s_All.Find(owner) >= 0)
				continue;
			vector position = source.GetPosition();
			if (source.m_Parent)
				position = source.m_Parent.GetPosition();

			if (vector.DistanceSq(position, camera) <= 10000)
				count++;
		}
		return count;
	}
	static const int TICK_MS = 1000;
	static const int LOG_MS = 5000;




	static const bool TEST_ALL = false;
	static int s_Mode;
	static bool Active()
	{
		if (s_Mode == 0)
		{
			s_Mode = 1;
			if (!GetGame().IsDedicatedServer() && !FileExist("$profile:AZ_Vampire/lights_budget_off.txt"))
				s_Mode = 2;
			Print("[AZ_Vamp] light budget mode " + s_Mode + " (2 = on)");
		}
		return s_Mode == 2;
	}

	static ref array<EntityAI> s_All;
	static ref array<EntityAI> s_Lit;
	static bool s_Ticking;
	static int s_LogAt;
	static int s_Flips;
	static bool s_Bypass;
	static int s_BypassAt;
	static int s_Force;

	static void Add(EntityAI e)
	{
		if (!e || GetGame().IsDedicatedServer())
			return;
		if (!s_All)
		{
			s_All = new array<EntityAI>;
			s_Lit = new array<EntityAI>;
		}
		s_All.Insert(e);
		if (s_All.Count() % 200 == 1)
			Print("[AZ_Vamp] light budget: " + s_All.Count() + " registered, last " + e.GetType());
		if (!s_Ticking)
		{
			s_Ticking = true;
			Print("[AZ_Vamp] light budget: first light registered, " + e.GetType());
		}
	}



	static float s_Acc;
	static void Pulse(float dt)
	{
		if (!s_All)
			return;
		s_Acc += dt * 1000;
		if (s_Acc < TICK_MS)
			return;
		s_Acc = 0;
		Tick();
	}

	static void Remove(EntityAI e)
	{
		if (!s_All)
			return;
		s_All.RemoveItem(e);
		s_Lit.RemoveItem(e);
	}

	static void SetLit(EntityAI e, bool on)
	{
		s_Flips++;
		AZ_VampCandle c = AZ_VampCandle.Cast(e);
		if (c)
		{
			c.AZ_SetLit(on);
			return;
		}
		AZ_VampPumpkinLantern l = AZ_VampPumpkinLantern.Cast(e);
		if (l)
			l.AZ_SetLit(on);
		AZ_VampLitDecor prop = AZ_VampLitDecor.Cast(e);
		if (prop)
			prop.AZ_SetOutdoorLit(on);
	}

	static void Tick()
	{
		if (!s_All)
			return;
		vector cam = GetGame().GetCurrentCameraPosition();
		float onSq = ON_M * ON_M;
		float keepSq = KEEP_M * KEEP_M;
		array<EntityAI> cand = new array<EntityAI>;
		array<float> score = new array<float>;
		for (int i = s_All.Count() - 1; i >= 0; i--)
		{
			EntityAI e = s_All[i];
			if (!e)
			{
				s_All.Remove(i);
				continue;
			}
			float d = vector.DistanceSq(e.GetPosition(), cam);
			float limit = onSq;
			bool wasLit = s_Lit.Find(e) >= 0;
			if (wasLit)
				limit = keepSq;
			if (d <= limit)
			{
				if (wasLit)
					d = d * 0.6;
				cand.Insert(e);
				score.Insert(d);
			}
		}
		int now = GetGame().GetTime();
		if (s_Force == 1)
			s_Bypass = true;
		else if (s_Force == 2)
			s_Bypass = false;
		else if (now >= s_BypassAt)
		{
			s_BypassAt = now + 10000;
			s_Bypass = TEST_ALL || FileExist("$profile:AZ_Vampire/lights_all.txt");
		}
		array<EntityAI> want = new array<EntityAI>;
		if (s_Bypass)
		{
			foreach (EntityAI every : s_All)
				want.Insert(every);
			cand.Clear();
		}
		int cap = Math.Clamp(s_Cap, 0, MAX_LIT);
		s_OtherCount = OtherLights(cam);
		cap = Math.Min(cap, Math.Max(0, MAX_LIT - s_OtherCount));
		while (want.Count() < cap && cand.Count() > 0)
		{
			int best = 0;
			for (int k = 1; k < cand.Count(); k++)
			{
				if (score[k] < score[best])
					best = k;
			}
			bool spaced = true;
			foreach (EntityAI chosen : want)
			{
				if (vector.DistanceSq(chosen.GetPosition(), cand[best].GetPosition()) < POOL_GAP_M * POOL_GAP_M)
					spaced = false;
			}
			if (spaced)
				want.Insert(cand[best]);
			cand.Remove(best);
			score.Remove(best);
		}
		for (int j = s_Lit.Count() - 1; j >= 0; j--)
		{
			EntityAI old = s_Lit[j];
			if (!old)
			{
				s_Lit.Remove(j);
				continue;
			}
			if (want.Find(old) < 0)
			{
				SetLit(old, false);
				s_Lit.Remove(j);
			}
		}
		foreach (EntityAI w : want)
		{
			if (s_Lit.Find(w) < 0)
			{
				SetLit(w, true);
				s_Lit.Insert(w);
			}
		}
		if (now >= s_LogAt)
		{
			s_LogAt = now + LOG_MS;
			Print("[AZ_Vamp] light budget: " + s_Lit.Count() + " lit of " + s_All.Count() + " registered (max " + cap + ", other " + s_OtherCount + ", " + ON_M + " m), fps " + Math.Round(GetGame().GetFps()) + ", switched " + s_Flips + ", bypass " + s_Bypass);
			s_Flips = 0;
		}
	}
}


modded class ScriptedLightBase
{
	void ScriptedLightBase()
	{
		if (!GetGame().IsDedicatedServer())
			AZ_VampLightBudget.Track(this);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampLightBudget.RPC || !GetGame().IsClient())
			return;
		Param1<int> budget = new Param1<int>(4);
		if (ctx.Read(budget))
			AZ_VampLightBudget.s_Cap = Math.Clamp(budget.param1, 0, 8);
	}
}
