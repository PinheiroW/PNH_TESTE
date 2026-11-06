


class AZ_PPEBloodMoon extends PPERequester_GameplayBase
{
	static const float FADE_TIME = 6;
	static const float G_DROP = 0.0;
	static const float B_DROP = 0.0;
	static const float VIGNETTE = 0.0;
	static const float VIGNETTE_RED = 0.0;
	static const int L_VIGNETTE = 399;
	protected float m_T;
	protected bool m_In;

	void FadeIn()
	{
		m_In = true;
		m_T = 0;
		Start();
	}

	void FadeOut()
	{
		m_In = false;
		m_T = 0;
		SetRequesterUpdating(true);
	}

	override protected void OnUpdate(float delta)
	{
		super.OnUpdate(delta);
		if (m_T > FADE_TIME)
		{
			if (!m_In)
				Stop();
			return;
		}
		if (m_T == 0)
			Print("[AZ_Vamp] blood moon tint " + m_In);
		m_T += delta;
		float k = Math.Clamp(m_T / FADE_TIME, 0, 1);
		if (!m_In)
			k = 1 - k;
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, VIGNETTE * k, L_VIGNETTE, PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTECOLOR, {VIGNETTE_RED * k, 0.0, 0.0, 0.0}, L_VIGNETTE, PPOperators.ADD);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {1.0, 1.0 - G_DROP * k, 1.0 - B_DROP * k, 0.0}, PPEGlow.L_23_TOXIC_TINT, PPOperators.MULTIPLICATIVE);
	}

	static AZ_PPEBloodMoon Get()
	{
		AZ_PPEBloodMoon r = AZ_PPEBloodMoon.Cast(PPERequesterBank.GetRequester(AZ_PPEBloodMoon));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPEBloodMoon);
			r = AZ_PPEBloodMoon.Cast(PPERequesterBank.GetRequester(AZ_PPEBloodMoon));
			if (r)
				r.Revalidate();
		}
		return r;
	}



	void Revalidate()
	{
		m_Valid = PPERequesterBank.VerifyRequester(this);
		Print("[AZ_Vamp] blood moon tint requester valid " + m_Valid);
	}
}

modded class MissionGameplay
{
	protected float m_AZMoonCheck;
	protected bool m_AZMoonOn;
	protected ref EffectSound m_AZMoonBell;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		m_AZMoonCheck -= timeslice;
		if (m_AZMoonCheck > 0)
			return;
		m_AZMoonCheck = 1;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;


		if (p.m_AZBloodMoon && AZ_VampMoonCue.Claim(p.m_AZMoonRevision))
			m_AZMoonBell = SEffectManager.PlaySound("AZ_VampMidnightBell_SoundSet", p.GetPosition());
		bool on = p.IsAlive() && p.m_AZVampire && p.m_AZBloodMoon && !AZ_VampCryptLook.s_In && !AZ_VampBand.Is("T");
		if (on == m_AZMoonOn)
			return;
		m_AZMoonOn = on;
		AZ_PPEBloodMoon r = AZ_PPEBloodMoon.Get();
		if (!r)
			return;
		if (on)
			r.FadeIn();
		else
			r.FadeOut();
	}

}



class AZ_VampMoonCue
{
	static int s_PulseAt = -10000;
	static ref map<string, int> s_Heard = new map<string, int>;
	// Backstop: the activation bell rings at most once per 10 real minutes,
	// even if the server hands out a new revision every tick.
	static const int CUE_COOLDOWN_MS = 600000;
	static bool Claim(int revision)
	{
		if (revision <= 0)
			return false;
		if (s_PulseAt > 0 && GetGame().GetTime() - s_PulseAt < CUE_COOLDOWN_MS)
			return false;
		string address;
		int port;
		string key = "local";
		if (GetGame().GetHostAddress(address, port))
		{
			int endpointHash = address.Hash();
			key = endpointHash.ToString() + "_" + port;
		}
		else if (GetGame().IsMultiplayer())
			return false;
		if (!s_Heard.Contains(key))
		{
			int heard = 0;
			string line;
			FileHandle read = OpenFile("$profile:AZ_Vampire/moon_revision_" + key + ".txt", FileMode.READ);
			if (read)
			{
				FGets(read, line);
				CloseFile(read);
				heard = line.ToInt();
			}
			s_Heard.Set(key, heard);
		}
		if (s_Heard.Get(key) == revision)
			return false;
		s_Heard.Set(key, revision);
		MakeDirectory("$profile:AZ_Vampire");
		FileHandle write = OpenFile("$profile:AZ_Vampire/moon_revision_" + key + ".txt", FileMode.WRITE);
		if (write)
		{
			FPrintln(write, revision.ToString());
			CloseFile(write);
		}
		s_PulseAt = GetGame().GetTime();
		Print("[AZ_VampMoonClient] activation cue revision=" + revision);
		return true;
	}
}
