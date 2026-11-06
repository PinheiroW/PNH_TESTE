



modded class PlayerBase
{
	static const int RPC_STARE = 4702251;
	static const int RPC_EMBERS = 4702252;
	static const int RPC_HOLD = 4702253;
	static const int RPC_COMPEL = 4702254;
	static const float EMBER_K = 0.12;
	static const int EMBER_FADE_MS = 2000;
	protected int m_AZHoldUntil;
	static const int COMPEL_TURN_MS = 2000;
	protected int m_AZStareUntil;
	protected int m_AZStareFrom;
	protected vector m_AZStareAt;
	protected float m_AZStareSign = 1.0;
	protected float m_AZStareErrLast = -1;
	protected int m_AZStareCheckAt;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == RPC_EMBERS && GetGame().IsClient())
		{
			Param1<int> em = new Param1<int>(0);
			if (ctx.Read(em))
				AZ_VampEmbers.Start(em.param1);
			return;
		}
		if (rpc_type == RPC_HOLD && GetGame().IsClient() && IsControlledPlayer())
		{
			Param1<int> hd = new Param1<int>(0);
			if (ctx.Read(hd))
			{
				m_AZHoldUntil = GetGame().GetTime() + hd.param1;
				Print("[AZ_Vamp] river stare client: a kneeler held for " + hd.param1 + " ms");
			}
			return;
		}
		if ((rpc_type != RPC_STARE && rpc_type != RPC_COMPEL) || !GetGame().IsClient() || !IsControlledPlayer())
			return;
		Param2<vector, int> sp = new Param2<vector, int>(vector.Zero, 0);
		if (!ctx.Read(sp))
			return;
		m_AZStareAt = sp.param1;
		m_AZStareFrom = GetGame().GetTime();
		m_AZStareUntil = m_AZStareFrom + sp.param2;
		m_AZStareSign = 1.0;
		m_AZStareErrLast = -1;
		m_AZStareCheckAt = m_AZStareFrom + 400;
		Print("[AZ_Vamp] river stare client: drawn to her face " + m_AZStareAt + " for " + sp.param2 + " ms");
		if (rpc_type == RPC_STARE)
			AZ_VampStareBeat.Start(this, sp.param2);
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (m_AZHoldUntil != 0 && GetGame().IsClient() && IsControlledPlayer())
		{
			if (GetGame().GetTime() >= m_AZHoldUntil)
				m_AZHoldUntil = 0;
			else if (GetInputController())
				GetInputController().OverrideMovementSpeed(HumanInputControllerOverrideType.ONE_FRAME, 0);
		}
		if (m_AZStareUntil == 0 || !GetGame().IsClient() || !IsControlledPlayer())
			return;
		int now = GetGame().GetTime();
		if (now >= m_AZStareUntil)
		{
			m_AZStareUntil = 0;
			if (AZ_VampStareBeat.s_Who == this)
			{
				AZ_VampStareBeat.Stop();
				m_AZBeatGen++;
				AZ_BeatTick(m_AZBeatGen);
			}
			Print("[AZ_Vamp] river stare client: released");
			return;
		}
		HumanInputController hic = GetInputController();
		if (!hic)
			return;
		hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ONE_FRAME, 0);
		vector cam = GetGame().GetCurrentCameraPosition();
		vector have = GetGame().GetCurrentCameraDirection();
		vector want = m_AZStareAt - cam;
		want.Normalize();
		float pitchErr = Math.Asin(Math.Clamp(want[1], -1, 1)) - Math.Asin(Math.Clamp(have[1], -1, 1));
		float yawErr = Math.Atan2(want[0], want[2]) - Math.Atan2(have[0], have[2]);
		if (yawErr > Math.PI)
			yawErr = yawErr - Math.PI2;
		if (yawErr < -Math.PI)
			yawErr = yawErr + Math.PI2;

		float since = now - m_AZStareFrom;
		float k = Math.Clamp(since / COMPEL_TURN_MS, 0.15, 1.0);
		float step = Math.Clamp(pDt * 3.0 * k, 0, 0.5);
		hic.OverrideAimChangeX(HumanInputControllerOverrideType.ONE_FRAME, yawErr * step * m_AZStareSign);
		hic.OverrideAimChangeY(HumanInputControllerOverrideType.ONE_FRAME, pitchErr * step * m_AZStareSign);

		float err = Math.AbsFloat(pitchErr) + Math.AbsFloat(yawErr);

		if (now >= m_AZStareCheckAt && !Camera.GetCurrentCamera())
		{
			if (m_AZStareErrLast >= 0 && err > m_AZStareErrLast + 0.02 && m_AZStareSign > 0)
			{
				m_AZStareSign = -1.0;
				Print("[AZ_Vamp] river stare client: aim sign flipped (error " + m_AZStareErrLast + " -> " + err + ")");
			}
			m_AZStareErrLast = err;
			m_AZStareCheckAt = now + 400;
		}
	}
}




class AZ_VampEmbers
{
	static ref map<ScriptedLightBase, float> s_Was;
	static int s_Gen;

	static array<ScriptedLightBase> Fires()
	{
		array<ScriptedLightBase> all = new array<ScriptedLightBase>;
		if (AZ_VampCryptLights.s_Trap)
			all.InsertAll(AZ_VampCryptLights.s_Trap);
		if (AZ_VampCryptLights.s_Nave)
			all.InsertAll(AZ_VampCryptLights.s_Nave);
		if (AZ_VampCryptLights.s_Dais)
			all.InsertAll(AZ_VampCryptLights.s_Dais);
		if (AZ_VampCryptLights.s_Bowls)
			all.InsertAll(AZ_VampCryptLights.s_Bowls);
		if (AZ_VampCryptLights.s_River)
			all.InsertAll(AZ_VampCryptLights.s_River);
		return all;
	}

	static void Start(int ms)
	{
		if (s_Was)
			End(s_Gen);
		s_Gen++;
		s_Was = new map<ScriptedLightBase, float>;
		array<ScriptedLightBase> fires = Fires();
		foreach (ScriptedLightBase l : fires)
		{
			if (l && !s_Was.Contains(l) && !AZ_VampMoonLight.Cast(l))
				s_Was.Set(l, l.m_Brightness);
		}
		for (int i = 1; i <= 4; i++)
		{
			int at = i * PlayerBase.EMBER_FADE_MS / 4;
			float k = i * 0.25;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Step, at, false, s_Gen, k);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(End, ms, false, s_Gen);
		Print("[AZ_Vamp] embers: " + s_Was.Count() + " fires sink for " + ms + " ms");
	}

	static void Step(int gen, float k)
	{
		if (gen != s_Gen || !s_Was)
			return;
		float f = 1.0 - (1.0 - PlayerBase.EMBER_K) * k;
		foreach (ScriptedLightBase l, float b : s_Was)
		{
			if (l && b > 0)
				l.SetBrightnessTo(Math.Max(0.02, b * f));
		}
	}

	static void End(int gen)
	{
		if (gen != s_Gen || !s_Was)
			return;
		foreach (ScriptedLightBase l, float b : s_Was)
		{
			if (l && b > 0)
				l.SetBrightnessTo(b);
		}
		Print("[AZ_Vamp] embers: " + s_Was.Count() + " fires flare back");
		s_Was = null;
	}
}





class AZ_VampStareBeat
{
	static const string SET = "AZ_VampStareBeat_SoundSet";
	static const int FIRST_MS = 250;
	static const float GAP0_MS = 780;
	static const float GAP_GROW = 1.09;
	static const float VOL0 = 0.4;
	static const float VOL_GROW = 1.12;
	static int s_Until;
	static int s_Gen;
	static ref EffectSound s_Snd;
	static PlayerBase s_Who;

	static void Start(PlayerBase p, int ms)
	{
		if (!GetGame().ConfigIsExisting("CfgSoundSets " + SET))
			return;
		s_Gen++;
		s_Who = p;
		s_Until = GetGame().GetTime() + ms;
		float gap = GAP0_MS * ms / 8000.0;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, FIRST_MS, false, s_Gen, gap, VOL0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Whisper, WHISPER_MS, false, s_Gen);
	}


	static const string WHISPER_SET = "AZ_VampLine_stare_SoundSet";
	static const int WHISPER_MS = 3000;

	static void Whisper(int gen)
	{
		if (gen != s_Gen || !s_Who || GetGame().GetTime() >= s_Until || !GetGame().ConfigIsExisting("CfgSoundSets " + WHISPER_SET))
			return;
		EffectSound w = SEffectManager.PlaySound(WHISPER_SET, s_Who.GetPosition() + Vector(0, 1.6, 0));
		if (w)
			w.SetSoundAutodestroy(true);
		Print("[AZ_Vamp] river stare client: her whisper");
	}

	static void Tick(int gen, float gap, float vol)
	{
		if (gen != s_Gen || !s_Who || GetGame().GetTime() >= s_Until)
			return;
		s_Snd = SEffectManager.PlaySound(SET, s_Who.GetPosition() + Vector(0, 1.6, 0));
		if (s_Snd)
		{
			s_Snd.SetSoundVolume(vol);
			s_Snd.SetSoundAutodestroy(true);
		}
		int next = gap;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, next, false, gen, gap * GAP_GROW, Math.Min(1.0, vol * VOL_GROW));
	}

	static void Stop()
	{
		s_Gen++;
		s_Until = 0;
		if (s_Snd)
			s_Snd.SoundStop();
		s_Snd = null;
	}


	static bool Playing()
	{
		return s_Until != 0 && GetGame().GetTime() < s_Until;
	}
}
