






class AZ_VampCryptDark
{
	static const string CFG = "azo\\az_vampire_mask\\data\\lighting\\crypt_lighting.cfg";
	static const string UNDERGROUND = "dz\\data\\lighting\\lighting_underground.txt";
	static const float BLEND_IN_S = 1.5;
	static const float BLEND_OUT_S = 1.0;
	static float s_Lerp;
	static float s_HandlerLerp;
	static float s_HandlerT = -10;
	static bool s_In;
	static int s_Presence = -1;
	static string s_Cfg = "";
	static int s_DayKey = -1;
	static int s_MinKey = -1;
	static bool s_DatePending;
	static float s_DateLoadT = -10;
	static string s_LogLabel = "";
	static bool s_HandlerBuilt;

	static float HandlerLerp()
	{
		if (GetGame().GetTickTime() - s_HandlerT > 0.5)
			return 0;
		return s_HandlerLerp;
	}

	static void Tick(PlayerBase p, float dt)
	{
		bool inside = p.IsAlive() && AZ_VampCrypt.Inside(p.GetPosition());
		int presence = p.m_UndergroundPresence;

		string cfg = CFG;
		string cls = "AZCrypt";
		if (AZ_VampBand.Is("C"))
		{
			cfg = UNDERGROUND;
			cls = "Underground";
		}





		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);





		int minKey = h * 60 + mi;
		int minStep = Math.AbsInt(minKey - s_MinKey);
		if (minStep > 720)
			minStep = 1440 - minStep;
		int dayKey = y * 10000 + mo * 100 + d;
		if (s_MinKey >= 0 && (minStep > 90 || (dayKey != s_DayKey && minStep > 30)))
			s_DatePending = true;
		s_DayKey = dayKey;
		s_MinKey = minKey;
		bool dateJump = false;
		if (s_DatePending && GetGame().GetTickTime() - s_DateLoadT > 3)
		{
			dateJump = true;
			s_DatePending = false;
			s_DateLoadT = GetGame().GetTickTime();
		}


		bool handlerBuilt = s_HandlerBuilt;
		s_HandlerBuilt = false;
		if (inside && !AZ_VampBand.Is("G") && (!s_In || presence != s_Presence || cfg != s_Cfg || dateJump || handlerBuilt))
		{
			GetGame().GetWorld().LoadUserLightingCfg(cfg, cls);
			s_Cfg = cfg;
			Print("[AZ_Vamp] crypt dark: profile " + cls + " loaded, presence " + presence + " lerp " + s_Lerp + " handler " + HandlerLerp() + " date jump " + dateJump + " handler built " + handlerBuilt);
		}
		if (!inside)
			s_Cfg = "";
		if (inside != s_In)
			Print("[AZ_Vamp] crypt dark: inside " + inside + " at " + p.GetPosition() + " presence " + presence + " vampire " + p.m_AZVampire);
		s_In = inside;
		s_Presence = presence;
		float target = 0;
		if (inside)
			target = 1;
		if (target == s_Lerp && target == 0)
			return;
		if (s_Lerp < target)
			s_Lerp = Math.Min(target, s_Lerp + dt / BLEND_IN_S);
		else if (s_Lerp > target)
			s_Lerp = Math.Max(target, s_Lerp - dt / BLEND_OUT_S);
		GetGame().GetWorld().SetUserLightingLerp(Math.Max(s_Lerp, HandlerLerp()));
		LogShot(p, presence);
	}


	static void LogShot(PlayerBase p, int presence)
	{
		if (AZ_VampShotRpc.s_Label == "" || AZ_VampShotRpc.s_Label == s_LogLabel)
			return;
		s_LogLabel = AZ_VampShotRpc.s_Label;
		Print("[AZ_Vamp] crypt dark " + s_LogLabel + ": lerp " + s_Lerp + " handler " + HandlerLerp() + " presence " + presence + " inside " + s_In + " crypt look " + AZ_VampCryptLook.s_In + " vampire " + p.m_AZVampire + " sight " + p.m_AZVampSight + " band " + AZ_VampBand.s_V + " eye " + GetGame().GetWorld().GetEyeAccom());
	}
}

modded class UndergroundHandlerClient
{
	void UndergroundHandlerClient(PlayerBase player)
	{
		AZ_VampCryptDark.s_HandlerBuilt = true;
	}

	override protected void ProcessLighting(float timeSlice)
	{
		super.ProcessLighting(timeSlice);
		AZ_VampCryptDark.s_HandlerLerp = m_LightingLerp;
		AZ_VampCryptDark.s_HandlerT = GetGame().GetTickTime();
		if (AZ_VampCryptDark.s_Lerp > m_LightingLerp)
			g_Game.GetWorld().SetUserLightingLerp(AZ_VampCryptDark.s_Lerp);
	}
}

modded class PlayerBase
{
	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (GetGame().IsClient() && IsControlledPlayer())
			AZ_VampCryptDark.Tick(this, pDt);
	}
}
