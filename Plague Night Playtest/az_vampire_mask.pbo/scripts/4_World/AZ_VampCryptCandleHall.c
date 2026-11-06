









class AZ_VampCandleHall
{
	static const float F0 = -20.4;
	static const float F1 = -16.8;
	static const float HALF_R = 5.0;
	static const float SHARE_M = 15.0;
	static const int SNUFF_MS = 10000;
	static const int PLAYER_CD_MS = 300000;
	static const int GLOBAL_CD_MS = 45000;
	static const int TICK_MS = 500;
	static const int RPC_SNUFF = 4702247;

	static bool s_Built;
	static int s_Gen;
	static int s_ReadyAt;
	static int s_Snuffs;
	static ref map<string, int> s_Next;
	static ref array<PlayerBase> s_Probe;

	static vector Center()
	{
		return AZ_VampCrypt.Hall((F0 + F1) * 0.5, 0, 1.0);
	}

	static float Radius()
	{
		return 8.0;
	}

	static void Build()
	{
		s_Next = new map<string, int>;
		if (AZ_VampCryptTraps.s_RoomSealed)
		{
			Print("[AZ_Vamp] candle hall: not built (arrival gallery sealed)");
			return;
		}

		for (int m = 0; m < 2; m++)
		{
			float mf = -19.4 + 1.8 * m;
			AZ_VampCrypt.PutCrypt("AZ_VampHallVotive", AZ_VampCrypt.Hall(mf + 0.45, -5.2, 0), AZ_VampCrypt.HALL_YAW);
			AZ_VampCrypt.PutCrypt("AZ_VampHallVotive", AZ_VampCrypt.Hall(mf - 0.45, 5.2, 0), AZ_VampCrypt.HALL_YAW);
		}
		s_Built = true;
		s_Gen++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, s_Gen);
		Print("[AZ_Vamp] candle hall built: 4 votives, stretch f " + F0 + ".." + F1);
	}

	static bool InStretch(vector pos)
	{
		vector l = AZ_VampCrypt.HallLocal(pos);
		return l[0] > F0 && l[0] < F1 && Math.AbsFloat(l[1]) < HALF_R && l[2] > -1.0 && l[2] < 3.0;
	}

	static void Tick(int gen)
	{
		if (gen != s_Gen)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, false, gen);
		int now = GetGame().GetTime();
		if (now < s_ReadyAt || !AZ_VampCryptTraps.On())
			return;
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		if (s_Probe)
		{
			foreach (PlayerBase d : s_Probe)
			{
				if (d)
					list.Insert(d);
			}
		}
		foreach (PlayerBase p : list)
		{
			if (!p || !AZ_VampCryptTraps.IsHuman(p) || !InStretch(p.GetPosition()))
				continue;
			string uid = AZ_VampState.Uid(p);
			if (s_Next.Contains(uid) && now < s_Next.Get(uid))
				continue;
			Snuff(p, list, now);
			return;
		}
	}

	static void Snuff(PlayerBase p, array<PlayerBase> list, int now)
	{
		s_ReadyAt = now + GLOBAL_CD_MS;
		s_Snuffs++;
		vector c = Center();
		int sent = 0;
		foreach (PlayerBase h : list)
		{
			if (!h || !AZ_VampCryptTraps.IsHuman(h) || vector.Distance(h.GetPosition(), p.GetPosition()) > SHARE_M)
				continue;
			s_Next.Set(AZ_VampState.Uid(h), now + PLAYER_CD_MS);
			if (!h.GetIdentity())
				continue;
			GetGame().RPCSingleParam(h, RPC_SNUFF, new Param3<vector, float, int>(c, Radius(), SNUFF_MS), true, h.GetIdentity());
			sent++;
		}
		Print("[AZ_Vamp] candle hall snuffed by " + AZ_VampState.Uid(p) + ", " + sent + " clients told");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Whisper, 1500, false, p);
	}

	static void Whisper(PlayerBase p)
	{
		if (p && p.IsAlive())
			AZ_VampWhisperLines.Circle(p, p.GetPosition(), p.GetDirection(), 3);
	}


	static void Probe()
	{
		if (AZ_VampCryptTraps.s_RoomSealed)
		{
			Print("[AZ_Vamp] candle probe: SKIP (entrance off)");
			Print("[AZ_Vamp] candle probe: done");
			return;
		}
		if (!s_Built)
		{
			Print("[AZ_Vamp] candle probe: FAIL not built");
			return;
		}
		s_ReadyAt = 0;
		s_Next.Clear();
		PlayerBase vamp = Dummy(0);
		PlayerBase human = Dummy(1);
		if (!vamp || !human)
		{
			Print("[AZ_Vamp] candle probe: FAIL spawn");
			return;
		}
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		vamp.SetPosition(AZ_VampCrypt.Hall(-18.5, 1.0, 0.05));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 1500, false, 1, vamp, human, s_Snuffs);
	}

	static PlayerBase Dummy(int seed)
	{
		vector at = AZ_VampCrypt.Hall(-14.0, 2.0 + seed, 0.05);
		PlayerBase d = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Male(seed + 7), at, false, true));
		if (!d)
			return null;
		AZ_VampDress.Survivor(d, seed + 7);
		d.SetPosition(at);
		if (!s_Probe)
			s_Probe = new array<PlayerBase>;
		s_Probe.Insert(d);
		return d;
	}

	static string Verdict(bool ok)
	{
		if (ok)
			return "PASS";
		return "FAIL";
	}

	static void ProbeStep(int step, PlayerBase vamp, PlayerBase human, int before)
	{
		bool ok;
		switch (step)
		{
			case 1:
				ok = s_Snuffs == before;
				Print("[AZ_Vamp] candle probe: vampire in the gallery -> snuffs " + before + " -> " + s_Snuffs + " " + Verdict(ok) + " (expect none)");
				vamp.SetPosition(AZ_VampCrypt.Hall(-14.0, 2.0, 0.05));
				human.SetPosition(AZ_VampCrypt.Hall(-18.5, -1.0, 0.05));
				break;
			case 2:
				ok = s_Snuffs == before + 1;
				Print("[AZ_Vamp] candle probe: human in the gallery -> snuffs " + before + " -> " + s_Snuffs + " " + Verdict(ok) + " (expect one)");
				s_ReadyAt = 0;
				before = s_Snuffs;
				break;
			case 3:
				ok = s_Snuffs == before;
				Print("[AZ_Vamp] candle probe: same human still there -> snuffs " + before + " -> " + s_Snuffs + " " + Verdict(ok) + " (expect none: per-player cooldown)");
				s_Next.Clear();
				s_ReadyAt = 0;
				AZ_VampState.Cfg().cryptTraps = false;
				break;
			case 4:
				ok = s_Snuffs == before;
				Print("[AZ_Vamp] candle probe: kill switch, human there -> snuffs " + before + " -> " + s_Snuffs + " " + Verdict(ok) + " (expect none)");
				AZ_VampState.Cfg().cryptTraps = true;
				foreach (PlayerBase d : s_Probe)
				{
					if (d)
						GetGame().ObjectDelete(d);
				}
				s_Probe = null;
				s_Next.Clear();
				s_ReadyAt = 0;
				Print("[AZ_Vamp] candle probe: done");
				return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 1500, false, step + 1, vamp, human, before);
	}
}


class AZ_VampHallVotive extends AZ_VampVotive
{
	static ref array<AZ_VampHallVotive> s_All;
	static bool s_Ticking;

	override void EEInit()
	{
		super.EEInit();
		if (m_AZLight)
		{
			m_AZLight.Destroy();
			m_AZLight = null;
		}
		if (GetGame().IsDedicatedServer())
			return;
		if (!s_All)
			s_All = new array<AZ_VampHallVotive>;
		s_All.Insert(this);
		if (!s_Ticking)
		{
			s_Ticking = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ClientTick, 500, true);
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (s_All)
			s_All.RemoveItem(this);
		super.EEDelete(parent);
	}


	float AZ_LitBeyondF()
	{
		return -12.0;
	}

	bool AZ_Want(PlayerBase p)
	{
		if (!p)
			return false;
		vector l = AZ_VampCrypt.HallLocal(p.GetPosition());
		return l[0] < AZ_LitBeyondF() && Math.AbsFloat(l[1]) < 6.25 && l[2] > -2.0 && l[2] < 6.0;
	}

	static void ClientTick()
	{
		if (!s_All)
			return;
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		foreach (AZ_VampHallVotive v : s_All)
		{
			if (v)
				v.AZ_SetLit(v.AZ_Want(me));
		}
	}

	void AZ_SetLit(bool lit)
	{
		if (lit && !m_AZLight)
			m_AZLight = AZ_VampLightGate.AtPoint(AZLightType(), this, "light");
		else if (!lit && m_AZLight)
		{
			m_AZLight.Destroy();
			m_AZLight = null;
		}
	}
}



class AZ_VampTrapVotive extends AZ_VampHallVotive
{
	override float AZ_LitBeyondF()
	{
		return -1.0;
	}
}


modded class AZ_VampLitDecor
{
	protected float m_AZSnuffFrom;

	void AZ_Snuff(int ms)
	{
		if (!m_AZLight || m_AZSnuffFrom > 0)
			return;
		m_AZSnuffFrom = m_AZLight.m_Brightness;
		m_AZLight.SetBrightnessTo(0);
		Particle.PlayOnObject(ParticleList.CAMP_SMALL_SMOKE, this, "0 0.15 0");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_SnuffEnd, ms, false);
	}

	void AZ_SnuffEnd()
	{
		if (m_AZLight && m_AZSnuffFrom > 0)
			m_AZLight.SetBrightnessTo(m_AZSnuffFrom);
		m_AZSnuffFrom = 0;
	}
}


modded class AZ_Crypt_Brazier
{
	protected float m_AZSnuffFrom;

	void AZ_Snuff(int ms)
	{
		if (m_AZSnuffFrom > 0)
			return;
		m_AZSnuffFrom = 1;
		if (m_AZLight)
		{
			m_AZSnuffFrom = m_AZLight.m_Brightness;
			m_AZLight.SetBrightnessTo(0);
		}
		if (m_AZFire)
			m_AZFire.Stop();
		m_AZFire = null;
		Particle.PlayOnObject(ParticleList.CAMP_SMALL_SMOKE, this, "0 0.9 0");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_SnuffEnd, ms, false);
	}

	void AZ_SnuffEnd()
	{
		if (m_AZLight && m_AZSnuffFrom > 0)
			m_AZLight.SetBrightnessTo(m_AZSnuffFrom);
		m_AZSnuffFrom = 0;
		if (!m_AZFire)
			m_AZFire = Particle.PlayOnObject(AZFireParticle(), this, AZFireAt());
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampCandleHall.RPC_SNUFF || !GetGame().IsClient())
			return;
		Param3<vector, float, int> p;
		if (!ctx.Read(p))
			return;
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(p.param1, p.param2, near, null);
		foreach (Object o : near)
		{
			AZ_VampLitDecor lit = AZ_VampLitDecor.Cast(o);
			if (lit)
			{
				lit.AZ_Snuff(p.param3);
				continue;
			}
			AZ_Crypt_Brazier br = AZ_Crypt_Brazier.Cast(o);
			if (br)
				br.AZ_Snuff(p.param3);
		}
	}
}
