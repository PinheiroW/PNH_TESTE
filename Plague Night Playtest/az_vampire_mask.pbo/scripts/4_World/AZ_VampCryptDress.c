






class AZ_Crypt_Moon_Grate extends House
{



	static const float MIST_ON = 0.3;
	static float s_MistRate = 0;
	static const bool MOON_ON = true;
	protected ScriptedLightBase m_AZLight;
	protected ScriptedLightBase m_AZCrawlGlow;
	protected ScriptedLightBase m_AZApseGlow;
	protected ScriptedLightBase m_AZVotives;
	protected ref array<Particle> m_AZMist;
	static AZ_Crypt_Moon_Grate s_Grate;


	static void AZ_ApseShadow(bool on)
	{
		if (s_Grate && s_Grate.m_AZApseGlow)
			s_Grate.m_AZApseGlow.SetCastShadow(on);
		Print("[AZ_Vamp] apse key shadows " + on + " grate " + (s_Grate != null));
	}

	void AZ_MoonOn(bool on)
	{
		if (!m_AZLight)
			return;
		if (on)
			m_AZLight.SetBrightnessTo(AZ_VampMoonLight.AZ_BRIGHT);
		else
			m_AZLight.SetBrightnessTo(0);
	}

	override void EEInit()
	{
		super.EEInit();
		if (GetGame().IsDedicatedServer())
			return;
		s_Grate = this;
		if (MOON_ON)
			m_AZLight = AZ_VampLightGate.Light(AZ_VampMoonLight, GetPosition() - "0 0.3 0");
		if (m_AZLight)
			m_AZLight.SetOrientation("0 -90 0");



		m_AZCrawlGlow = AZ_VampLightGate.Light(AZ_VampCrawlGlow, AZ_VampCrypt.Hall(-5.6, -0.3, 0.7));







		vector keyAt = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 9.7, -2.6, 9.0);
		vector keyTo = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 8.4, 0, 4.0);



		if (AZ_VampCryptLights.REVERT)
			m_AZApseGlow = AZ_VampLightGate.Light(AZ_VampApseGlow, keyAt);
		if (m_AZApseGlow)
			m_AZApseGlow.SetDirection((keyTo - keyAt).Normalized());


		m_AZVotives = AZ_VampLightGate.Light(AZ_VampBallGlow, AZ_VampCrypt.Hall(-6.3, -2.4, 2.9));
		AZ_VampCryptLights.RiverUp();



		AZ_MistSpawn();
		Print("[AZ_Vamp] crypt lights: moon " + (m_AZLight != null) + " crawl " + (m_AZCrawlGlow != null) + " apse " + (m_AZApseGlow != null) + " apse at " + AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 6.5, 0, 7.0));
	}


	void AZ_MistSpawn()
	{
		if (m_AZMist || s_MistRate <= 0)
			return;
		m_AZMist = new array<Particle>;
		float ap = AZ_VampCrypt.APSE_F;
		array<vector> at = new array<vector>;
		for (int mf = 9; mf <= 49; mf += 10)
			at.Insert(AZ_VampCrypt.Nave(mf, 0));
		at.Insert(AZ_VampCrypt.Nave(ap + 3.0, -2.6, AZ_VampCrypt.DAIS_Y));
		at.Insert(AZ_VampCrypt.Nave(ap + 3.0, 2.6, AZ_VampCrypt.DAIS_Y));
		foreach (vector mp : at)
		{
			Particle mist = Particle.PlayInWorld(ParticleList.SPOOKY_MIST, mp);
			if (!mist)
				continue;
			mist.ScaleParticleParamFromOriginal(EmitorParam.BIRTH_RATE, s_MistRate);
			m_AZMist.Insert(mist);
		}
		Print("[AZ_Vamp] crypt mist: " + m_AZMist.Count() + " emitters, rate x" + s_MistRate);
	}



	static void AZ_MistRate(float rate)
	{
		s_MistRate = rate;
		if (!s_Grate)
			return;
		if (!s_Grate.m_AZMist)
		{
			s_Grate.AZ_MistSpawn();
			return;
		}
		foreach (Particle pm : s_Grate.m_AZMist)
		{
			if (pm)
				pm.ScaleParticleParamFromOriginal(EmitorParam.BIRTH_RATE, rate);
		}
		Print("[AZ_Vamp] crypt mist: rate x" + rate + " on " + s_Grate.m_AZMist.Count() + " emitters (fog A/B/C)");
	}

	override void EEDelete(EntityAI parent)
	{
		if (s_Grate == this)
			s_Grate = null;		// AZ-2290: no stale grate ref across a mission reload (hardening, not a proven crash fix)
		super.EEDelete(parent);
		if (m_AZLight)
			m_AZLight.Destroy();
		if (m_AZCrawlGlow)
			m_AZCrawlGlow.Destroy();
		if (m_AZApseGlow)
			m_AZApseGlow.Destroy();
		if (m_AZVotives)
			m_AZVotives.Destroy();
		AZ_VampCryptLights.RiverDown();
		if (m_AZMist)
		{
			foreach (Particle pm : m_AZMist)
			{
				if (pm)
					pm.Stop();
			}
		}
	}
}











class AZ_VampWade
{

	static bool In(PlayerBase p)
	{
		if (!p || !AZ_VampCrypt.Site())
			return false;
		vector pos = p.GetPosition();
		if (!AZ_VampCrypt.Inside(pos))
			return false;
		vector l = AZ_VampCrypt.HallLocal(pos);
		return l[0] > AZ_VampCrypt.HALL_F0 && l[0] < AZ_VampCrypt.APSE_F + 2.5 && Math.AbsFloat(l[1]) < 2.0 && l[2] < -AZ_VampCrypt.LEDGE_H - 0.4;
	}


	static bool Human(PlayerBase p)
	{
		return p.IsAlive();
	}
}

modded class PlayerBase
{
	protected bool m_AZWading;
	protected float m_AZWadeCheck;
	protected int m_AZWadeFlip;

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);



		if (!GetGame().IsDedicatedServer() && !IsControlledPlayer())
			return;
		m_AZWadeCheck -= pDt;
		if (m_AZWadeCheck > 0)
			return;
		m_AZWadeCheck = 0.25;
		bool wade = AZ_VampWade.Human(this) && AZ_VampWade.In(this);


		if (wade == m_AZWading)
		{
			m_AZWadeFlip = 0;
			return;
		}
		m_AZWadeFlip++;
		if (m_AZWadeFlip < 2)
			return;
		m_AZWadeFlip = 0;
		m_AZWading = wade;
		HumanInputController hic = GetInputController();
		if (!hic)
			return;
		if (wade)
		{
			hic.SetErectSpeedLimit(true, DayZPlayerConstants.MOVEMENTIDX_WALK);
			hic.SetCrouchSpeedLimit(true, DayZPlayerConstants.MOVEMENTIDX_WALK);
		}
		else
		{
			hic.SetErectSpeedLimit(false);
			hic.SetCrouchSpeedLimit(false);
		}
	}


	override void OnStepEvent(string pEventType, string pUserString, int pUserInt)
	{
		super.OnStepEvent(pEventType, pUserString, pUserInt);
		if (GetGame().IsDedicatedServer() || !AZ_VampWade.Human(this) || !AZ_VampWade.In(this))
			return;
		SEffectManager.PlaySound("runErc_water_Char_SoundSet", GetPosition());
	}
}





modded class UndergroundHandlerClient
{
	static const string AZ_CRYPT_LIGHTING = "azo\az_vampire_mask\data\lighting\crypt_lighting.cfg";
	static const vector AZ_CRYPT_TRIGGER = "7624 33 8840";
	protected int m_AZLighting = -1;

	override protected void OnTriggerInsiderUpdate()
	{
		super.OnTriggerInsiderUpdate();
		int want = 0;
		if (m_BestTrigger && vector.DistanceSq(m_BestTrigger.GetPosition(), AZ_CRYPT_TRIGGER) < 16)
			want = 1;
		if (want == m_AZLighting)
			return;
		m_AZLighting = want;
		Print("[AZ_Vamp] crypt lighting: " + want + " (1 the crypt's warm preset, 0 vanilla underground), trigger " + m_BestTrigger);
		Weather w = g_Game.GetWeather();
		AZ_VampCryptLook.s_In = want == 1;
		if (want)
		{
			g_Game.GetWorld().LoadUserLightingCfg(AZ_CRYPT_LIGHTING, "AZCrypt");
			m_AZFogOut = w.GetFog().GetActual();
			m_AZVolOut = w.GetDynVolFogDistanceDensity();
			m_AZVolHOut = w.GetDynVolFogHeightDensity();
			Print("[AZ_Vamp] crypt fog in: fog " + m_AZFogOut + " volfog dist " + m_AZVolOut + " height " + m_AZVolHOut + " enabled " + w.IsDynVolFogEnabled());
		}
		else
		{
			g_Game.GetWorld().LoadUserLightingCfg(UNDERGROUND_LIGHTING, "Underground");
			if (m_AZFogOut >= 0)
			{
				w.GetFog().Set(m_AZFogOut, 2, 0);
				w.SetDynVolFogDistanceDensity(m_AZVolOut, 2);
				w.SetDynVolFogHeightDensity(m_AZVolHOut, 2);
			}
		}
	}



	protected float m_AZFogOut = -1;
	protected float m_AZVolOut;
	protected float m_AZVolHOut;
	protected float m_AZFogT;
	protected int m_AZFogLogs;



	protected string m_AZBandSeen = "";

	override protected void ApplyEyeAcco()
	{
		super.ApplyEyeAcco();
		if (AZ_VampBand.Is("F"))
			GetRequester().SetEyeAccommodation(1);
		else if (AZ_VampBand.Is("I"))
			GetRequester().SetEyeAccommodation(Math.Max(m_EyeAcco, 0.3));
		else if (AZ_VampBand.Is("J"))
			GetRequester().SetEyeAccommodation(Math.Max(m_EyeAcco, 0.5));
	}

	override protected void ProcessLighting(float timeSlice)
	{
		super.ProcessLighting(timeSlice);
		if (AZ_VampBand.s_V != m_AZBandSeen)
		{
			m_AZBandSeen = AZ_VampBand.s_V;
			if (AZ_VampBand.Is("C"))
				g_Game.GetWorld().LoadUserLightingCfg(UNDERGROUND_LIGHTING, "Underground");
			else if (AZ_VampBand.Is("G"))
				g_Game.GetWorld().LoadUserLightingCfg(AZ_VampBand.LIGHTING_G, "AZCryptG");
			else if (m_AZLighting == 1)
				g_Game.GetWorld().LoadUserLightingCfg(AZ_CRYPT_LIGHTING, "AZCrypt");
			if (AZ_VampBand.Is("D") && m_AZFogOut >= 0)
			{
				Weather bw = g_Game.GetWeather();
				bw.GetFog().Set(m_AZFogOut, 0, 0);
				bw.SetDynVolFogDistanceDensity(m_AZVolOut, 0);
				bw.SetDynVolFogHeightDensity(m_AZVolHOut, 0);
			}
			Print("[AZ_Vamp] TEST band lighting " + m_AZBandSeen + " crypt preset " + m_AZLighting + " lerp " + m_LightingLerp + " eye " + m_EyeAcco);
		}
		if (m_AZLighting != 1 || AZ_VampBand.Is("D"))
			return;
		m_AZFogT -= timeSlice;
		if (m_AZFogT > 0)
			return;
		m_AZFogT = 1.0;
		Weather w = g_Game.GetWeather();
		if (m_AZFogLogs < 12)
		{
			m_AZFogLogs++;
			Print("[AZ_Vamp] crypt fog now: fog " + w.GetFog().GetActual() + " volfog dist " + w.GetDynVolFogDistanceDensity() + " height " + w.GetDynVolFogHeightDensity() + " lighting lerp " + m_LightingLerp + " target " + m_LightingLerpTarget + " eye " + m_EyeAcco + " presence " + m_Player.m_UndergroundPresence);
		}
		w.GetFog().Set(0.0, 1, 0);
		w.SetDynVolFogDistanceDensity(0.0, 1);
		w.SetDynVolFogHeightDensity(0.0, 1);
	}
}




modded class UndergroundTrigger
{
	static bool s_AZLogged;

	override protected bool CanAddObjectAsInsider(Object object)
	{
		if (!object || !object.IsMan())
			return false;
		ManBase man = ManBase.Cast(object);
		if (!man)
		{
			if (!s_AZLogged)
				Print("[AZ_Vamp] underground trigger: skipped a Man that is no ManBase: " + object.GetType());
			s_AZLogged = true;
			return false;
		}
		return man.IsControlledPlayer();
	}
}


class AZ_VampCryptLook
{
	static bool s_In;
}

modded class AZ_VampConfig
{
	float cryptMist = 0;
}
